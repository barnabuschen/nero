#ifndef NeroIO_H
#define NeroIO_H
#include "../common/type.h"
//#include "../tools/readUTF8File.h"
#include "../NeuralNetwork/NeuralNetwork.h"
struct  NeuronObjectMsg_
{ 
long MsgId; 
nero_8int fucId;
NeuronObject * Obi; 
} ;
struct  NeuronObjectMsgWithStr_
{ 
long MsgId; 
nero_8int fucId;
NeuronObject * Obi; 
nero_8int str[100];
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




nero_s32int Log_printNeroObjMsg(void *);

/*从一个数组对象中提前词组，存与str中*/
nero_s32int IO_getWordsInNero(nero_8int str[],NeuronObject * obj);

nero_s32int Log_printSomeMsgForObj(void * obj_,void *str_);

nero_s32int Log_printNeroObjLink(void * arg);

/*从一个字符对象中提取，存与str中*/
nero_s32int IO_getZhInNero(nero_8int str[],NeuronObject * obj);

nero_s32int Log_printAllKindOf(void * obj_,void *str_);

nero_s32int IO_GetNeroObjMsg(void * arg);












#endif
