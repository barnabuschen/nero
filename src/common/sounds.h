#ifndef WAV_H
#define WAV_H
#include "../common/type.h"
#include <stdio.h>  

  

/* RIFF WAVE file struct. 
 * For details see WAVE file format documentation  
 * (for example at http://www.wotsit.org). 
 */  
typedef struct WAV_HEADER_S  
{  
    char            riffType[4];    //4byte,资源交换文件标志:RIFF     
    unsigned int    riffSize;       //4byte,从下个地址到文件结尾的总字节数   
    char            waveType[4];    //4byte,wave文件标志:WAVE     
    char            formatType[4];  //4byte,波形文件标志:FMT    
    unsigned int    formatSize;     //4byte,音频属性(compressionCode,numChannels,sampleRate,bytesPerSecond,blockAlign,bitsPerSample)所占字节数  
    unsigned short  compressionCode;//2byte,编码格式(1-线性pcm-WAVE_FORMAT_PCM,WAVEFORMAT_ADPCM)  
    unsigned short  numChannels;    //2byte,通道数  
    unsigned int    sampleRate;     //4byte,采样率  
    unsigned int    bytesPerSecond; //4byte,传输速率  
    unsigned short  blockAlign;     //2byte,数据块的对齐  
    unsigned short  bitsPerSample;  //2byte,采样精度  
    char            dataType[4];    //4byte,数据标志:data  
    unsigned int    dataSize;       //4byte,从下个地址到文件结尾的总字节数，即除了wav header以外的pcm data length  
}WAV_HEADER;  
  
typedef struct WAV_INFO_S  
{  
  WAV_HEADER    header;  
  FILE          *fp;  
  unsigned int  channelMask;  
}WAV_INFO;  
  


struct Wave
{
    int nSample;//wav中样本个数
    int frSize;//一帧中的样本数
    int frIdx;//当前帧位置
    int frRate;//帧移
    float *wavdata;//语音数据
    int nRow;
    float *Rdata;//是fbank结果数据
};
 struct FBankInfo
{
    int frameSize;       /* speech frameSize */
    int numChans;        /* number of channels */
    long sampPeriod;     /* sample period 一个样本持续的时间


    */
    int fftN;            /* fft size */
    int klo, khi;         /* lopass to hipass cut-off fft indices */
    int usePower;    /* use power rather than magnitude *///boolen
    int takeLogs;    /* log filterbank channels *///boolen
    float fres;          /* scaled fft resolution */
    float *cf;           /* array[1..pOrder+1] of centre freqs */
    float *loChan;     /* array[1..fftN/2] of loChan index */
    float *loWt;         /* array[1..fftN/2] of loChan weighting */
    float *x;            /* array[1..fftN] of fftchans */
};
 struct IOConfig
{
    // float curVol;/* current volume dB (0.0-100.0) */
    float preEmph;
    int frSize;//一帧中的样本数
    int frIdx;//当前帧位置
    int frRate;//帧移
    float *fbank;
    struct FBankInfo fbInfo;
    float *s;//帧数据

};


/*读取PCM文件到wavdata中*/
void LoadFile(char *s, struct Wave *w);
void GetWave(float *buf, struct Wave *w);
void ZeroMeanFrame(float *frame);
void PreEmphasise(float *frame, float k);
void Ham(float *frame);
float Mel(int k, float fres);
float WarpFreq(float fcl, float fcu, float freq, float minFreq, float maxFreq, float alpha);
struct FBankInfo InitFBank(struct IOConfig *cf);
void FFT(float *s, int invert);
void Realft(float *s);
void Wave2FBank(float *s, float *fbank, struct FBankInfo info);
void ConvertFrame(struct IOConfig *cf, struct Wave *w);
void linkdata(struct IOConfig *cf, struct Wave *w, int k);
void zeromean(struct Wave *w);
int filter_bank(char *s,int waveSampleRate,int  SampleNums   , float * RdataArray );
int frank_test();
int putWaveDataInDB(char * WaveFileCSV);



#define WINDOWSIZE_MFCC    25  //单位ms
#define TARGETRATE_MFCC    10    //单位ms
#define SOURCERATE_MFCC    625    //一个样本持续的时间,16k hz  ---62500 ns 

                                //                  44.1 hz  ---22675.73 ns 


// #define INPUT_DIMEN 1640
// #define LAYER_DIMEN 128
// #define OUTPUT_DIMEN 3
// #define LAYER 4

// #define PI   3.14159265358979
// #define TPI  6.28318530717959     /* PI*2 */
// #define NUMCHANS 40   //在MEL刻度下等分成   个频带，滤波器组的组数
// #define MaxDataNum 160000







int copy_wav( char * fileName);
int putDataInDB_wav(char * fileName);
int wavInputOpen(WAV_INFO *pWav, const char *filename);
void dumpWavInfo(WAV_INFO wavInfo);












#endif  