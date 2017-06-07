#ifndef NeroMsgIDFile_H
#define NeroMsgIDFile_H




#define IPCKEY 0x111



/**********************Task operateKind ID*************************/

/*几个有关NeroConfiguration的宏*/
#define  Conf_Modify_addLevelObjAlways    1   //修改addLevelObjAlways
#define  Conf_Modify_CreateNewBaseObjKind    2   //修改CreateNewBaseObjKind

#define  Conf_Modify_ReSet    100  //修改conf   为默认状态
#define  Forecast_Control_Set    120  //修改Forecast  Control  struct


/*****************************************************/

#define  Process_Create_ForOutputWord    3  //create a   NeuronNode_ForOutputWord  kind   obj







#define DeBugForMsgID

#ifdef DeBugForMsgID


/**********************Nero  ID*************************/
#define  MsgId_Nero_NONE    0
#define  MsgId_Nero_CreateNetNet    1000
#define  MsgId_Nero_addZhCharIntoNet    1001 /*将字符信息加入网络 */
#define  MsgId_Nero_DataFlowProcess    1002
#define  MsgId_Nero_ConfModify    1003
#define  MsgId_Nero_AddNewBaseKindByname   1004   /*生成新类时，如果所有子类都已经能够确认的情况下，可以直接输入类别名生成新的高级衍生类 */
#define  MsgId_Nero_SetDataFlowResult    1005

/*****************************************************/


/**********************log  ID*************************/
#define  MsgId_Log_PrintObjMsg    2000
#define  MsgId_Log_PrintObjMsgWithStr    2001
//~ #define  MsgId_Log_PrintNeroObjLinkTree    2002
/*****************************************************/


/**********************io  ID*************************/
#define  MsgId_IO_GetObjMsg    3000
#define  MsgId_IO_PrintObjMsgWithStr    3001
#define  MsgId_IO_dataIO    3002
#define  MsgId_IO_ForOutputWord    3003

/*****************************************************/




#else

#define  MsgId_Nero_NONE    1
#define  MsgId_Nero_CreateNetNet    1
#define  MsgId_Nero_addZhCharIntoNet    1 /*将字符信息加入网络 */
#define  MsgId_Nero_DataFlowProcess    1





#endif

#endif
