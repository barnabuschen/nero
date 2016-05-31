#ifndef NeroTASK_H
#define NeroTASK_H
#include "../common/type.h"
//#include "../tools/readUTF8File.h"
#include "../NeuralNetwork/NeuralNetwork.h"



#define TFFDataWidth  100   //可以保存的数据条数
#define TFFDataLength 300   //每条数据数据的最大长度

typedef struct TaskFileFormat
{
        nero_s32int order;/*保存命令种类*/  
        nero_s32int MsgCount;/*信息条数（分割符号分隔），包括命令*/  
        nero_8int   msgSeparator;/*信息分割符号，一般用空格*/
        nero_8int   orderSeparator;/*不同命令的分割符号，一般用换行*/
        nero_8int   str[3500];
        nero_us8int      data[TFFDataWidth][TFFDataLength];/*二维数组指针*/
        
}TFF;










void obtainOrderFromTFF(TFF * tff);/*从TFF中分析得到命令后在函数里面直接发送就行了*/


// void ReadTaskFromTxt();
void ReadTaskFromTxt(nero_8int  * fileNameInpt);
void getMsgInToTFF(struct TaskFileFormat *  tff);
void JustDoTask();
void CreateBaseKindOfShu();
void ModifyBaseKindOfShu();
//every line  in  file is a  data string ,every char in str  is  a  char  obj
void ReadTaskFromTxtByline(nero_8int  * FileName);


#endif
