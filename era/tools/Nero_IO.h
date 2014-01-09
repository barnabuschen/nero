#ifndef NeroIO_H
#define NeroIO_H
#include "../common/type.h"
//#include "../tools/readUTF8File.h"
#include "../NeuralNetwork/NeuralNetwork.h"
struct  NeuronObjectMsg_
{ 
long MsgId; 
char fucId;
NeuronObject * Obi; 
} ;




struct one_arg_message_entry 
{
    nero_us32int id; 
    nero_s32int (*operate)(void *); 
};
/*消息映射宏定义*/
#define BEGIN_ONE_ARG_MESSAGE_MAP(map_name) \
	struct one_arg_message_entry  (map_name)[]= {

#define MSG_NAME(msg_id, FucName) \
	{ msg_id, FucName},

#define END_ONE_ARG_MESSAGE_MAP \
	{0, 0} \
	};





void *thread_for_IO_Pic(void *arg);


void *thread_for_Log_Pic(void *arg);




nero_s32int Log_printNeroObjMsg(void *);
























#endif
