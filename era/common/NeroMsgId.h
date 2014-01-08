#ifndef NeroMsgIDFile_H
#define NeroMsgIDFile_H


#define DeBugForMsgID

#ifdef DeBugForMsgID

#define  MsgId_Nero_NONE    0 
#define  MsgId_Nero_CreateNetNet    1000 
#define  MsgId_Nero_addZhCharIntoNet    1001 /*将字符信息加入网络 */ 
#define  MsgId_Nero_DataFlowProcess    1002 

#else

#define  MsgId_Nero_NONE    1 
#define  MsgId_Nero_CreateNetNet    1 
#define  MsgId_Nero_addZhCharIntoNet    1 /*将字符信息加入网络 */ 
#define  MsgId_Nero_DataFlowProcess    1 



#endif



































































#endif
