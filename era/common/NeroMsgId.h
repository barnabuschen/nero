#ifndef NeroMsgIDFile_H
#define NeroMsgIDFile_H







#define IPCKEY 0x111

#define DeBugForMsgID

#ifdef DeBugForMsgID


/**********************Nero  ID*************************/
#define  MsgId_Nero_NONE    0 
#define  MsgId_Nero_CreateNetNet    1000 
#define  MsgId_Nero_addZhCharIntoNet    1001 /*将字符信息加入网络 */ 
#define  MsgId_Nero_DataFlowProcess    1002 
/*****************************************************/


/**********************log  ID*************************/
#define  MsgId_Log_PrintObjMsg    2000 
#define  MsgId_Log_PrintObjMsgWithStr    2001 

/*****************************************************/


#else

#define  MsgId_Nero_NONE    1 
#define  MsgId_Nero_CreateNetNet    1 
#define  MsgId_Nero_addZhCharIntoNet    1 /*将字符信息加入网络 */ 
#define  MsgId_Nero_DataFlowProcess    1 



#endif



































































#endif
