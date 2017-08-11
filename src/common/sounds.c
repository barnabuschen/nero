#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  


#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */
#include  <malloc.h>  
#include  <math.h>
#include  <assert.h>  
#include <time.h>

// #include <cairo.h>
#include "sounds.h"  
#include <hiredis/hiredis.h>
#include "../NeuralNetwork/NeuralNetwork.h"
// #include "readUTF8File.h"

#include "../common/fileOperating.h"

#include "../NeuralNetwork/NeuralOperating.h"
#include "../common/type.h"

/* func     : endian judge 
 * return   : 0-big-endian othes-little-endian 
 */  

static nero_8int  file_path_getcwd[FILEPATH_MAX]="/tmp";/*保存当前目录*/
static nero_8int  TmpDataArea[1024]; 



#define INPUT_DIMEN 1640
#define LAYER_DIMEN 128
#define OUTPUT_DIMEN 3
#define LAYER 4

#define PI   3.14159265358979
#define TPI  6.28318530717959     /* PI*2 */
#define NUMCHANS 24   //在MEL刻度下等分成   个频带，滤波器组的组数
#define MaxDataNum 160000





int IS_LITTLE_ENDIAN(void)   
{  
    int __dummy = 1;  
    return ( *( (unsigned char*)(&(__dummy) ) ) );  
}  
  // LITTLE_ENDIAN小字节序、低字节序）,即低位字节排放在内存的低地址端，高位字节排放在内存的高地址端
unsigned int readHeader(void *dst, signed int size, signed int nmemb, FILE *fp)   
{  
    unsigned int n, s0, s1, err;  
    unsigned char tmp, *ptr;  
      
    if ((err = fread(dst, size, nmemb, fp)) != nmemb)   
    {  
        return err;  
    }  
    if (!IS_LITTLE_ENDIAN() && size > 1)   
    {  
        //printf("big-endian \n");  
        ptr = (unsigned char*)dst;  
        for (n=0; n<nmemb; n++)   
        {  
            for (s0=0, s1=size-1; s0 < s1; s0++, s1--)   
            {  
                tmp = ptr[s0];  
                ptr[s0] = ptr[s1];  
                ptr[s1] = tmp;  
            }  
            ptr += size;  
        }  
    }  
    else  
    {  
        //printf("little-endian \n");  
    }  
      
    return err;  
}  
  
void dumpWavInfo(WAV_INFO wavInfo)  
{  
    printf("compressionCode:%d \n",wavInfo.header.compressionCode);  
    printf("numChannels:%d \n",wavInfo.header.numChannels);  
    printf("sampleRate:%d \n",wavInfo.header.sampleRate);  
    printf("bytesPerSecond:%d \n",wavInfo.header.bytesPerSecond);  
    printf("blockAlign:%d \n",wavInfo.header.blockAlign);  
    printf("bitsPerSample:%d \n",wavInfo.header.bitsPerSample);  
  
}  
  
int wavInputOpen(WAV_INFO *pWav, const char *filename)  
{  
    signed int offset;  
    WAV_INFO *wav = pWav ;  
  
    if (wav == NULL)   
    {  
      printf("wavInputOpen:Unable to allocate WAV struct.\n");  
      goto error;  
    }  
    wav->fp = fopen(filename, "r");  
    if (wav->fp == NULL)   
    {  
      printf("wavInputOpen:Unable to open wav file. %s\n", filename);  
      goto error;  
    }  
  
    /* RIFF标志符判断 */  
    if (fread(&(wav->header.riffType), 1, 4, wav->fp) != 4)   
    {  
      printf("couldn't read RIFF_ID\n");  
      goto error;  /* bad error "couldn't read RIFF_ID" */  
    }  
    if (strncmp("RIFF", wav->header.riffType, 4))   
    {  
        printf("RIFF descriptor not found.\n") ;  
        goto error;  
    }  
    // printf("Find RIFF \n");  
      
    /* Read RIFF size. Ignored. */  
    readHeader(&(wav->header.riffSize), 4, 1, wav->fp);  
    // printf("wav->header.riffSize:%d \n",wav->header.riffSize);  
  
    /* WAVE标志符判断 */  
    if (fread(&wav->header.waveType, 1, 4, wav->fp) !=4)   
    {  
        printf("couldn't read format\n");  
        goto error;  /* bad error "couldn't read format" */  
    }  
    if (strncmp("WAVE", wav->header.waveType, 4))   
    {  
        printf("WAVE chunk ID not found.\n") ;  
        goto error;  
    }  
    // printf("Find WAVE \n");  
  
    /* fmt标志符判断 */  
    if (fread(&(wav->header.formatType), 1, 4, wav->fp) != 4)   
    {  
        printf("couldn't read format_ID\n");  
        goto error;  /* bad error "couldn't read format_ID" */  
    }  
    if (strncmp("fmt", wav->header.formatType, 3))   
    {  
        printf("fmt chunk format not found.\n") ;  
        goto error;  
    }  
    // printf("Find fmt \n");  
    //pos 0x10
    readHeader(&wav->header.formatSize, 4, 1, wav->fp);  // Ignored  
    // printf("wav->header.formatSize:%d \n",wav->header.formatSize);  
  
    /* read  info ------the key msg*/  
    readHeader(&(wav->header.compressionCode), 2, 1, wav->fp); //格式种类 
    readHeader(&(wav->header.numChannels), 2, 1, wav->fp);  //通道数 
    readHeader(&(wav->header.sampleRate), 4, 1, wav->fp);  //采样率 
    readHeader(&(wav->header.bytesPerSecond), 4, 1, wav->fp);  //传输速率 ,波形数据传输速率（每秒平均字节数）
    readHeader(&(wav->header.blockAlign), 2, 1, wav->fp);  //数据块的对齐，即DATA数据块长度
    readHeader(&(wav->header.bitsPerSample), 2, 1, wav->fp);  //采样精度-PCM位宽 
  
    offset = wav->header.formatSize - 16;  
  
    /* Wav format extensible */  
    if (wav->header.compressionCode == 0xFFFE)   
    {  
        static const unsigned char guidPCM[16] = {  
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,  
            0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71  
        };
        unsigned short extraFormatBytes, validBitsPerSample;  
        unsigned char guid[16];  
        signed int i;  
  
        /* read extra bytes */  
        readHeader(&(extraFormatBytes), 2, 1, wav->fp);  
        offset -= 2;  
  
        if (extraFormatBytes >= 22)   
        {
            readHeader(&(validBitsPerSample), 2, 1, wav->fp);  
            readHeader(&(wav->channelMask), 4, 1, wav->fp);  
            readHeader(&(guid), 16, 1, wav->fp);  
  
            /* check for PCM GUID */  
            for (i = 0; i < 16; i++) if (guid[i] != guidPCM[i]) break;  
            if (i == 16) wav->header.compressionCode = 0x01;  
  
            offset -= 22;  
        }
    }
    // printf("wav->header.compressionCode:%d \n",wav->header.compressionCode);  
  
    /* Skip rest of fmt header if any. */  
    for (;offset > 0; offset--)   
    {  
        fread(&wav->header.formatSize, 1, 1, wav->fp);  
    }  
  
 
    do   
    {  
        /* Read data chunk ID */  
        if (fread(wav->header.dataType, 1, 4, wav->fp) != 4)   
        {  
            // printf("Unable to read data chunk ID.\n");  
            free(wav);  
            goto error;  
        }  
        /* Read chunk length. */  
        readHeader(&offset, 4, 1, wav->fp);  
  
        /* Check for data chunk signature. */  
        if (strncmp("data", wav->header.dataType, 4) == 0)   
        {  
            // printf("Find data \n");  
            wav->header.dataSize = offset;  
            //************if find data ,end  while,over*************
            break;  
        }  
          
        /* Jump over non data chunk.----------  */  
        for (;offset > 0; offset--)   
        {  
            fread(&(wav->header.dataSize), 1, 1, wav->fp);  
        }  
    } while (!feof(wav->fp));  
    printf("wav->header.dataSize:%d \n",wav->header.dataSize);     
      
    /* return success */  
    if (wav->fp)   
    {  
        fclose(wav->fp);  
        wav->fp = NULL;  
    }      
    return 0;  
  
/* Error path */  
error:  
    if (wav)   
    {  
      if (wav->fp)   
      {  
        fclose(wav->fp);  
        wav->fp = NULL;  
      }  
      //free(wav);  
    }  
    return -1;   
}  
// 将文件WaveFileCSV 中列出的wave都分别写入db中
int putWaveDataInDB(char * WaveFileCSV)
{
    int i;
    // nero_8int * fileName3="/home/jty/nero/nero/src/data/sound/wave.csv";
    char *fName;
        char  tmpFName[1024];

    for(i=1; ;i++)
    {
        fName = getLineInFile( WaveFileCSV,i);//返回获取的字符串
        if(fName != NULL  &&  strlen(fName) > 2)
        {
            sprintf(tmpFName,"/data/sound/%s",fName);
            // printf(" fName:%s\n",fName );
            // printf(" tmpFName:%s\n",file_path_getcwd );
              putDataInDB_wav(tmpFName);

        }
        else
        {
            break;

        }
    }
    if(fName)
    {
        free(fName);
    }


    return 0;
}


// 将wav文件的数据部分输入一个redis的列表中去，列表名就是wav文件名
//如果已经有该列表了，就先清空数据
int putDataInDB_wav(char * fileName)
{
    redisContext* c;
    redisReply* r_tmp1;
    redisReply* r;

    WAV_INFO wavInfo;  
    nero_8int  fileName_[FILEPATH_MAX];
    nero_8int RedisListName[FILEPATH_MAX];   

    nero_s32int res,dataNum,blockAlign,bytesPerSecond,bitsPerSample;

    nero_us32int strlenMax,flength,strlenMin,strLen,pos,frames ;
    nero_s32int fd;
    nero_8int *mapped_mem, * p,*dataAddress,*end2,*end ;

    short int channelA;
    short int channelB;
    // nero_8int       *linStart,*linEnd;
    void * start_addr = 0;


    c = redisConnect("127.0.0.1", 6379);
    if (c->err) 
    {
        printf("Failed to redisConnect \n");
        redisFree(c);
        return;
    }
    const char* command1 = "select 0";
     r = (redisReply*)redisCommand(c,command1);
    if (NULL == r) 
    {
        redisFree(c);
        return;
    }
    if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK") == 0)) 
    {
        printf("Failed to execute command[%s].\n",command1);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    sprintf(RedisListName,"%s",fileName);
    printf(" RedisListName: %s  \n",RedisListName );  
    // r = (redisReply*)redisCommand(c,"flushdb");
    // freeReplyObject(r);
    // printf("flushdb.\n");

    r = (redisReply*)redisCommand(c,"EXISTS %s",RedisListName);
    if ( r->type == REDIS_REPLY_INTEGER && r->integer == 1)
    {
        // printf("Failed to execute EXISTS.\n");
        freeReplyObject(r);
        r = (redisReply*)redisCommand(c,"del %s",RedisListName);

    }
    freeReplyObject(r);
    // printf("r->type =%d.  r->integer=%d\n",r->type,r->integer);
    
    // printf("flushdb.\n");


// open wav file  
    getcwd(file_path_getcwd,FILEPATH_MAX);
    sprintf(fileName_,"%s%s",file_path_getcwd,fileName);
    res = wavInputOpen(&wavInfo, fileName_);  
    // write data into DB
    //首先找到data的位置


    fd = open(fileName_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    flength = lseek(fd, 1, SEEK_END);
//  write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
    lseek(fd, 0, SEEK_SET);
/*  write(fd, "7", 1);*/
    mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
    MAP_PRIVATE,       //不允许其它进程访问此内存区域
    fd, 0);
    printf(" fileName_: %s  flength=%d \n",fileName_, flength);
    p=mapped_mem;
    end=p+flength-1;

    
    // memcpy( TmpDataArea, p ,1024);
    // TmpDataArea[1023] = 0;
     // printf(" %s\n", TmpDataArea);
    // dataAddress =strstr(TmpDataArea,"data");//strstr(str1,str2)用于判断字符串str2是否是str1的子串。如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。
   dataAddress = NULL;
    while( p < end)
    {
        // printf("%x%x%x%x \n",*p ,*(p+1) ,*(p+2) ,*(p+3));
        if(  *(p)  ==  0x64  )
        {
            if(  *(p+1)  ==  0x61  )
            {
                if(  *(p+2)  ==  0x74  )
                {
                    if(  *(p+3)  ==  0x61 )
                    {
                        dataAddress = p +8 ;
                        break;
                    }
                }
            }
        }
        p++;
    } 
    dumpWavInfo( wavInfo)  ;
    dataNum = 0;
    blockAlign = wavInfo.header.blockAlign;//每个样本数据的字节数
    bitsPerSample = wavInfo.header.bitsPerSample;//采样精度   PCM位宽
    
    bytesPerSecond = wavInfo.header.bytesPerSecond;
    // printf(" blockAlign:%d  ,  bitsPerSample:%d, IS_LITTLE_ENDIAN =%d,short int;%d\n", blockAlign,bitsPerSample,IS_LITTLE_ENDIAN() ,sizeof(short int) );



    //注意，list是先入后出的，注意数据的位置
    r = (redisReply*)redisCommand(c,"LPUSH %s %x",RedisListName,wavInfo.header.sampleRate);
    freeReplyObject(r);
    r = (redisReply*)redisCommand(c,"LPUSH %s %x",RedisListName,wavInfo.header.bytesPerSecond);
    freeReplyObject(r);

    if( dataAddress != NULL  &&  blockAlign == 4  &&  bitsPerSample == 16 )
    {
        p  =   dataAddress;
        frames =  wavInfo.header.dataSize / blockAlign   ;
        //双通道的话只记录一个通道的数据
        // unsigned int    sampleRate;     //4byte,采样率  
        // unsigned int    bytesPerSecond; //4byte,传输速率  
        // unsigned short  bitsPerSample;  //2byte,采样精度   

        while(1)
        {
            // RIFF文件是按照little-endian字节顺序写入的
            channelA  =  * (( short int  *)p);
            p = p +2;
            // channelB  =  * (( short int  *)p);
            p = p +2;
            dataNum++;//帧 numbers
            r = (redisReply*)redisCommand(c,"LPUSH %s %x",RedisListName,channelA);
            freeReplyObject(r);           
             // printf("%x,%x,", channelA,channelB);
            if( p >= end /*|| *(p) ==0x0a*/  ||  dataNum >=frames )
            {
                break;
            }  
        }
    }
    else if(dataAddress != NULL  &&  blockAlign == 2  &&  bitsPerSample == 16 )
    {
        p  =   dataAddress;
        frames =  wavInfo.header.dataSize / blockAlign   ;
        while(1)
        {
            // RIFF文件是按照little-endian字节顺序写入的
            channelA  =  * (( short int  *)p);
            p = p +2;
            dataNum++;//帧 numbers
            r = (redisReply*)redisCommand(c,"LPUSH %s %x",RedisListName,channelA);
            freeReplyObject(r);            
             // printf("%x,", channelA);
            if( p >= end /*|| *(p) ==0x0a*/  ||  dataNum >=frames)
            {
                break;
            }  
        }
    }
    else
    {
      printf(" can not find data str:%s \n", dataAddress);

    }
    printf(" end of p :%x  dataNum=%d  %x,%x\n", *p,dataNum ,  p,end);
    // printf(" end of p :%x  dataNum=%d  %x,%x\n", *p,dataNum ,  p);
    close(fd);
    munmap(mapped_mem, flength);
    redisFree(c);
    return 0;
}

int copy_wav( char * fileName)  
{  
    WAV_INFO wavInfo;  
    nero_8int  fileName_[FILEPATH_MAX];
    char   newfileName[FILEPATH_MAX];

    char   pic_path[FILEPATH_MAX];

    char   type[]="2.wav";
    // char   pic[]="hello.png";
    nero_us32int len,res;

    getcwd(file_path_getcwd,FILEPATH_MAX);
    sprintf(fileName_,"%s%s",file_path_getcwd,fileName);
    // sprintf(pic_path,"%s%s",file_path_getcwd,pic);

    res = wavInputOpen(&wavInfo, fileName_);  
    //print msg
    dumpWavInfo( wavInfo);
    return 0;  
    strcpy(newfileName,fileName_);
    len = strlen(newfileName);
     // printf(" len of %s=%d \n",newfileName, len);
    if(len > 4  && len <FILEPATH_MAX )
    {

        newfileName[len - 4] = 0 ;//name.wav

        // printf(" len of %s=%d \n",  newfileName,strlen(newfileName));
        strcat(newfileName,type);
        // printf(" newfileName;%s \n",  newfileName);
        copyFileTo_(fileName_,newfileName);   
    }
    return 0;  
}  
static  float *hamWin_;

/*读取PCM文件到wavdata中*/
void LoadFile(char *s, struct Wave *w)
{
    FILE *fp;
    fp = fopen(s, "rb");
    if (!fp)
    {
        printf("can not open this file:%s\n",s);
        exit(0);
    }
    unsigned char ch1, ch2, ch3, ch4;
    int i,MaxDataNum_;
    float * buf ;
    buf = w->wavdata;
    for (i = 0; i < 40; i++)
    {
        ch1 = fgetc(fp);
    }
    MaxDataNum_= w->nSample;//不管文件的实际数据个数，只读取MaxDataNum_个数据
    ch1 = fgetc(fp); ch2 = fgetc(fp); ch3 = fgetc(fp); ch4 = fgetc(fp);
    //get total data bytes
    w->nSample = (ch2 * 16 * 16 + ch1) + (ch4 * 16 * 16 + ch3) * 16 * 16 * 16 * 16;
    // w->nSample /= 2;
    printf("nSample nums is %ld  \n", w->nSample);
    if(w->nSample < MaxDataNum_)
    {

         printf("LoadFile:nSample nums is error %ld  \n", w->nSample);
         fclose(fp);
         return;
    }
    // w->nRow = (w->nSample - w->frSize) / w->frRate + 1;
    
    for (i = 0; i<MaxDataNum_; i++)
    // for (i = 0; i<w->nSample; i++)
    {
        ch1 = fgetc(fp); //每次读取两个字符，存在数组ch中
        ch2 = fgetc(fp);
        //if (i % 8 == 0)      //每行输出16个字符对应的十六进制数
        //printf("\n");
        float temp = ch2 * 16 * 16 + ch1;
        if (temp < 32768){
            //printf("%d ", temp);
            *buf++ = temp;
            //printf("%lf ", (float)(ch2 * 16 * 16 + ch1) / 32767);
        }
        else{
            //printf("%d ",temp - 65535 - 1);
            *buf++ = temp - 65535 - 1;
            //printf("%lf ", (float)((ch2 * 16 * 16 + ch1)-65535-1) / 32768);
        }
    }
    fclose(fp);
}
/*从wavdata中提取当前帧*/
void GetWave(float *buf, struct Wave *w)
{
    int k;
//      struct Wave
// {
//     int nSample;//wav中样本个数
//     int frSize;//一帧中的样本数
//     int frIdx;//当前帧位置
//     int frRate;//帧移
//     float *wavdata;
//     int nRow;
//     float *Rdata;
// };
    //w->frSize = 400;
    if ((w->frIdx + w->frSize > w->nSample ) &&  (w->frIdx - w->nSample <  w->frSize))
    {
        // printf("GetWave: attempt to read past end of buffer\n");
        for (k = 0; k < w->frSize; k++)//不足的补零
        {
            buf[k] = 0;
        }
        for (k = 0; w->frIdx + k < w->nSample; k++)
        {
            buf[k] = w->wavdata[w->frIdx + k];
        }

    }
    else
    {
        for (k = 0; k < w->frSize; k++)//w->frSize = 400;
        {
            buf[k] = w->wavdata[w->frIdx + k];
        }
        

    }
    w->frIdx += w->frRate;//w->frRate = 160
    // printf("w->frIdx =%d\n",w->frIdx);
}
//就是减去一个平均值
void ZeroMeanFrame(float *frame)
{
    int size, i;
    float sum = 0.0, off;
    size = frame[0];
    for (i = 1; i <= size; i++) sum += frame[i];
    off = sum / size;
    for (i = 1; i <= size; i++) frame[i] -= off;
}
void PreEmphasise(float *frame, float k)
{
    /*
    Pre-emphasis,预加重。与之对应的是De-emphasis去加重。是噪声整形技术在模拟信号的处理中，
    一项关于噪声整形技术原理的技术。所谓预加重是指在信号发送之前，先对模拟信号的高频分量进行适当的提升。
    收到信号之后，再对信号进行逆处理，即去加重，对高频分量进行适当的衰减。这种预加重与去加重技术可以
    使信号在传输中高频损耗的影响降低，也可以使噪声的频谱发生变化，这就是模拟降噪的原理。*/
    int i;
    float preE = k;
    int size = frame[0];
    for (i = size; i >= 2; i--)
        frame[i] -= frame[i - 1] * preE;
    frame[1] *= 1.0 - preE;
}
void Ham(float *frame)
{
    int frameSize = frame[0];
    int i;
    float a;
    float *hamWin;
    if(hamWin_ == NULL)
    {
        printf("Ham error :null hamWin \n" );
        exit(0);
    }
    hamWin = hamWin_;
    a = TPI / (frameSize - 1);
    int b = cos(a * 0);
    for (i = 1; i <= frameSize; i++)
        hamWin[i] = 0.54 - 0.46 * cos(a*(i - 1));
    for (i = 1; i <= frameSize; i++)
    {
        frame[i] *= hamWin[i];
    }
    // free(hamWin);
}

float Mel(int k, float fres)
{
    return 1127 * log(1 + (k - 1)*fres);
}
float WarpFreq(float fcl, float fcu, float freq, float minFreq, float maxFreq, float alpha)
{
    if (alpha == 1.0)
        return freq;
    else {
        float scale = 1.0 / alpha;
        float cu = fcu * 2 / (1 + scale);
        float cl = fcl * 2 / (1 + scale);

        float au = (maxFreq - cu * scale) / (maxFreq - cu);
        float al = (cl * scale - minFreq) / (cl - minFreq);

        if (freq > cu)
            return  au * (freq - cu) + scale * cu;
        else if (freq < cl)
            return al * (freq - minFreq) + minFreq;
        else
            return scale * freq;
    }
}
struct FBankInfo InitFBank(struct IOConfig *cf)
{
    int numChans = NUMCHANS; 
    int usePower = 0; 
    int takeLogs = 1; 
    int sampPeriod = SOURCERATE_MFCC;//sampPeriod要change by  wav  hz
    float alpha = 1; 
    int warpLowCut = 0; 
    int warpUpCut = 0;
    struct FBankInfo fb;
    float mlo, mhi, ms, melk;
    int k, chan, maxChan, Nby2;
    int doubleFFT = 0;


    /* Save sizes to cross-check subsequent usage */
    fb.frameSize = cf->frSize;
    fb.numChans = numChans;//现在高不清楚这里到底对不对
    fb.sampPeriod = sampPeriod;//
    fb.usePower = usePower;
    fb.takeLogs = takeLogs;
    /* Calculate required FFT size */
    fb.fftN = 2;
    while (fb.frameSize>fb.fftN)
        fb.fftN *= 2;
    if (doubleFFT)//不执行
        fb.fftN *= 2;
    Nby2 = fb.fftN / 2;
    fb.fres = 1.0E7 / (sampPeriod * fb.fftN * 700.0);
    maxChan = numChans + 1;
    /* set lo and hi pass cut offs if any */
    fb.klo = 2; fb.khi = Nby2;       /* apply lo/hi pass filtering */
    mlo = 0; mhi = Mel(Nby2 + 1, fb.fres);


    /* Create vector of fbank centre frequencies */
    fb.cf = (float*)malloc(sizeof(float)*maxChan + 1);
    fb.cf[0] = maxChan;
    ms = mhi - mlo;
    for (chan = 1; chan <= maxChan; chan++) {
        if (alpha == 1.0) {
            fb.cf[chan] = ((float)chan / (float)maxChan)*ms + mlo;
        }
        else {
            /* scale assuming scaling starts at lopass */
            float minFreq = 700.0 * (exp(mlo / 1127.0) - 1.0);
            float maxFreq = 700.0 * (exp(mhi / 1127.0) - 1.0);
            float cf = ((float)chan / (float)maxChan) * ms + mlo;

            cf = 700 * (exp(cf / 1127.0) - 1.0);

            fb.cf[chan] = 1127.0 * log(1.0 + WarpFreq(warpLowCut, warpUpCut, cf, minFreq, maxFreq, alpha) / 700.0);
        }
    }

    /* Create loChan map, loChan[fftindex] . lower channel index */
    fb.loChan = (float*)malloc(sizeof(float)*Nby2 + 1);
    fb.loChan[0] = Nby2;
    for (k = 1, chan = 1; k <= Nby2; k++){
        melk = Mel(k, fb.fres);
        if (k<fb.klo || k>fb.khi) fb.loChan[k] = -1;
        else {
            while (fb.cf[chan] < melk  && chan <= maxChan) ++chan;
            fb.loChan[k] = chan - 1;
        }
    }

    /* Create vector of lower channel weights */
    fb.loWt = (float*)malloc(sizeof(float)*Nby2 + 1);
    fb.loWt[0] = Nby2;
    for (k = 1; k <= Nby2; k++) {
        chan = fb.loChan[k];
        if (k<fb.klo || k>fb.khi) fb.loWt[k] = 0.0;
        else {
            if (chan>0)
                fb.loWt[k] = ((fb.cf[chan + 1] - Mel(k, fb.fres)) /
                (fb.cf[chan + 1] - fb.cf[chan]));
            else
                fb.loWt[k] = (fb.cf[1] - Mel(k, fb.fres)) / (fb.cf[1] - mlo);
        }
    }
    /* Create workspace for fft */
    fb.x = (float*)malloc(sizeof(float)*fb.fftN + 1);
    fb.x[0] = fb.fftN;
    return fb;
}

void FFT(float *s, int invert)
{
    int ii, jj, n, nn, limit, m, j, inc, i;
    double wx, wr, wpr, wpi, wi, theta;
    double xre, xri, x;

    n = s[0];
    nn = n / 2; j = 1;
    for (ii = 1; ii <= nn; ii++) {
        i = 2 * ii - 1;
        if (j>i) {
            xre = s[j]; xri = s[j + 1];
            s[j] = s[i];  s[j + 1] = s[i + 1];
            s[i] = xre; s[i + 1] = xri;
        }
        m = n / 2;
        while (m >= 2 && j > m) {
            j -= m; m /= 2;
        }
        j += m;
    };
    limit = 2;
    while (limit < n) {
        inc = 2 * limit; theta = TPI / limit;
        if (invert) theta = -theta;
        x = sin(0.5 * theta);
        wpr = -2.0 * x * x; wpi = sin(theta);
        wr = 1.0; wi = 0.0;
        for (ii = 1; ii <= limit / 2; ii++) {
            m = 2 * ii - 1;
            for (jj = 0; jj <= (n - m) / inc; jj++) {
                i = m + jj * inc;
                j = i + limit;
                xre = wr * s[j] - wi * s[j + 1];
                xri = wr * s[j + 1] + wi * s[j];
                s[j] = s[i] - xre; s[j + 1] = s[i + 1] - xri;
                s[i] = s[i] + xre; s[i + 1] = s[i + 1] + xri;
            }
            wx = wr;
            wr = wr * wpr - wi * wpi + wr;
            wi = wi * wpr + wx * wpi + wi;
        }
        limit = inc;
    }
    if (invert)
        for (i = 1; i <= n; i++)
            s[i] = s[i] / nn;

}
void Realft(float *s)
{
    int n, n2, i, i1, i2, i3, i4;
    double xr1, xi1, xr2, xi2, wrs, wis;
    double yr, yi, yr2, yi2, yr0, theta, x;

    n = s[0] / 2; n2 = n / 2;
    theta = PI / n;
    FFT(s, 0);
    x = sin(0.5 * theta);
    yr2 = -2.0 * x * x;
    yi2 = sin(theta); yr = 1.0 + yr2; yi = yi2;
    for (i = 2; i <= n2; i++) {
        i1 = i + i - 1;      i2 = i1 + 1;
        i3 = n + n + 3 - i2; i4 = i3 + 1;
        wrs = yr; wis = yi;
        xr1 = (s[i1] + s[i3]) / 2.0; xi1 = (s[i2] - s[i4]) / 2.0;
        xr2 = (s[i2] + s[i4]) / 2.0; xi2 = (s[i3] - s[i1]) / 2.0;
        s[i1] = xr1 + wrs * xr2 - wis * xi2;
        s[i2] = xi1 + wrs * xi2 + wis * xr2;
        s[i3] = xr1 - wrs * xr2 + wis * xi2;
        s[i4] = -xi1 + wrs * xi2 + wis * xr2;
        yr0 = yr;
        yr = yr * yr2 - yi  * yi2 + yr;
        yi = yi * yr2 + yr0 * yi2 + yi;
    }
    xr1 = s[1];
    s[1] = xr1 + s[2];
    s[2] = 0.0;
}
void Wave2FBank(float *s, float *fbank, struct FBankInfo info)
{
    const float melfloor = 1.0;
    int k, bin;
    float t1, t2;   /* real and imag parts */
    float ek;      /* energy of k'th fft channel */

    float te = 0.0;
    for (k = 1; k <= info.frameSize; k++)
        te += (s[k] * s[k]);
    /* Apply FFT */
    for (k = 1; k <= info.frameSize; k++)
        info.x[k] = s[k];    /* copy to workspace */
    for (k = info.frameSize + 1; k <= info.fftN; k++)
        info.x[k] = 0.0;   /* pad with zeroes */
    Realft(info.x);                            /* take fft */

    /* Fill filterbank channels */
    int i = 0;
    for (i = 1; i <= fbank[0]; i++)
        fbank[i] = 0.0;
    for (k = info.klo; k <= info.khi; k++) {             /* fill bins */
        t1 = info.x[2 * k - 1]; t2 = info.x[2 * k];
        if (info.usePower)
            ek = t1*t1 + t2*t2;
        else
            ek = sqrt(t1*t1 + t2*t2);
        bin = info.loChan[k];
        t1 = info.loWt[k] * ek;
        if (bin > 0) fbank[bin] += t1;
        if (bin < info.numChans) fbank[bin + 1] += ek - t1;
        //printf("k:%d bin:%d info.loWt:%f fbank[bin]:%f\n", k, bin, info.loWt[k], fbank[bin]);

    }

    /* Take logs */
    if (info.takeLogs)
        for (bin = 1; bin <= info.numChans; bin++) {
            t1 = fbank[bin];
            if (t1 < melfloor) t1 = melfloor;
            fbank[bin] = log(t1);
        }
}

void ConvertFrame(struct IOConfig *cf, struct Wave *w)
{
    float re, rawte = 0.0, te, *p, cepScale = 1.0;
    int i, bsize = 0;
    char buf[50];
    int rawE;//boolen
    cf->frIdx = w->frIdx; 
    cf->frSize = w->frSize; 
    cf->frRate = w->frRate;
    cf->preEmph = 0.97;
    ZeroMeanFrame(cf->s);//零均值处理
    PreEmphasise(cf->s, cf->preEmph);//预加重处理
    Ham(cf->s);//加窗 此处可以优化，创建ham窗多次十分耗费时间,可以用全局申请ham的内存
    cf->fbInfo = InitFBank(cf);
            // printf("test 8 \n" );

    cf->fbank = (float*)malloc(sizeof(float)* NUMCHANS);
    cf->fbank[0] = NUMCHANS;
    Wave2FBank(cf->s, cf->fbank, cf->fbInfo);//提取NUMCHANS为40的fbank

}
void linkdata(struct IOConfig *cf, struct Wave *w, int k)
{
    for (int i = 0; i < NUMCHANS; i++)
    {
        *(w->Rdata + i + (k*NUMCHANS)) = *(cf->fbank + 1 + i);

    }
}
void zeromean(struct Wave *w)
{
    int i, j;
    float sum[NUMCHANS];
    int n = w->nRow;
    for (i = 0; i < NUMCHANS; i++)
    {
        sum[i] = 0.0;
        for (j = 0; j < n; j++)
        {
            sum[i] += *(w->Rdata + j*NUMCHANS + i);
        }
        sum[i] = sum[i] / n;
    }
    for (i = 0; i < NUMCHANS*n; i++)
    {
        *(w->Rdata + i) -= sum[i%NUMCHANS];
    }
}
//return data in RdataArray :NUMCHANS * w->nRow  ,return  w->nRow   ,w->nRow =  ..
int  filter_bank(char *s,int waveSampleRate,int  SampleNums  , float * RdataArray )
{
    /*初始化*/
    /*采样个数为16028*/
    /*由HTK，采样率为25ms，if ,则帧长为400，默认帧移为160
    如果wave文件的hz为44100hz，那么帧长=【25ms *  44100  】/  1000 ms = 1102.5

    由于语音信号在10-30ms认为是稳定的，则可设置帧长为80~240点。帧移可以设置为帧长的1/2.

分帧
先将N个采样点集合成一个观测单位，称为帧。
通常情况下N的值为256或512，涵盖的时间约为20~30ms左右。
为了避免相邻两帧的变化过大，因此会让两相邻帧之间有一段重叠区域，此重叠区域包含了M个取样点，
通常M的值约为N的1/2或1/3。通常语音识别所采用语音信号的采样频率为8KHz或16KHz，以8KHz来说，
若帧长度为256个采样点，则对应的时间长度是256/8000 1000=32ms。
    */
    int j, m, e, x;
    struct Wave *w;
    //waveSampleRate  = 16000
    if(SampleNums <  (2*WINDOWSIZE_MFCC  * waveSampleRate/1000)  &&   RdataArray != NULL)
    {

        printf("Too little   SampleNums :%d\n",SampleNums);
        return 0;
    }

    w = (struct Wave*)malloc(sizeof(struct Wave));
    w->frIdx = 0; 
    w->frRate =   TARGETRATE_MFCC  * waveSampleRate/1000 ;
    w->frSize =   WINDOWSIZE_MFCC  * waveSampleRate/1000 ;
    w->nSample = SampleNums;
    w->nRow = (w->nSample - w->frSize) / w->frRate + 1;//=996
    // printf("w->nRow =%d\n",w->nRow);
    // printf("  test 1 \n");
    w->wavdata = (float*)malloc(sizeof(float) * w->nSample);
    LoadFile(s, w);
    // w->Rdata = (float*)malloc(sizeof(float)*NUMCHANS*w->nRow);//#define NUMCHANS 40
    w->Rdata = RdataArray;
    // printf("w->Rdata address =%x\n",w->Rdata);
    struct IOConfig *cf;
    cf = (struct IOConfig*)malloc(sizeof(struct IOConfig));
    cf->s = (float*)malloc(sizeof(float) * w->frSize + 1);
    /*读PCM文件到wavdata中，可以直接读取缓存数据，此处是为了方便PC测试*/

    //used in void Ham(float *frame)
    hamWin_ = (float*)malloc(sizeof(float) * w->frSize );
    // printf("  test 2\n");
    // for (int k = 0; k < w->nRow + 1; k++)//这个加一是否需要值得考虑以下,如果不加，很大可能会丢弃最后面的一部分数据
    for (int k = 0; k < w->nRow  ; k++)   
    {
        /*分帧,计算帧能量*/
        // printf("w->nRow =%d\n",w->nRow);
        cf->s[0] = w->frSize;

        GetWave(cf->s + 1, w);/*从wavdata中提取当前帧, 不足的补零*/
        

        /*  cf->curVol 貌似无用,delete it
        for (j = 1, m = e = 0.0; j <= w->frSize; j++) 
        {
            x = (int)cf->s[j];
            m += x; 
            e += x*x;
        }
        m = m / w->frSize; 
        e = e / w->frSize - m*m;
        if (e>0.0) e = 10.0*log10(e / 0.32768);
        else e = 0.0;
         cf->curVol = e;

        */
        /*处理*/
        ConvertFrame(cf, w);
        // printf("  test 3 \n");
        linkdata(cf, w, k);
    }
    // printf("  test 4 \n");
    zeromean(w);
    // for (int i = 0; i < NUMCHANS*w->nRow; i++)
    // {
    //     if (i%NUMCHANS == 0)
    //         printf("\n%d：\n", i / NUMCHANS);
    //          printf("%f ", *(w->Rdata + i));

    // }
    if( cf->fbInfo.cf != NULL)
    {

        free(cf->fbInfo.cf);
    }
    if(cf->fbInfo.loChan  != NULL)
    {

         free(cf->fbInfo.loChan);
    }
    if(cf->fbInfo.loWt  != NULL)
    {

        free(cf->fbInfo.loWt);
    }
    if(cf->fbInfo.x  != NULL)
    {

        free(cf->fbInfo.x);
    }
    if(cf->fbank  != NULL)
    {

      free(cf->fbank);
    }
    if(cf->s  != NULL)
    {

         free(cf->s);
    }
    if(cf   != NULL)
    {

          free(cf );
    }


    // if(w->Rdata  != NULL)
    // {

    //      free(w->Rdata);
    // }
    if(w->wavdata  != NULL)
    {

          free(w->wavdata );
    }  
    //     free(cf->fbInfo.cf);
    // free(cf->fbInfo.loChan);
    // free(cf->fbInfo.loWt);
    // free(cf->fbInfo.x);
    // free(cf->fbank);
    // free(cf->s);
    free(w);

    if(hamWin_ != NULL)
    {

          free(hamWin_ );
    }
    // free(hamWin_);
    return w->nRow;
}


int frank_test()
{
    char * filename = "/data/sound/001101_L2.wav";
                        // /home/jty/nero/nero/src/data/sound
    float   * RdataArray ;
    int SampleNums ,i,j ;



    WAV_INFO wavInfo;  
    nero_8int  fileName_[FILEPATH_MAX];
    nero_us32int res;

    getcwd(file_path_getcwd,FILEPATH_MAX);
    sprintf(fileName_,"%s%s",file_path_getcwd,filename);
    res = wavInputOpen(&wavInfo, fileName_);  
    dumpWavInfo( wavInfo);
    // return 0;

    SampleNums = wavInfo.header.dataSize;   //这是数据的总量，区别单双通道
    int waveSampleRate = wavInfo.header.sampleRate; 
    int frRate =   TARGETRATE_MFCC  * waveSampleRate/1000 ;
    int frSize =   WINDOWSIZE_MFCC  * waveSampleRate/1000 ;

    if(wavInfo.header.numChannels != 1)
        SampleNums = SampleNums /wavInfo.header.numChannels ;

    int nRow = (SampleNums -  frSize) /  frRate + 1; 
    if(wavInfo.header.numChannels != 1  ||   wavInfo.header.sampleRate != 16000)
    {
         printf("wrong wave file:%s  %d  %d\n",fileName_,wavInfo.header.numChannels,wavInfo.header.sampleRate);
        return 0;

    }
    printf("nRow = %d  ,SampleNums =%d\n",nRow,SampleNums);

    RdataArray =(float*)malloc(sizeof(float) * NUMCHANS *  nRow);
    // return 0;
    // printf("RdataArray:%x \n",RdataArray);
    if(RdataArray == NULL)
    {
         printf("\n\n\nRdataArray is NULL \n");
        return 0;
    }

    int row  = filter_bank(fileName_,waveSampleRate, SampleNums ,   RdataArray );
    // for(i=0,j=0 ;i < nRow; i++ )

    // printf("nRow = %d  ,SampleNums =%d\n",nRow,SampleNums);
    for(i=0,j=0 ;i < nRow; i++ )
    {
        for( j=0; j < NUMCHANS; j++)
        {
             printf(" %f ",RdataArray[i*NUMCHANS +j]);

        }
         printf("\n\n\n\n");

    }
    free(RdataArray);
    return 0;
}



















