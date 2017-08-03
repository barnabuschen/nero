#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  


#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */

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
    r = (redisReply*)redisCommand(c,"flushdb");
    freeReplyObject(r);
    printf("flushdb.\n");
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

    sprintf(RedisListName,"%s",fileName);
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
