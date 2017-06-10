#ifndef NeroIO_H
#define NeroIO_H
#include "../common/type.h"
//#include "../tools/readUTF8File.h"
#include "../NeuralNetwork/NeuralNetwork.h"


struct  NeuronObjectMsg_
{
long MsgId;
nero_8int fucId;
void    *  Obi;
} ;
struct  NeuronObjectMsgWithStr_
{
long MsgId;
nero_8int fucId;
NeuronObject * Obi;
nero_8int str[3000];
} ;
struct  IODataMsg_
{
long MsgId;
nero_8int fucId;
nero_s32int operateKind;
nero_8int str[300];
} ;



struct one_arg_message_entry
{
    nero_us32int id;
    nero_s32int (*operate)(void *);
};
struct two_arg_message_entry
{
    nero_us32int id;
    nero_s32int (*operate)(void *,void *);
};
/*消息映射宏定义*/
#define BEGIN_ONE_ARG_MESSAGE_MAP(map_name) \
	struct one_arg_message_entry  (map_name)[]= {

#define MSG_NAME(msg_id, FucName) \
	{ msg_id, FucName},

#define END_ONE_ARG_MESSAGE_MAP \
	{0, 0} \
	};



#define BEGIN_TWO_ARG_MESSAGE_MAP(map_name) \
	struct two_arg_message_entry  (map_name)[]= {

#define MSG_NAME(msg_id, FucName) \
	{ msg_id, FucName},

#define END_TWO_ARG_MESSAGE_MAP \
	{0, 0} \
	};



void *thread_for_IO_Pic(void *arg);


void *thread_for_Log_Pic(void *arg);
void doTest_();
void IO_SaveSysIntoDatabase(NeuronObject  *godNero);
//~ 打印某对象的子对象树
nero_s32int Log_printNeroObjLinkTree(void * arg);

nero_s32int Log_printNeroObjMsg(void *);
nero_s32int Log_printAllNeroMsg(void * arg);
/*从一个数组对象中提前词组，存与str中*/
nero_s32int IO_getWordsInNero(nero_8int str[],NeuronObject * obj);
//向文件中打印对象的信息
nero_s32int Log_printSomeMsgForObj(void * obj_,void *str_);
//打印对象的链接信息
nero_s32int Log_printNeroObjLink(void * arg);

/*从一个字符对象中提取，存与str中*/
nero_s32int IO_getZhInNero(nero_8int str[],NeuronObject * obj);
/*打印某个类别下面的所有的衍生类*/
nero_s32int Log_printAllKindOf(void * obj_,void *str_);

nero_s32int IO_GetNeroObjMsg(void * arg);

nero_s32int IO_InputDataToSys(void * operateKind,void *str);
nero_s32int IO_NeroConfigurationModify(void * operateKind,void *dataFilePath);
nero_s32int IO_ForOutputWord(void * arg);
int UTF8ToGBK(char src[],char res[]);
int  GetToken( char * str);

nero_s32int Log_printFormattedMsg(void * obj_,void *str_);





#endif
