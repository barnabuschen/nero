#ifndef NeroTASK_H
#define NeroTASK_H
#include "../common/type.h"
//#include "../tools/readUTF8File.h"
#include "../NeuralNetwork/NeuralNetwork.h"


typedef struct TaskFileFormat
{
        nero_s32int order;/*保存命令种类*/  
        nero_s32int MsgCount;/*信息条数（分割符号分隔），包括命令*/  
        nero_8int   msgSeparator;/*信息分割符号，一般用空格*/
        nero_8int   orderSeparator;/*不同命令的分割符号，一般用换行*/
        nero_8int   str[500];
        void **     data;/*二维数组指针*/
        
}TFF;


void ReadTaskFromTxt(void);
void getMsgInToTFF(struct TaskFileFormat *  tff);
void JustDoTask();
void CreateBaseKindOfShu();
void ModifyBaseKindOfShu();




#endif
