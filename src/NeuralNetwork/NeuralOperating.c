
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "NeuralNetwork.h"
#include "NeuralOperating.h"
#include "../tools/readUTF8File.h"
#include "../tools/Nero_IO.h"
/*#include "../common/error.h"*/
static struct  NeuronObjectMsg_    neroObjMsg_st;
static struct  NeuronObjectMsgWithStr_    neroObjMsgWithStr_st;
// #define Process_TemporaryNUM   7500    //just used  in  fuc  Process_StrengthenLink
nero_us32int     tmpObiUsed;						//record    how many objs  store  in
static NeuronObject  * Process_tmpObi[Process_TemporaryNUM];
static struct NeroObjForecastList   Process_forecastListNode[Process_TemporaryNUM];
 nero_8int   Process_tmpStr[500];

struct DataFlowForecastInfo  forecastInfo_st;
static struct DataFlowResultRecordInfo  dataFlowRstRcdInfo;
volatile sig_atomic_t atomForDataFlowProcessInput=0;
/*************Operating函数族/**************/
BEGIN_ONE_ARG_MESSAGE_MAP(Operating_msg_OneArg_map)
/*    MSG_NAME(1, IO_GetNeroObjMsg)*/
/*    MSG_NAME(2, Log_printNeroObjLink)*/
/*    MSG_NAME(3, Log_GetNeroObjMsg)*/
/*    MSG_NAME(4, sort_data)*/
END_ONE_ARG_MESSAGE_MAP


BEGIN_TWO_ARG_MESSAGE_MAP(Operating_msg_TwoArg_map)
    MSG_NAME(1, Operating_NeroConfigurationModify)
/*    MSG_NAME(2, IO_NeroConfigurationModify)*/
/*    MSG_NAME(3, del_data)*/
/*    MSG_NAME(4, sort_data)*/
END_TWO_ARG_MESSAGE_MAP

nero_s32int Operating_NeroConfigurationModify(void * operateKind,void *c)
{
        NeroConf * tmpConf=c;
        nero_s32int kind=*((nero_s32int *)operateKind);
        struct NeroObjForecastControl  * tmpcrtl;
/*        printf("i=%d.\n",tmpConf->addLevelObjAlways);*/
        switch(kind)
        {

         case Conf_Modify_addLevelObjAlways:
                neroConf.addLevelObjAlways=tmpConf->addLevelObjAlways;
                 // printConfChangMsg(Conf_Modify_addLevelObjAlways);

                break;
         case Conf_Modify_CreateNewBaseObjKind:
                neroConf.CreateNewBaseObjKind=tmpConf->CreateNewBaseObjKind;
                   // printConfChangMsg(Conf_Modify_CreateNewBaseObjKind);

                break;
           case Conf_Modify_ReSet:
					resetNeroConf();
                   // printConfChangMsg(Conf_Modify_ReSet);

                break;
           case Forecast_Control_Set:
           		tmpcrtl=(struct NeroObjForecastControl  *) c;
					// resetNeroConf();
           		forecastInfo_st.controlMsg.expectedKind		=tmpcrtl->expectedKind;
           		forecastInfo_st.controlMsg.baseORDerivative=tmpcrtl->baseORDerivative;
           		forecastInfo_st.controlMsg.Refreshed		=tmpcrtl->Refreshed;
           		forecastInfo_st.controlMsg.DurationTime		=tmpcrtl->DurationTime;
           		forecastInfo_st.controlMsg.metaData		=tmpcrtl->metaData;

        		// printf("Forecast_Control_Set..............(%d %d %d %d).............\n",forecastInfo_st.controlMsg.expectedKind,forecastInfo_st.controlMsg.baseORDerivative,forecastInfo_st.controlMsg.Refreshed,forecastInfo_st.controlMsg.DurationTime);

                break;

        default:break;
        }

}
void * thread_for_Operating_Pic(void *arg)
{
	int x=0,countRunTimes=0,i;
	int timeToWaitCandy=0;
	char *fileName=NULL;
	long MsgId;
	struct { long MsgId; char text[100]; } OperatingMsg;
	struct ZhCharArg * arg1;
	struct DataFlowProcessArg * arg2;
	struct  IODataMsg_ * DataIO_st;
	struct  DataFlowDataMsg_twoNum  * dataFlowDataMsg_twoNum_;
	key_t ipckey;
	int Operating_mq_id;

	int received,hasSetUpNeroSys,hasAddZhChar;


/*	const nero_s32int size_message_map = */
/*	      sizeof(IO_msg_print_map) / sizeof(struct one_arg_message_entry );   //求得表长*/
	const nero_s32int size_message_map2 =
	      sizeof(Operating_msg_TwoArg_map) / sizeof(struct two_arg_message_entry );   //求得表长
	/* Generate the ipc key */
/*	Operating_ipckey="/home/ub/shareSpace/Operating_ipckey";*/


/*	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
	#define IPCKEY 0x111
	ipckey = ftok(Operating_ipckey, IPCKEY);
	// printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
	// printf("Operating_ipckey key is %d\n", ipckey);
/*	*/
	/* Set up the message queue */
	Operating_mq_id = msgget(ipckey,0);// IPC_CREAT
	printf("thread_for_Operating_Pic:Operating_mq_id =%d\n", Operating_mq_id);
	// printf("Operating_mq_id :strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
/*	msgctl(Operating_mq_id,IPC_RMID,0);*/
/*	printf("Operating_mq_id :    清空队列: %s\n", strerror(errno));*/
/*	Operating_mq_id = msgget(ipckey,0);// IPC_CREAT*/
/*	printf("Operating_ipckey Message identifier is %d\n", Operating_mq_id);*/
	hasSetUpNeroSys=0;
	hasAddZhChar=0;
/*	while( msgrcv(Operating_mq_id, &OperatingMsg, sizeof(OperatingMsg), 0, MSG_NOERROR) >1 );*/
	while(x == 0)
	{
/*		sleep(1);*/
		// printf("wait for Operating msg......\n");
		received = msgrcv(Operating_mq_id, &OperatingMsg, sizeof(OperatingMsg), 0, MSG_NOERROR);
				MsgId=OperatingMsg.MsgId;

		if (errno != 0)
			printf("Operating Operating strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
		if (received<1)
		{
			#ifdef Nero_DeBugInOperating_Pic
			 printf("received fail\n");
			#endif
			continue;
		}
		else
			#ifdef Nero_DeBugInOperating_Pic_
			 printf("received  ok:MsgId=%d\n",MsgId);
			#endif

		switch(MsgId)
		{

		case MsgId_Nero_SetDataFlowResult:
			dataFlowDataMsg_twoNum_ =  (struct DataFlowDataMsg_twoNum *)OperatingMsg.text;

			if(dataFlowRstRcdInfo.actualNums   <   DataFlow_MaxResultNUm)
			{
			// 	struct DataFlowResultRecordInfo
			// {
			// 	nero_us32int expectKinds [DataFlow_MaxResultNUm];//期望对象的kind,该值往往需在sys内部根据string  get
			// 	nero_us32int BaseOrObj   [DataFlow_MaxResultNUm];//期望对象is  base  or obj? == 1  means  is baseObj
			// 	nero_us32int actualNums;
			// };			
				dataFlowRstRcdInfo.expectKinds[dataFlowRstRcdInfo.actualNums] =  dataFlowDataMsg_twoNum_->kind;
				dataFlowRstRcdInfo.BaseOrObj[dataFlowRstRcdInfo.actualNums] =  dataFlowDataMsg_twoNum_->baseORDerivative;
				dataFlowRstRcdInfo.actualNums++;
			}
			// printf("MsgId_Nero_SetDataFlowResult1  kind=%d,base=%d\n",dataFlowDataMsg_twoNum_->kind,dataFlowDataMsg_twoNum_->baseORDerivative);
			break;
		case MsgId_Nero_AddNewBaseKindByname:
			// 生成新类时，如果所有子类都已经能够确认的情况下，可以直接输入类别名生成新的高级衍生类
			arg2=(struct DataFlowProcessArg *)OperatingMsg.text;
			countRunTimes++;
			/*判断系统到底初始化没有*/
			if (hasSetUpNeroSys == 1)
			{
			 	// printf("MsgId_Nero_AddNewBaseKindByname{%d}:\n",countRunTimes);
				//  printf("MsgId_Nero_DataFlowProcess1  arg2->dataNum={%d}:\n",arg2->dataNum);
				// DataFlowProcess(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf)

				// DataFlowProcess(arg2->DataFlow,arg2->dataKind,arg2->dataNum,  GodNero, arg2->conf);
				Process_AddNewBaseKindByname(arg2->DataFlow,arg2->dataKind,arg2->dataNum,  GodNero, arg2->conf);

				// #ifdef Nero_DeBuging09_01_14
				// #endif
					 // printf("MsgId_Nero_DataFlowProcess1  arg2->dataNum={%d}:\n",arg2->dataNum);

				/*show  neroNet*/
				#ifdef  Nero_DeBuging03_12_13_
				 createNeroNetDotGraphForWords(GodNero, "data/wordspic.dot");
				printf("createNeroNetDotGraph   done.\n");
				#endif

				#ifdef  Nero_DeBuging03_12_13_
				system("xdot data/wordspic.dot");
				#endif
				#ifdef Nero_DeBuging10_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				nero_s32int xxxxxx=NeuronNode_ForChCharacter;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));

				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif
			}
			else
			{
					printf("MsgId_Nero_DataFlowProcess:系统未初始化\n");
					printf("Operating_ipckey key is %d\n", ipckey);
			}
			break;
		case MsgId_Nero_ConfModify:

	 			DataIO_st=(struct    IODataMsg_   * )&OperatingMsg;

				for( i = 0; i < size_message_map2; i++)
				{
				    if( Operating_msg_TwoArg_map[i].id == DataIO_st->fucId )
					 (*(Operating_msg_TwoArg_map[i].operate) )(&(DataIO_st->operateKind),DataIO_st->str);
				}
				break;

		case MsgId_Nero_CreateNetNet:
			CreateActNeroNet();
			hasSetUpNeroSys=1;
			#ifdef Nero_DeBuging09_01_14
			 printf("MsgId_Nero_CreateNetNet: done\n");
			#endif
			break;
		case MsgId_Nero_addZhCharIntoNet:
			if (hasSetUpNeroSys == 1)
			{
			arg1=(struct ZhCharArg *)OperatingMsg.text;
			nero_addZhCharIntoNet( GodNero,arg1->chChar, arg1->charCounts);
			#ifdef Nero_DeBuging09_01_14
			 printf("MsgId_Nero_addZhCharIntoNet:添加字符数：%d个\n",arg1->charCounts);
			#endif
			hasAddZhChar=1;
			}
			else{
				printf("MsgId_Nero_addZhCharIntoNet:系统未初始化\n");
					printf("Operating_ipckey key is %d\n", ipckey);

			}

			break;
		case MsgId_Nero_DataFlowProcess :
			arg2=(struct DataFlowProcessArg *)OperatingMsg.text;
			countRunTimes++;


			// while(hasSetUpNeroSys != 1)
			// {

			// 	// sleep(1);
				// printf("MsgId_Nero_DataFlowProcess:    hasSetUpNeroSys=%d     \n",hasSetUpNeroSys);

			// }


			/*判断系统到底初始化没有*/
			if (hasSetUpNeroSys == 1)
			{

				 // printf("MsgId_Nero_DataFlowProcess1{%d}:\n",countRunTimes);
					//  printf("MsgId_Nero_DataFlowProcess1  arg2->dataNum={%d}:\n",arg2->dataNum);

				DataFlowProcess(arg2->DataFlow,arg2->dataKind,arg2->dataNum,  GodNero, arg2->conf);
				// #ifdef Nero_DeBuging09_01_14
				// #endif
					 // printf("MsgId_Nero_DataFlowProcess1  arg2->dataNum={%d}:\n",arg2->dataNum);

				/*show  neroNet*/
				#ifdef  Nero_DeBuging03_12_13_
				 createNeroNetDotGraphForWords(GodNero, "data/wordspic.dot");
				printf("createNeroNetDotGraph   done.\n");
				#endif

				#ifdef  Nero_DeBuging03_12_13_
				system("xdot data/wordspic.dot");
				#endif


				#ifdef Nero_DeBuging10_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				nero_s32int xxxxxx=NeuronNode_ForChCharacter;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));


				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif
			}
			else
			{


					printf("MsgId_Nero_DataFlowProcess:系统未初始化\n");
					printf("Operating_ipckey key is %d\n", ipckey);

			}

			break;


		default:
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_NONE:  \n");
			 printf("Operating msg=%s (%d)\n", OperatingMsg.text, received);
			#endif
			break;
		}







	}
	printf("end to wait Operating msg................................\n");
}

/*
利用传入的kind新建一个基类
参数中dataKind是无用的，都是NeuronNode_ForChWord

DataFlow中就是dataNum个数的str，除了第一个是需要新建类型的名字外其他，是基类子数据的类型的名字
如果是下面的sys原来的基类的话就制定一个str来代替，比如干脆直接用61.62来代替

#define NeuronNode_ForConnect   10    //当一个概念节点的类型为此时表示一个链接节点
#define NeuronNode_ForImage   50    //当一个概念节点的类型为此时表示一个图像对象
#define NeuronNode_ForLine   51    //当一个概念节点的类型为此时表示一个线条对象


#define NeuronNode_ForChCharacter   61    //当一个概念节点的类型为此时表示一个汉字，包括中文标点符号
#define NeuronNode_ForChWord   62    //当一个概念节点的类型为此时表示一个中文词语
#define NeuronNode_ForChSentence   63    //当一个概念节点的类型为此时表示一个中文句子



*/
nero_s32int  Process_AddNewBaseKindByname(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf)
{
	nero_s32int i,j,hasAddObj,res1,res2;
	NeuronObject * tmpObi,* tmpBaseObi,* tmpBaseObi2;
	NeuronObject ** objs=NULL;
	NeuronObject * complexObj;
	NerveFiber *curFiber;
	/*参数检查*/
	if (DataFlow == NULL  || dataKind ==NULL  ||  dataNum <2  ||  dataKind[0]  != NeuronNode_ForChWord  ||  dataNum > Process_TemporaryNUM )
	{


		printf("\n\n Process_AddNewBaseKindByname1:nero_msg_ParameterError\n\n");
		return nero_msg_ParameterError;
	}

// nero_us32int     tmpObiUsed;						//record    how many objs  store  in
// static NeuronObject  * Process_tmpObi[Process_TemporaryNUM];
// static struct NeroObjForecastList   Process_forecastListNode[Process_TemporaryNUM];
	(objs)=(NeuronObject **)malloc(sizeof(NeuronObject *)*dataNum);

	// complexObj=NULL;
	// tmoForRecordNUm=0;
	// ifHasUnknowObj=0;
	printf("\n\n Process_AddNewBaseKindByname0:its here\n\n");
	// find all kinds for DataFlow
	i=0;
	objs[i]=nero_IfHasNeuronObject(DataFlow[i],dataKind[i], GodNero);
	if(objs[i] ==  NULL)
	{

		printf("\n\n Process_AddNewBaseKindByname2:nero_msg_ParameterError\n\n");
		return nero_msg_ParameterError;

	}
	for(i=1;i< dataNum;i++)
	{
		dataKind[i]=nero_getObjKindByName(DataFlow[i],  GodNero);

		objs[i] = nero_getBaseObjByKind(dataKind[i],GodNero);

	}

    {
		/*判断是否可以进行新基类（抽象概念）创建*/
		res2=Process_IfCreateNewBaseObj(objs,dataNum,GodNero,conf);
		/*开始添加基类 */
		if (res2 == NeroYES)//do not create new kind
		{
			#ifdef Nero_DeBuging09_01_14_
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
			neroObjMsgWithStr_st.Obi = NULL;
			sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:start create new basekind=%d !",conf->NewNeroClassID);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
			#endif

			printf("need  to create new kind\n");
			tmpBaseObi=nero_CreateNewBaseObj(objs,dataNum,GodNero, conf);
			// printf("NewBaseObj:%x\n",tmpBaseObi);
			tmpBaseObi2=nero_CreateNewBaseObjInSAP(objs,dataNum,SAGodNero, conf);

			#ifdef Nero_DeBuging06_02_14_
			if(tmpBaseObi)
			{
				// for (i=0;i<objNum;i++)
				{
				// printf("new BaseKind =%d\n",nero_GetNeroKind(tmpBaseObi));
				}
				// printf("所有child对象kind为:\n");
				curFiber=tmpBaseObi->inputListHead;
				if(curFiber == NULL)
				{
				// printf("  inputListHead =NULL  \n");
				}
				// while(curFiber)
				// {
				// 	tmpObiForTest=curFiber->obj;
				// 	printf("          kind:%d\n",nero_GetNeroKind(tmpObiForTest));
				// 	curFiber=curFiber->next;
				// }

				#ifdef Nero_DeBuging09_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
				neroObjMsgWithStr_st.Obi = NULL;
				sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:nero_CreateNewBaseObj success,new basekind=%d !",nero_GetNeroKind(tmpBaseObi));
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif

			}
			else
			{
				#ifdef Nero_DeBuging09_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
				neroObjMsgWithStr_st.Obi = NULL;
				sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:nero_CreateNewBaseObj fail !");
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif
				printf("nero_CreateNewBaseObj fail \n\n\n");
			}
			#endif
		}
		else
		{
			#ifdef Nero_DeBuging09_01_14_
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
			neroObjMsgWithStr_st.Obi = NULL;
			sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:not create newbase,but see if need to Modify it,nextBaseKind=%d",conf->NewNeroClassID);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
			#endif
			/*如果不需要添加新的基类，那就是修改基类了*/
			printf("do not create new base  kind,but  see  if need to  Modify it\n");
			// if(res2 !=   nero_msg_unknowError )
			// {
			// 	// printf("Modify base kind\n");
			// 	newBaseObjKind_= res2;
			// 	nero_ModifyBaseKind(objs,objNum,GodNero,conf,res2);
			// }
		}

    }
    atomForDataFlowProcessInput  =0;
	// you should  init   neroConf  every time  you  run  this  fuc
	resetNeroConf();
	// forecastInfo_st.controlMsg.Refreshed=0;
	// for (j=0;j<dataNum;j++)
	// {
	// 	if (DataFlow[j])
	// 	{
	// 		free(DataFlow[j]);
	// 	}
	// }
	// if (dataKind)
	// {
	// 	free(dataKind);
	// }

	// if (DataFlow)
	// {
	// 	free(DataFlow);
	// }
	if (objs)
	{
		free(objs);
	}
	return nero_msg_ok;
}


/*
DataFlow  数据的指针数组，该数据的解析方式需要依据dataKind来指定,一些特殊的数据，其长度该数据本身指定
dataKind   数据的类型,指出每个数据的类型
		dataKind=NeuronNode_ForChCharacter  则DataFlow就是一个汉字的utf8码
		dataKind=NeuronNode_ForChWord  则DataFlow就是一个中文词组的utf8码，不能有符号,必须保证最后一个字节为0
		dataKind=NeuronNode_ForChSentence 则DataFlow就是一个中文句子的utf8码

2013年12月18日newStart

dataNum	   数据的指针数组数据的个数，就是数组的长度

0：首先默认所以数据流的类型是一样的
1:dataNum必须>=1
	dataNum=1   直接判断该数据是否在系统中已经存在该数据，不存在则默认添加一个新概念
	dataNum=1

2:这个函数貌似只有输入数据，那么输出数据呢，因为暂时你先不管这个，所以这里先不输出数据
3:


*/


nero_s32int DataFlowProcess(void *DataFlow_[],nero_s32int dataKind_[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf)
{
	nero_s32int i,j,hasAddObj/*,hasNewObj*/,res1,objNum,OldobjNum,res2,tmoForRecordNUm,ifHasUnknowObj,newBaseObjKind_,justForTest11;
	NeuronObject * tmpObi,* tmpBaseObi,* tmpBaseObi2,* tmpObiForTest;
	nero_s32int ifHasUndefinedKind;
	static 	NeuronObject ** objs=NULL;
	NeuronObject * TmpStagingAreaNero=NULL;
	NeuronObject * complexObj;
/*        struct NeroObjForecastList   *findObiPoint; */
/*        NeuronObject * findForecastObj;	*/
/*        struct NeroObjForecastList   *findForecastObjPoint; 	*/
	NerveFiber *curFiber;
	static nero_us32int coutOferror_Msg_=0;//  it recond  how many  times  the  DataFlowProcess  be  called
	static void *DataFlow[DataFlowPoolListNum];
	static nero_s32int dataKind[DataFlowPoolListNum]={0};
	/*参数检查*/
	// pthread_mutex_lock(&mutexForDataFlowProcessInput);
	// printf("DataFlowProcess  lock\n");
	if (DataFlow_ == NULL  || dataKind_ ==NULL  ||  dataNum <1  || dataNum > DataFlowPoolListNum)
	{
		printf("\n\nDataFlowProcess:nero_msg_ParameterError1\n\n");
		return nero_msg_ParameterError;
	}
/*	system("xdot data/wordspic.dot");*/

	if(coutOferror_Msg_ == 0)
	{
		(objs)=(NeuronObject **)malloc(sizeof(NeuronObject *)* DataFlowPoolListNum);
        for(i=0,j=0;i < DataFlowPoolListNum ;i++)
        {

                DataFlow[i]= (void *)malloc((sizeof( char))*DataFlowPoolStrMaxLen);  //DataFlowPoolStrMaxLen
                // printf("malloc\n");

        }
	}
	if(objs == NULL)
	{
		printf("\nDataFlowProcess:nero_msg_ParameterError2\n");
		return nero_msg_ParameterError;
	}
	for(i=0;i<dataNum;i++)
	{
		// strcpy(DataFlow[i],DataFlow_[i]);
		// snprintf(DataFlow[i],DataFlowPoolListNum,"",DataFlow_[i]);

		switch(dataKind_[i])
		{
			case NeuronNode_ForChCharacter:
				memmove(DataFlow[i],DataFlow_[i],4);
				break;
			case NeuronNode_ForChWord:
				strcpy(DataFlow[i],DataFlow_[i]);
				break;
			default:
				memmove(DataFlow[i],DataFlow_[i],DataFlowPoolListNum);
				break;
		}
		dataKind[i]=dataKind_[i];
	}
	// pthread_mutex_unlock(&mutexForDataFlowProcessInput);
	// printf("DataFlowProcess  unlock\n");
	 atomForDataFlowProcessInput  =0;
	 // static   nero_s32int  countForadfaejali=0;
	 ifHasUndefinedKind=0;
	 //check if has  Undefined kind[有不知道类别的数据]
	 // 暂时这么设定，一旦有Undefined kind就绕过所有操作，只去判断这个数据流最可能指向什么哪个上层obj
	for (i=0  ;i<dataNum;i++)
	{
		if( dataKind[i]  ==  NeuronNode_ForUndefined)
		{
			ifHasUndefinedKind=1;
			// printf("DataFlowProcess0: NeuronNode_ForUndefined[%d]\n",i);
			break;
		}
	}
	coutOferror_Msg_=coutOferror_Msg_+1;
	complexObj=NULL;
	tmoForRecordNUm=0;
	ifHasUnknowObj=0;

	#ifdef DataFlowProcess_error_Msg
	printf("coutOferror_Msg_   11:%d.\n",coutOferror_Msg_);
	#endif


	/*先不比对DataFlow  dataKind  dataNum*/
	/*断DataFlow中的数据是否在系统中已经存在该数据*/
	for (i=0,j=0,hasAddObj=0;i<dataNum  &&  ifHasUndefinedKind != 1;i++)
	{


		#ifdef DataFlowProcess_error_Msg
		printf("coutOferror_Msg_   111:%d.\n",coutOferror_Msg_);
		#endif



		#ifdef   Nero_DeBuging04_01_14
		// char str[500];
		// char str2[500];
/*				PrintUtf8 ttt;*/
/*				printf("\n");		*/
/*				ttt.tmp=*((ChUTF8 *)DataFlow);*/
/*				ttt.end=0;	*/
		#ifdef DataFlowProcess_error_Msg
		if( i == (dataNum -1 ))
			printf("%s[%d]END\n",(nero_s8int *)DataFlow[i],dataKind[i]);
		else
			printf("%s[%d] ",(nero_s8int *)DataFlow[i],dataKind[i]);
		#endif
		// sprintf(str,"data/wordspic%d.dot",i);
		// sprintf(str2,"xdot data/wordspic%d.dot",i);
		// createNeroNetDotGraphForWords(GodNero, str);
		// system(str2);

		#endif

		#ifdef DataFlowProcess_error_Msg
		printf("coutOferror_Msg_   1121:%d.\n",coutOferror_Msg_);
		#endif
		// printf("processed  data:kind=%d ,%s\n",dataKind[i],DataFlow[i]);
		/*先不管有句子的情况*/
		/*通过objs[j]里面的值就可以知道有没有在网络中找到这个对象*/
		tmpObi =nero_IfHasNeuronObject(DataFlow[i],dataKind[i], GodNero);
		#ifdef DataFlowProcess_error_Msg
		printf("coutOferror_Msg_   112:%d.\n",coutOferror_Msg_);
		#endif

		//这里有个问题，就是tmpObi ！= NULL 那么，是否需要加强相应的FiberStrengthen
		if (tmpObi == NULL  )
		{
				// ifHasUnknowObj=1;  msg1:在DataFlowProcess中找不到该概念,kind=%d,i=%d  str=%s
										// msg1:在DataFlowProcess中找不到该概念,kind=61,i=2
				#ifdef Nero_DeBuging09_01_14_

					#ifdef Nero_DeBuging09_01_14
					if(conf->CreateNewBaseObjKind == 1   )
					{
						printf("CreateNewBaseObjKind = 1 :找不到子概念  i=%d,str=%s,kind=%d \n",i,DataFlow[i],dataKind[i]);
					}
					#endif
				// printf("找不到子概念  i=%d\n",i);
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = tmpObi;
				if(dataKind[i] == 62)
					sprintf(neroObjMsgWithStr_st.str,"msg11:在DataFlowProcess中找不到该概念,kind=%d,i=%d  str=%s",dataKind[i],i,DataFlow[i]);
				else if(dataKind[i] == 2012)
				{
					sprintf(neroObjMsgWithStr_st.str,"msg12:在DataFlowProcess中找不到该概念,kind=%d,i=%d  str=%s",dataKind[i],i,DataFlow[i]);
				}
				else
					sprintf(neroObjMsgWithStr_st.str,"msg13:在DataFlowProcess中找不到该概念,kind=%d,i=%d,dataNum=%d  ",dataKind[i],i,dataNum);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif



		}
		else
		{
				// ifHasUnknowObj=0;	msg1:在DataFlowProcess中找不到该概念,kind=2012,i=4
				#ifdef Nero_DeBuging09_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
				neroObjMsgWithStr_st.Obi = tmpObi;
				if(dataKind[i] == 62)
					sprintf(neroObjMsgWithStr_st.str,"msg14:在DataFlowProcess中find概念,kind=%d,i=%d  str=%s",dataKind[i],i,DataFlow[i]);
				else
					sprintf(neroObjMsgWithStr_st.str,"msg15:在DataFlowProcess中fidn概念,kind=%d,i=%d,dataNum=%d",dataKind[i],i,dataNum);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif
				tmoForRecordNUm++;
				// printf(" already has  this obj  in sys :i=%d.j=%d,tmoForRecordNUm=%d\n",i,j,tmoForRecordNUm);
			// exit(0);
			// printf("找到子概念\n");
		}



		/*如果不存在则尝试将该对象加入网络*/
		if (tmpObi == NULL  && conf->addNewObj == 1)
		{


			// do  not add nero for  every  kind  of data
			// there is  bug,where
			 // 你不需要为所有类型都生成obj,除了NeuronNode_ForChCharacter这样得内定类型外
			// 对于其他kind》NeuronNode_ForComplexDerivative得类型来说
			// 要想生成永久对象，首先必须通过Process_StrengthenLink测试
			// 而这里可以先生成临时对象
			switch(dataKind[i])
			{
				case NeuronNode_ForChCharacter:
				case NeuronNode_ForChWord:
				case NeuronNode_ForLayering:
						tmpObi=  nero_addNeroByData(DataFlow[i],dataKind[i],GodNero);
						break;
				default:
						// 比如对于生成一个 整数 14 类型得obj来说，首先，14作为一个字符串
						// sys并没有这么一个obj
						// if  CreateNewBaseObjKind == 1  生成永久对象，否则生成临时对象
						// 但是还是有问题，就是生成得不能是太复杂得东西，if 把一个
						// 东西得子东西得结构看成一棵树，树得深度不能超过一层
						if (conf->CreateNewBaseObjKind == 1  ||   conf->addLevelObjAlways==1)
						{
							//暂时只处理这个
							// printf("在DataFlowProcess:nero_addNeroByData\n");
							tmpObi=  nero_addNeroByData(DataFlow[i],dataKind[i],GodNero);
						}
						else
						{

						}

						break;

			}

			#ifdef Nero_DeBuging21_12_13
			if (tmpObi != NULL  )
			{

/*				nero_printNeroLink("log/ObjLink.log",(void *)tmpObi);*/
/*				*/
/*				 neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;*/
/*				 neroObjMsg_st.fucId = 1;*/
/*				 neroObjMsg_st.Obi = tmpObi;*/
/*				 msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);*/
/*				*/
				// printf("添加子概念,dataKind=%d,adress:%x,  upp obj=%x\n",dataKind[i],tmpObi,tmpObi->outputListHead->obj);

				#ifdef Nero_DeBuging09_01_14_
				if(conf->CreateNewBaseObjKind == 1   )
				{
					printf("CreateNewBaseObjKind = 1 :new obj create success  i=%d,str=%s,kind=%d ,new kind =%d \n",i,DataFlow[i],dataKind[i],nero_GetNeroKind(tmpObi));
				}
				#endif


				#ifdef Nero_DeBuging09_01_14_
				// printf("添加子概念成功\n\n");
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = tmpObi;
				// if(nero_GetNeroKind(tmpObi) == 62)
				// 	sprintf(neroObjMsgWithStr_st.str,"msg2:在DataFlowProcess中创建对象成功: (%s),kind=%d ,%x\n",DataFlow[i],nero_GetNeroKind(tmpObi),tmpObi);
				// else
				sprintf(neroObjMsgWithStr_st.str,"DataFlowProcess:msg2:在DataFlowProcess中创建对象成功: kind=%d ,%x",nero_GetNeroKind(tmpObi),tmpObi);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif

		        #ifdef Nero_DeBuging09_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = NULL;
				sprintf(neroObjMsgWithStr_st.str,"				CreateNewBaseObjKind=%d,addLevelObjAlways=%d",conf->CreateNewBaseObjKind,conf->addLevelObjAlways);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif

				#ifdef Nero_DeBuging09_01_14_
				neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
				neroObjMsg_st.fucId = 2;
				neroObjMsg_st.Obi = tmpObi;
				msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);
				#endif
			}
			else
			{
					// printf("添加子概念失败,dataKind=%d,i=%d\n",dataKind[i],i);
					ifHasUnknowObj=1;
			        #ifdef Nero_DeBuging09_01_14_
			        // printf("++++++++++++++DataFlow[i]=%s.\n",DataFlow[i]);
					// printf("添加子概念失败,dataKind=%d,i=%d\n",dataKind[i],i);
					//~ printf("添加子概念成功\n\n");
					neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
					neroObjMsgWithStr_st.fucId = 1;
					neroObjMsgWithStr_st.Obi = NULL;
					// sprintf(neroObjMsgWithStr_st.str,"DataFlowProcess:msg2:在DataFlowProcess中创建对象 fail :%s,dataKind[i]=%d,addLevelObjAlways=%d",DataFlow[i],dataKind[i],conf->addLevelObjAlways);
					sprintf(neroObjMsgWithStr_st.str,"DataFlowProcess:msg2:在DataFlowProcess中创建对象 fail :%s,dataKind[i]=%d",DataFlow[i],dataKind[i]);
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
					#endif
			        #ifdef Nero_DeBuging09_01_14_

					neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
					neroObjMsgWithStr_st.fucId = 1;
					neroObjMsgWithStr_st.Obi = NULL;
					sprintf(neroObjMsgWithStr_st.str,"				CreateNewBaseObjKind=%d,addLevelObjAlways=%d",conf->CreateNewBaseObjKind,conf->addLevelObjAlways);
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
					#endif
			}
			#endif
/*			createNeroNetDotGraphForWords(GodNero, "data/wordspic.dot");		*/
/*			system("xdot data/wordspic.dot");*/
			if (tmpObi != NULL  && conf->addNewObj ==1)
			{
				hasAddObj=1;/*只要添加过新概念就设置为1*/
				objs[j]=tmpObi;
				j++;
			}

		}
		else if (tmpObi != NULL )
		{
			objs[j]=tmpObi;
			// printf(" already has  this obj  in sys :objs[j]=%x.tmpObi=%x\n",objs[j],tmpObi);
			// printf(" already has  this obj  in sys :i=%d.j=%d\n",i,j);
			j++;
		}




	}
     /*对象预测-------------------------*/
	/*初始化*/
	 nero_s32int tmpc=0;
	#ifdef DataFlowProcess_error_Msg_
	if (coutOferror_Msg_ <=  Nero_TestCount )
	{
	printf("id=%d  --------------------------------\n",coutOferror_Msg_);
	printf("识别对象数=%d.  UpperLayer=%d,SameLayer=%d\n",j,forecastInfo_st.headOfUpperLayer.times,forecastInfo_st.headOfSameLayer.times);
	}
	if (coutOferror_Msg_ <=  Nero_TestCount )
	{
		for (;tmpc<j;tmpc++)
		{
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi = objs[tmpc];
			sprintf(neroObjMsgWithStr_st.str,"打印识别对象obj(%d) =%x id=%d",tmpc,objs[tmpc],coutOferror_Msg_);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		}
		// printf("\n");
	}

    #endif
	forecastInfo_st.objs=objs;
	if(ifHasUndefinedKind ==1)
		forecastInfo_st.objNum=0;
	else
		forecastInfo_st.objNum=j;
	forecastInfo_st.objPoint=0;
	forecastInfo_st.DeBugMsg=coutOferror_Msg_;
/*	forecastInfo_st.head=NULL;//在thread_for_Sys_Pic(void *arg)中被初始化*/


	#ifdef DataFlowProcess_error_Msg1_

	//print objs  array
	printf("before Process_ObjForecast:\n");
	for(tmpc=0;tmpc < j;tmpc++)
	{

		printf("%x{%d}  ", forecastInfo_st.objs[tmpc] ,nero_GetNeroKind(forecastInfo_st.objs[tmpc]) );

	}
	// printf(" \n" );
	#endif

	/***************************************************************/
	// CreateNewBaseObjKind=1将影响新类的创建，影响类型判断,所以这里需要要这个条件
	// #define Nero_TestCount     30
	// printf("before Process_SearchObjForStr  \n");
	if (  /*Nero_TestCount >= coutOferror_Msg_  && */ conf->CreateNewBaseObjKind != 1   &&  conf->addLevelObjAlways != 1  && forecastInfo_st.controlMsg.expectedKind >=  NeuronNode_ForUndefined)
	{
		//首先将所有i UndefinedKind的DataFlow找到对应的obj
		//现在的情况是只知道数据：DataFlow[i]  不知道对应的dataKind[i],
		// 还有这些data，是由什么元数据组成的,such like :NeuronNode_ForChCharacter
		// 为了加快进度，这里假设元数据就是NeuronNode_ForChCharacter
		// printf("before Process_SearchObjForStr ifHasUndefinedKind=%d \n",ifHasUndefinedKind);
		if(ifHasUndefinedKind ==1)
		{
			// for (i=0,j=0;i<dataNum;i++)
			{
				// if( dataKind[i]  ==  NeuronNode_ForUndefined)
				{
					j=   Process_SearchObjForStr( DataFlow , dataKind ,  dataNum, &forecastInfo_st,GodNero);
				}
			}


		}
        // Process_ObjForecast(&forescastInfo_st);
		//forescastInfo_st是个全局变量，千万别过度使用它，宁可另设置个结构变量

		#ifdef Nero_DeBuging06_05_17_
		// printf("forecastInfo_st.objNum  =  [%d] \n ",forecastInfo_st.objNum);

		for(i=0;i<  forecastInfo_st.objNum;i++)
		{
			printf("forecastInfo_st.objNum=%d,dataKind[%d]=%d, in :%x \n ",forecastInfo_st.objNum,i,nero_GetNeroKind(forecastInfo_st.objs[i]),(forecastInfo_st.objs[i]));
		}
		#endif
		tmpObiForTest = Process_ClassiFication(&forecastInfo_st,GodNero);
		// if(tmpObiForTest != NULL)
		// {
		// 	printf("Process_ClassiFication  = %x,[%d] \n ",tmpObiForTest,nero_GetNeroKind(tmpObiForTest));
		// }

	}
        /***************************************************************/
/*        printf("coutOferror=%d.\n",coutOferror_Msg_);*/
    // 分类的准确含义是什么？仅仅是找出来一个最相似的basekind么，还是包括find out与传入的objs最类似的上层衍生obj
    // it doesnot mattar ,if need to find a basekind ,just return the basekind obj.




	#ifdef DataFlowProcess_error_Msg_
		printf("coutOferror_Msg_   13:%d.\n",coutOferror_Msg_);
	#endif


	#ifdef DataFlowProcess_error_Msg_
		//print objs  array
		printf("after Process_ObjForecast:\n");
		for(tmpc=0;tmpc < forecastInfo_st.objNum;tmpc++)
		{

			printf("%x{%d}  ", forecastInfo_st.objs[tmpc] ,nero_GetNeroKind(forecastInfo_st.objs[tmpc]) );

		}
		printf(" \n\n\n" );
	#endif



	#ifdef DataFlowProcess_error_Msg_

		if (coutOferror_Msg_ <= Nero_TestCount )
		{

		        printf("DataFlowForecastInfo:\n    objNum=%d\n    objPoint=%d ",forecastInfo_st.objNum,forecastInfo_st.objPoint);
		        printf("\n    UpperLayerobjNum=%d\n    SameLayerobjNum=%d\n    LowerLayerobjNum=%d\n    dataNum=%d\n\n ",forecastInfo_st.headOfUpperLayer.times,forecastInfo_st.headOfSameLayer.times,forecastInfo_st.headOfLowerLayer.times,dataNum);
		}
   		 #ifdef DataFlowProcess_error_Msg
		tmpc=0;
	    // if (coutOferror_Msg_ <= Nero_TestCount )
	    {

	           for (;tmpc<forecastInfo_st.objNum;tmpc++)
	           {
					neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
					neroObjMsgWithStr_st.fucId = 1;
					neroObjMsgWithStr_st.Obi = objs[tmpc];
					sprintf(neroObjMsgWithStr_st.str,"after ObjForecast识打印识别对象obj =%x id=%d",objs[tmpc],coutOferror_Msg_);
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	           }
	    }
	    // printf("\n");
    	#endif
    #endif


		/*	printf("start形成层次结构* \n");*/

	objNum=forecastInfo_st.objNum;

	/*将这几个对象形成层次结构*/
	/*其实就是将这几个对象形成一个新的对象，见神经网络记录 sheet   5系统概略图*/
	/*
		你要做两件事情，1:看看子概念的子集有没有组成概念----------------这个是非常麻烦的，
					也是整个系统中最关键的机制，比如你输入一个句子
					怎么在大量句子中生成新词呢
				2:整个数组形成新概念
	*/
	 /*这个开关打开的时间说明可以进行新基类（抽象概念）创建了*
		it is obvious that  :  objs[0]  is the  name  of  the  new kind
	 */
	 	// printf("1 CreateNewBaseObjKind=%d.  ifHasUnknowObj=%d\n",conf->CreateNewBaseObjKind,ifHasUnknowObj);



	      #ifdef Nero_DeBuging06_02_14_
	      int tmpi;
	      printf("objNum=%d  ifHasUnknowObj=%d\n",objNum,ifHasUnknowObj);

	      for (tmpi=0;tmpi<objNum;tmpi++)
	      {
	      		if(nero_GetNeroKind(objs[tmpi]) !=  62)
	              printf("list  obj(%x)  kind=%d.  nextBaseKind  May  be=%d\n",tmpi,nero_GetNeroKind(objs[tmpi]),conf->NewNeroClassID);
	         	else
	              printf("list  obj(%s):  kind=%d.  nextBaseKind  May  be=%d\n",DataFlow[tmpi],nero_GetNeroKind(objs[tmpi]),conf->NewNeroClassID);

	      }
	      #endif
		#ifdef Nero_DeBuging09_01_14_
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:its time to CreateNewBaseObj,objNum=%d,nextBaseKind=%d",objNum,conf->NewNeroClassID);
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif


		if(ifHasUnknowObj == 1  ||  ifHasUndefinedKind == 1)
		{

			// printf("has  unknow obj ,so you can not  CreateNewBaseObjKind.........................\n");
		}
		else if (conf->CreateNewBaseObjKind == 1  &&  ifHasUnknowObj == 0)
        {

              // printf("\n");
              /*判断是否可以进行新基类（抽象概念）创建*/
              res2=Process_IfCreateNewBaseObj(objs,objNum,GodNero,conf);
              /*开始添加基类 */
              if (res2 == NeroYES)//do not create new kind
              {
	  				#ifdef Nero_DeBuging09_01_14_
					neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
					neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
					neroObjMsgWithStr_st.Obi = NULL;
					sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:start create new basekind=%d !",conf->NewNeroClassID);
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
					#endif

              		   // printf("need  to create new kind\n");
		            tmpBaseObi=nero_CreateNewBaseObj(objs,objNum,GodNero, conf);
                      // printf("NewBaseObj:%x\n",tmpBaseObi);
                    tmpBaseObi2=nero_CreateNewBaseObjInSAP(objs,objNum,SAGodNero, conf);

					#ifdef Nero_DeBuging06_02_14
					if(tmpBaseObi)
					{
						// for (i=0;i<objNum;i++)
						{
							// printf("new BaseKind =%d\n",nero_GetNeroKind(tmpBaseObi));
						}
						// printf("所有child对象kind为:\n");
						curFiber=tmpBaseObi->inputListHead;
						if(curFiber == NULL)
						{
							// printf("  inputListHead =NULL  \n");
						}
						// while(curFiber)
						// {
						// 	tmpObiForTest=curFiber->obj;
						// 	printf("          kind:%d\n",nero_GetNeroKind(tmpObiForTest));
						// 	curFiber=curFiber->next;
						// }

		  				#ifdef Nero_DeBuging09_01_14
						neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
						neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
						neroObjMsgWithStr_st.Obi = NULL;
						sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:nero_CreateNewBaseObj success,new basekind=%d !",nero_GetNeroKind(tmpBaseObi));
						msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
						#endif

					}
					else
					{

		  				#ifdef Nero_DeBuging09_01_14
						neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
						neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
						neroObjMsgWithStr_st.Obi = NULL;
						sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:nero_CreateNewBaseObj fail !");
						msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
						#endif
						// printf("nero_CreateNewBaseObj fail \n\n\n");
					}
					#endif
              }
              else
              {
  				#ifdef Nero_DeBuging09_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
				neroObjMsgWithStr_st.Obi = NULL;
				sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中:not create newbase,but see if need to Modify it,nextBaseKind=%d",conf->NewNeroClassID);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif
                 /*如果不需要添加新的基类，那就是修改基类了*/
          		// printf("do not create new base  kind,but  see  if need to  Modify it\n");
				if(res2 !=   nero_msg_unknowError )
				{
				// printf("Modify base kind\n");
					newBaseObjKind_= res2;
					nero_ModifyBaseKind(objs,objNum,GodNero,conf,res2);
				}
              }
              /*很显然对于那些特殊高层衍生概念的创建createObjFromMultiples会出现问题
              事实上：队伍数字这个抽象概念来说，如果输入了一个字符1，这个这个字符很可能在
              Process_ObjForecast(&forecastInfo_st);已经被替换为数字类型的概念，也可能
              不被替换，显然这需要上下文来判断   nero_msg_unknowError
              */

              /*这里大概需要保证如果objNum-1=1时这个对象必须是不存在的，如果存在的话
              在Process_ObjForecast(&forecastInfo_st);已经被替换为数字类型的概念
              */
              /*这时候你发现，一个概念出现了同时指向俩个基类的情况了*/
              // printf("nero_createObjFromMultiples  1111   \n");


              // this  is a  special  moment :so   you  just need to  create  Obj FromMultiples   in  new  created kind  obj
              // 这个函数只能处理  objNum-1 》1得情况

			if(res2 ==   NeroYES)
				newBaseObjKind_= nero_GetNeroKind(tmpBaseObi);
			else if(res2 !=   nero_msg_unknowError )
				newBaseObjKind_= res2;

			if(res2 !=   nero_msg_unknowError )
			{

	          	if(objNum > 2)
	          		tmpObi =nero_createObjFromMultiples( &(objs[1]), objNum-1);
	          	else
	          		tmpObi =nero_createObjFromSingleObj( (objs[1]),newBaseObjKind_,GodNero);
	            #ifdef Nero_DeBuging09_01_14_
	            printf("开始u创建了新类 objNum=%d\n",objNum);
	            if (tmpObi)
	            {
	                   printf("创建了新类 %x---kind=%d.objNum=%d\n\n",tmpObi,nero_GetNeroKind(tmpObi),objNum);
	            }

	            #endif


			}
			else
			{

				printf("do not create new obj  with  newbaseobj\n");

			}

 			#ifdef Nero_DeBuging10_01_14_
			// print  all  of  the  kind  obj
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId =2;
			neroObjMsgWithStr_st.Obi =NULL;
			// nero_s32int xxxxxx=NeuronNode_ForChWord;
			nero_s32int xxxxxx=2001;
			memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
     		#endif


        }
        else if (conf->addLevelObjAlways == 1    &&  ifHasUnknowObj == 0)
		{
				#ifdef   Nero_DeBuging04_01_14_
				char str[500];
				char str2[500];
				sprintf(str,"data/wordspic%d.dot",3);
				sprintf(str2,"xdot data/wordspic%d.dot",3);
				createNeroNetDotGraphForWords(GodNero, str);
				system(str2);

				#endif
				// printf("conf->addLevelObjAlways == 1:nero_createObjFromMultiples  2\n");
				#ifdef   Nero_DeBuging04_01_14_
				int  iilkjhwersd=0;
				for(;iilkjhwersd< 2;iilkjhwersd++)
				{
					printf(" addLevelObjAlways == 1===  objs[%d] =%x,kind=%d.\n",iilkjhwersd,objs[iilkjhwersd],nero_GetNeroKind(objs[iilkjhwersd]));
				}
				#endif

				complexObj=nero_createObjFromMultiples( objs, objNum);

				#ifdef   Nero_DeBuging28_04_17_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj
				neroObjMsgWithStr_st.Obi = NULL;
				int  iilkjhwersd=0;
				if( complexObj == NULL)
				{//Process_tmpStr
					// for(;iilkjhwersd< objNum;iilkjhwersd++)
					// {
						// if(iilkjhwersd == 0)
							// if(DataFlow[0] != NULL)
								iilkjhwersd=snprintf((char *)Process_tmpStr,sizeof(Process_tmpStr),"ssssss");
						// else
							// sprintf(neroObjMsgWithStr_st.str,"%s %s",neroObjMsgWithStr_st.str,DataFlow[iilkjhwersd]);
					// }
								printf("%s,iilkjhwersd=%d,%s\n",Process_tmpStr,iilkjhwersd,DataFlow[0]);
				}
				// snprintf(neroObjMsgWithStr_st.str,3000,"%s\n",neroObjMsgWithStr_st.str);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				// printf("%s,iilkjhwersd=%d,%s\n",Process_tmpStr,sizeof(Process_tmpStr),DataFlow[0]);
				#endif
				#ifdef   Nero_DeBuging04_01_14_
				char str[500];
				char str2[500];
				sprintf(str,"data/wordspic%d.dot",3);
				sprintf(str2,"xdot data/wordspic%d.dot",3);
				createNeroNetDotGraphForWords(GodNero, str);
				system(str2);
				#endif

		}
		else if(  ifHasUnknowObj == 0 &&  objNum >= 2)
		{
			#ifdef DataFlowProcess_error_Msg_
			printf("coutOferror_Msg_  before  StrengthenLink=%d.\n",coutOferror_Msg_);
			#endif
			/*如果不把这些概念形成一个新的概念，就把他们联系起来，就是用输出链表连接起来
			对于已经连接的对象则加强连接强度
			*/
	/*		printf("已经连接的对象则加强连接强度\n");*/

			/*这里面有一个问题，*/
			TmpStagingAreaNero =NULL;
			res1=Process_StrengthenLink(objs,objNum,GodNero, conf,&TmpStagingAreaNero);
	/*		printf("res1=%d.\n",res1);*/

			/*如果发现强度足够高时则生成新概念*/
			/*如果子概念分别为a b c,而 b c  已经组成了概念，那么这个由a b c  组成的概念和b c
			组成的概念是什么关系呢
			*/
			/*res1  ==  Process_msg_CreateNewObj得意思是说，临时区域中得某个对象已经可以转换为永久对象了*/
			// 转换需要注意得点是：
			// 1：永久区域得对象还是要重新申请，但是数据一定要判断是否可以复制
			// 2：临时区域中得对象怎么处理，这会牵扯到许多链接失效得问题必须考虑清楚
			// 3：
			if (res1  ==  Process_msg_CreateNewObj  && conf->addLevelObj == 1  && TmpStagingAreaNero != NULL)
			{
				// 进入到这里意味着由objs组成得一个临时区域中得对象(TmpStagingAreaNero)已经可以转化为永久对象了

				//
				nero_TransferSAPoolObj(GodNero,SAGodNero, conf,TmpStagingAreaNero);



				/*强度暂时先不归0，因为这样的结果还不清楚*/
				#ifdef Nero_DeBuging09_01_14_
				if (tmpObi)
				{
					neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
					neroObjMsgWithStr_st.fucId = 1;
					neroObjMsgWithStr_st.Obi = tmpObi;
					sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中创建高级衍生对象成功");
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				}
				#ifdef DataFlowProcess_error_Msg
				else
				{
				        printf("创建高级衍生对象失败\n");
				        printf("objNum=%d.\n",objNum);
				}
				#endif
				#endif
			}
		}


	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   14:%d.\n",coutOferror_Msg_);
	#endif
	if(ifHasUndefinedKind != 1)
		Process_IoFuc( &forecastInfo_st,  complexObj,GodNero);


	// you should  init   neroConf  every time  you  run  this  fuc
	resetNeroConf();
	forecastInfo_st.controlMsg.Refreshed=0;



	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   1:%d.\n",coutOferror_Msg_);
	#endif
	// if (objs)
	// {
	// 	free(objs);
	// }
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   2:%d.\n",coutOferror_Msg_);
	#endif
	// for (j=0;j<dataNum;j++)
	// {
	// 	if (DataFlow[j])
	// 	{
	// 		free(DataFlow[j]);
	// 	}
	// }
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   3:%d.\n",coutOferror_Msg_);
	#endif
	// if (dataKind)
	// {
	// 	free(dataKind);
	// }
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   4:%d.\n",coutOferror_Msg_);
	#endif
	// if (DataFlow)
	// {
	// 	free(DataFlow);
	// }

	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   5:%d.\n",coutOferror_Msg_);
	#endif
	// printf("DataFlowProcess   end ......   \n\n");

	return nero_msg_ok;

}
	/*
	Its time to  output  what  you want to give  outside  some msg.
	The msg  you  need to  process is :
		1: search  ------->  DataFlowForecastInfo->headOfUpperLayer
						find if has operating neros,including input ,output
						find if has
		2:  check the  retuen  of nero_createObjFromMultiples()  has operating neros or not
		3:  struct NeroObjForecastControl  give the wants of outside
	*/
void  Process_IoFuc(struct DataFlowForecastInfo   * forecastInfo_st,  NeuronObject *  complexObj,NeuronObject  *godNero)
{
	NerveFiber *tmpFiber;
	NerveFiber *lowFiber;
	NeuronObject *  tmp;
	NeuronObject *tmp2;
		NeuronObject *tmp1;
	struct list_head  * listHead;
	nero_s32int  i,j;
	nero_us32int kind;
	// if(forecastInfo_st == NULL  ||  complexObj == NULL  || orecastInfo_st->headOfUpperLayer == NULL  ||   forecastInfo_st->objNum <= 0 || orecastInfo_st->objs == NULL)
	// {
	// 	return ;
	// }
	listHead=(struct list_head  *)&(forecastInfo_st->headOfUpperLayer);


	/*
		first : search forecastInfo_st list  and objs

	*/
	if(forecastInfo_st != NULL )
	{
		//search  objs first
		if(forecastInfo_st->objNum > 0 &&  forecastInfo_st->objs != NULL)
		{

			// tmp =
			for(i=0,tmp=(forecastInfo_st->objs)[i];i < forecastInfo_st->objNum  ; tmp=(forecastInfo_st->objs)[i++]  )
			{
				kind = nero_GetNeroKind(tmp);
				switch(kind)
				{
					case NeuronNode_ForInputWord:

						break;
					case NeuronNode_ForOutputWord:

						#ifdef Nero_DeBuging09_01_14
							// print  one  obj  link
							neroObjMsg_st.MsgId = MsgId_IO_ForOutputWord;
							neroObjMsg_st.fucId = 2;
							neroObjMsg_st.Obi = tmp;
							// int  tmp2222=0;
							// printf("nero   msg:%x,%x \n",GodNero,&tmp2222);
							msgsnd( IO_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);
						#endif


						break;
					case NeuronNode_ForLayering:
			// #define  NeuronNode_ForLayering      110   //定义一个基类a是另一个基类b得上层类，that is  mean：基类b得输出列表会指向基类a
											// inputListHead  为俩个数据，前者是基类a 得kind值(save  in x)，后者是基类b得得kind值

						tmpFiber=tmp->inputListHead;
						tmp1=nero_getBaseObjByKind(tmpFiber->obj->x,godNero);
						tmp2=nero_getBaseObjByKind(tmpFiber->next->obj->x,godNero);
						if(tmp1 != NULL  &&  tmp2 != NULL)
						{
							// 问题来了你这样在基类得输出列表中加入一个指向上层得链接是否会影响基类衍生类得搜索结果
							//so the  way to solve this problem is : make b's  Derivative Object point to baseobj   rather than
							// baseobj
							// 这样也不行阿，这样得只是讲现有得衍生对象设置了层次关系，那以后加入得关系仍然没有阿
							// 除非是这样，就是没隔一段时间就执行下这段代码(需要一个新得机制................)
							lowFiber= tmp2->outputListHead;
							while(lowFiber != NULL &&  lowFiber->obj != NULL)
							{

								PointingToObject(lowFiber->obj,tmp1,Fiber_PointToUpperLayer);
								lowFiber=lowFiber->next;
							}

						}


						break;

					default:
						break;
				}



			}

		}




	}




}
//现在的情况是只知道数据：DataFlow[i]  不知道对应的dataKind[i],
// 还有这些data，是由什么元数据组成的,such like :NeuronNode_ForChCharacter
// 为了加快进度，这里假设元数据就是NeuronNode_ForChCharacter(在 forecastInfo->controlMsg.metaData 中指定)

nero_s32int   Process_SearchObjForStr(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,struct DataFlowForecastInfo  * forecastInfo,NeuronObject * godNero)
{
	nero_us32int pointForObjsTmp=0,tmpInt_;
	NeuronObject *  tmpObiForTest;
	nero_8int tmpstr_[3];
	nero_s32int i,j,metaData,ii,findRes_;
	nero_us32int searchKind;
	NeuronObject  * tmpboj_=NULL;//
	NeuronObject * findobj;
	static nero_s32int firstRunFlag=0;
	static 	NeuronObject * objs [DataFlowPoolListNum][DataFlowPoolStrMaxLen];//
	static nero_s32int objListNumCount  [DataFlowPoolListNum];//记录每个objs[i]中子数据的个数（或者说kind 61 obj的个数）
	/*参数检查*/
	if (forecastInfo == NULL  || godNero ==NULL || DataFlow ==NULL || dataKind ==NULL   ||  dataNum <  1)
	{
		printf("Process_SearchObjForStr: parm错误\n");
		return 0;
	}

	metaData = forecastInfo->controlMsg.metaData;
	for(i=0 ;i < DataFlowPoolListNum ;i++)
	{

	// 	(objs)=(void **)malloc(sizeof(void *)* DataFlowPoolListNum);//dataNum 极值 is DataFlowPoolListNum
        for(j=0 ;j  < DataFlowPoolStrMaxLen ;j++)
        {

     	   objs[i][j] =NULL;
                // printf("malloc\n");
        }
	}
	j=0;
    for(i=0 ;i < DataFlowPoolListNum ;i++)
    {
		objListNumCount[i]=0;
    }
	firstRunFlag=1;



	if(metaData !=    NeuronNode_ForChCharacter)
	{
		printf("Process_SearchObjForStr: metaData !=    %d\n",metaData);
			return 0;
	}
	// printf("\ndataNum == %d\n",dataNum);
	for (i=0,j=0;i<dataNum;i++)
	{
		if( dataKind[i]  ==  NeuronNode_ForUndefined)
		{
			    // Process_SearchObjForStr( DataFlow , dataKind ,  dataNum, &forecastInfo_st,GodNero);
			// printf("i=%d,%s \n",i,DataFlow[i]);
			switch(metaData)
			{
				case NeuronNode_ForChCharacter:
					objListNumCount[i]= nero_getObjsByStr(metaData,DataFlow[i] ,i , (objs[i]) ,  godNero);
					#ifdef Nero_DeBuging03_05_17_
					// printf("objListNumCount[i]=%d\n",objListNumCount[i]);
					for (ii=0 ;ii<objListNumCount[i];ii++)
					{
						// tmpstr_[0]= (objs[i][ii]).inputListHead->obj->x;
						tmpboj_= objs[i][ii]->inputListHead->obj;
						if(tmpboj_ ==  NULL)
						{
							printf("Process_SearchObjForStr:tmpboj_ ==  NULL");
							break;
						}
						if ( tmpboj_->y == 0  && tmpboj_->z == 0)
						{
							printf("%c",tmpboj_->x);
						}
						else if(tmpboj_->y != 0  && tmpboj_->z == 0)
						{
							printf("%c%c",tmpboj_->x,tmpboj_->y);

						}
						else
						{
							printf("%c%c%c",tmpboj_->x,tmpboj_->y,tmpboj_->z);
						}
					}
					// printf("\n");.
					printf(" ");
					#endif
					break;
				default:
					break;
			}
			// for(ii=0;ii < dataFlowRstRcdInfo.actualNums;ii++)
			// {
			// 	printf("kind=%d,[%d]\n", dataFlowRstRcdInfo.expectKinds[ii],dataFlowRstRcdInfo.BaseOrObj[ii]);
			// }
			//现在尝试讲objs[i]数组的obj合并为一个上层对象
			if(objListNumCount[i] >  1)// nero_IfHasObjFromMultiples4要求输入的data个数要求大于1,so  
			{
				// 在这里你可以暂时抛弃子对象数为1的情况，因为那种情况下的上层类一般不好找
				tmpObiForTest = NULL;
				//你需要注意的是：事实上，这个函数返回的对象并不一定是你所想要的obj，可能只是一个拥有相同子对象的其他kind  obj
				//解决方案是加入一个期望obj的列表，由外界给出,根据列表对此出的子obj进行有条件的选择
				findobj=nero_IfHasObjFromMultiples5(objs[i],objListNumCount[i],&dataFlowRstRcdInfo,i,godNero);
				if(findobj  !=  NULL)
				{
					tmpObiForTest = findobj;
					// printf("i=%d ,objListNumCount[i]=%d,kind=%d,%x,forecastInfo->objNum=%d\n ",i,objListNumCount[i],nero_GetNeroKind(findobj),findobj,forecastInfo->objNum);

				}
				else
				{
					// printf("i=%d ,objListNumCount[i]=%d, \n ",i,objListNumCount[i] );
				}
			    if(tmpObiForTest  != NULL)
			    {
			    	// printf(" Process_SearchObjForStr : objListNumCount(%d)  %x,%d,store in %x ,i=%d \n ",objListNumCount[i],tmpObiForTest,nero_GetNeroKind(tmpObiForTest),&(forecastInfo->objs[i]),i);
			    	forecastInfo->objs[forecastInfo->objNum] = tmpObiForTest;
			    	forecastInfo->objNum = forecastInfo->objNum +1;
			    }
			    else
			    {
					// forecastInfo->objNum = 0;
					// printf(" Process_SearchObjForStr : cannot find obj,i=%d ,objListNumCount[i]=%d\n ",i,objListNumCount[i]);
					// return 0;
			    }

			}
		}
		else
		{
			forecastInfo->objs[forecastInfo->objNum] = nero_IfHasNeuronObject(DataFlow[i],dataKind[i], godNero);
			if(forecastInfo->objs[i]  != NULL )
				forecastInfo->objNum = forecastInfo->objNum +1;
			else
			{
				forecastInfo->objNum = 0;
				printf(" Process_SearchObjForStr : cannot find obj2222 \n ");
				return 0;
			}

		}
	}
	// printf("\n");

	return j;

}

//调用前一定要仔细确认struct DataFlowForecastInfo的数据是否需要修改
NeuronObject *  Process_ClassiFication(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * godNero)
{
	static nero_us32int  rich=0;
	static nero_us32int  poor =0;
	static nero_us32int  error=0;
	nero_us32int pointForObjsTmp=0;
    NeuronObject * tmp;
    nero_s32int i,objKind,baseKind,baseDataKind,res1;
    NeuronObject *Obi,*tmpObiForTest;
	NerveFiber *curFiber;
	/*参数检查*/
	if (forecastInfo == NULL  || godNero ==NULL )
	{
		printf("Process_ClassiFication: parm错误\n");
		return NULL;
	}
		// printf("   		forecastInfo_st.objNum=%d.\n",forecastInfo_st.objNum);
		// if(forecastInfo_st.objNum  == 4)
	tmpObiForTest =NULL;
	CleanForecastList( forecastInfo);
	while(pointForObjsTmp < forecastInfo->objNum)
	{
		#ifdef Nero_DeBuging10_01_14
 		res1= Process_ModifyObjsForForecastList(forecastInfo,pointForObjsTmp,forecastInfo->controlMsg.expectedKind,godNero);
 		// printf("   		res111=%d\n\n",res1);
 		// printf("\n");
 		if(res1 ==  nero_msg_ok)
 		{
			// printf("   		Process_ModifyObjsForForecastList:     obj[%d] objkind change to kind %d\n",pointForObjsTmp,nero_GetNeroKind(objs[pointForObjsTmp])  );
 		}
	 	#endif

		// printf("   		objkind=%d.,expectedKind=%d\n",nero_GetNeroKind(objs[pointForObjsTmp]) ,forecastInfo_st.controlMsg.expectedKind );
		Process_UpdataForecastList(forecastInfo,forecastInfo->objs[pointForObjsTmp]);


		#ifdef Nero_DeBuging10_01_14_

			neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
			neroObjMsg_st.fucId = 3;//Log_printNeroObjLinkTree
			neroObjMsg_st.Obi = objs[pointForObjsTmp];
			msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);

			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 3;//Log_printFormattedMsg
			neroObjMsgWithStr_st.Obi = objs[pointForObjsTmp];
			sprintf(neroObjMsgWithStr_st.str," ouputlistOBJ:isbase=%d\n" , nero_isBaseObj(forecastInfo->objs[pointForObjsTmp]));
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif
		// 打印一个obj outputListHead   信息
		#ifdef Nero_DeBuging10_01_14_

			neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
			neroObjMsg_st.fucId = 2;//Log_printNeroObjLink
			neroObjMsg_st.Obi = objs[pointForObjsTmp];
			msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);
		#endif

			pointForObjsTmp++;
	}

	#ifdef Nero_DeBuging09_01_14_
		NeuronObject * nodeobj;
		struct NeroObjForecastList *listNode;
		// struct NeroObjForecastList *listHead;
		struct list_head *p;
		struct list_head *head;
		nero_s32int _Strengthen;
		listNode =	&(forecastInfo_st.headOfUpperLayer );
		// listHead=listNode;
		head= (struct list_head *)listNode;
		p=head->next;

		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;//Log_printSomeMsgForObj

		while(p != head)
		{
			listNode = (struct NeroObjForecastList *) p;
			nodeobj = listNode->obj;
			_Strengthen = listNode->Strengthen;
			neroObjMsgWithStr_st.Obi = nodeobj;
			sprintf(neroObjMsgWithStr_st.str,"222get _Strengthen,,,,nodeobj=%x,kind=%d, Strengthen=%d",nodeobj,nero_GetNeroKind(nodeobj),_Strengthen);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);

			p = p->next;
		}
	#endif
		/*
	//you should check  the 	forecastInfo_st.objs   kinds  and  forecastInfo_st.objNum;
	printf("   		2check  the 	forecastInfo_st.objs num=%d.\n",forecastInfo_st.objNum);
	for(justForTest11 =0 ;justForTest11 < forecastInfo_st.objNum;justForTest11++)
	{
		printf("   		objkind=%d\n",nero_GetNeroKind((forecastInfo_st.objs)[justForTest11])    );
	}
	*/

 	tmpObiForTest=Process_ObjsClassiFication(forecastInfo);
	// after run  fuc Process_ObjsClassiFication, the variable (Process_forecastListNode[0]).obj;
	//is your  found obj  which is mostest related obj  with objs[]
	// but in order to improve accuracy, you can use (Process_forecastListNode[0]).obj to  modify
	// objs[]  ,for find  more suitable  objkind
	// 1：首先修正objs[]
	// 2：再次运行Process_ObjsClassiFication(&forecastInfo_st);

	#ifdef Nero_DeBuging10_01_14
	neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
	neroObjMsgWithStr_st.fucId = 3;//Log_printFormattedMsg
	neroObjMsgWithStr_st.Obi = NULL;
	sprintf(neroObjMsgWithStr_st.str,"matchObj1:%x,kind =%d,isbase=%d \n",tmpObiForTest,nero_GetNeroKind(tmpObiForTest) , nero_isBaseObj(tmpObiForTest));
	// if(nero_GetNeroKind(tmpObiForTest) ==  2031)
	// {	
	// 	rich++;
	// 	sprintf(neroObjMsgWithStr_st.str,"rich[%d]\n",rich );
		
	// }
	// else if( nero_GetNeroKind(tmpObiForTest) ==  2032 )
	// {
	// 	poor++;
	// 	sprintf(neroObjMsgWithStr_st.str,"pool[%d]\n",poor );
	// }
	// else
	// {
	// 		error++;	
	// 	sprintf(neroObjMsgWithStr_st.str,"error[%d]\n",error);
	// }
	msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	#endif



	//强制清除list
	forecastInfo->controlMsg.Refreshed=1;
	CleanForecastList( forecastInfo);


	return tmpObiForTest;
}
/*判断是否需要创建新基类,if找到了相同名的基类 retun  kind*/
nero_s32int  Process_IfCreateNewBaseObj(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf)
{
        NeuronObject * tmp;
        nero_s32int i,mark,objKind,baseKind,baseDataKind;
        NeuronObject *Obi,*tmpObi;
	NerveFiber *curFiber;

	/*参数检查*/
	if (objs == NULL  || godNero ==NULL  ||  objNum <2 || conf ==NULL)
	{
		printf("Process_IfCreateNewBaseObj: parm错误\n");
		return nero_msg_ParameterError;
	}
	// printf("neroConf.NewNeroClassID==%d\n",neroConf.NewNeroClassID);
	// nero_get
    /*首先把第一个字符或者词组类放在objs第一个位置*/
    mark=-1;
    for (i=0;i<objNum;i++)
    {
            tmp=objs[i];
            if (tmp == NULL)
            {
                    printf("IfCreateNewBaseObj 严重错误\n");
                    return nero_msg_unknowError;
            }

            objKind=nero_GetNeroKind(tmp);
            if (objKind == NeuronNode_ForChWord   ||  objKind == NeuronNode_ForChCharacter)
            {
                    mark=i;
                  break;
            }
    }
    if (mark != 0)
    {
           /*交换了*/
/*               printf("交换了\n");*/
           tmp=objs[0];
           objs[0]=objs[mark];
           objs[mark]=objs[0];
    }
    /*可以先判断是否已经有相应名字的抽象概念了*/
    curFiber=godNero->outputListHead;
	for (;curFiber !=NULL ;curFiber=curFiber->next)
	{

	         Obi=curFiber->obj;
	         baseDataKind=nero_GetNeroKind(Obi);
	         // printf("baseDataKind=%d\n",baseDataKind);
	        if (baseDataKind >=NeuronNode_MinNewDerivativeClassId)
	        {

	                /*判断基类名和objs[0]是否相同*/
/*	                printf("IfCreateNewBaseObj:  baseDataKind=%d.\n",baseDataKind);*/
/*	                printf("getBaseObjName= %x  objs[0]=%x.\n",getBaseObjName(Obi,godNero),objs[0]);*/
	                if (getBaseObjName(Obi,godNero) ==  objs[0])
	                {
	                         // printf("Process_IfCreateNewBaseObj:找到了相同名的基类%d ,objs[0]:%x\n",baseDataKind,objs[0]);
	                         return baseDataKind;
	                }
	                else
	                {
	                		// printf("Process_IfCreateNewBaseObj:cant not find 相同名的基类  bjs[0]:%x,objNum=%d\n",objs[0],objNum);

	                }

	        }


	}

        // baseKind=nero_judgeNewObjKind(&(objs[1]),objNum-1);
		// baseKind=conf->NewNeroClassID ;
		// conf->NewNeroClassID=baseKind+1;


/*        printf("IfCreateNewBaseObj :baseKind=%d.\n",baseKind);*/
        /*如果baseKind =NeuronNode_ForComplexDerivative，
        说明找不到合适的类来
        匹配数据，需要创建新类
        */
        // if (NeuronNode_ForComplexDerivative == baseKind    )
        // {
        // 		printf("找不到合适的类\n");
                return NeroYES;
        // }
        // else
        // {
        // 		printf("找到合适的类:%d\n",baseKind);
        // 		 return NeroNO;

        // }



}






/*对数组中的概念进行增强连接（有序的）操作，如果没有连接的添加一个连接，*/
/*如果返回1 ,则表面所有连接的强度都已经达到最大值*/

// before :这个算法得bug在于：objs中子集得出现可以造成相互链接
// 当子集出现够多时，会导致许多不相干得联系（也会提高
// 链接次数）影响此次得结果
// 解决方法是：第一次出现objs时就生成一个衍生对象，该对象
// 如果无法确定kind就给他一个临时得kind，否则就是确定得kind
// ，关键是：该对象存储于临时区域，当将来该对象得链接程度
// 足够高时就生成新得衍生类，如果无法确定类比就放在无指定类比中
//   so,the new  obj (Complex Derivative  class obj ) will  link the   obj  in   NeroPool,but the new obj is in   Staging Area Pool


// 另外，该对象放在临时区域中，它得链接强度指的是obj得y值， 不会影响永久存储
// 得衍生类得链接关系（强度）
/*如果返回1 ,则表面 连接的强度都已经达到最大值,so  created  new  obj  in  永久区域 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////这个函数强调得是数据流中,数据与数据之间得关系/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 当短时间内出现大量相似数据时，会有机制加强这几个数据类基类得联系，达到一定程度后形成新类
////////////////这个函数得最终目的是，当短时间内出现大量相似数据时，你得系统可能发现这些数据
////////////////可以识别为某种特定类型  or  不可以被识别，当无法识别时会被定义为NeuronNode_ForUndefined,
////////////////如果把NeuronNode_ForUndefined也认为是一种特殊得类，那么你得最终目的就只有一个
////////////////就是在这一群数据中找出俩种关系：
////////////////一种是数据与数据之间得关系（子对象与自对象之间）----------无监督学习,在这个函数中只能找到这种关系
////////////////一种是数据与上层衍生类之间得关系-----------------------监督学习,在这个函数中can  not 找到这种关系(数据流中就包含着类别信息)
////////////////这俩种关系都可以用来进行数据得预测和分类-----------------这里实现得是对数据得预测
			// 就拿adult数据集来说,如果进行监督学习,
			// 那么我们得目标就是在学习之后判断:
			// 如果只输入前面得部分数据,然后最后出现>50k
			// 或者<50k得可能性
			// 无监督学习最关键得是激励得机制,你算法得激励机制是什么?
			// 数据流objs中就是最后一个数据对象Tag
			// 1:除最后一个数据对象外,找出所有对象得UpperObj,
			// 	如果UpperObj得最后一个的数据正好是Tag
			// 则加入临时列表List中
			// 2:比如objs中得某个子对象为a1,它(找出来得)对应得临时列表List1,
			// 	首先是加强他们得链接强度:a1对List1中所有对象得链接强度,and outputlist列表中得位置往前移动一位
			// 其次，判断子对象得outputlist，减弱但不解除子对象指向得在临时区域中得其他临时对象


		// 稀疏离散表征得精髓是，每一个输入得数据都将加大或者更加接近最后输出得那个数据（也可能是一个对象或
		// 者对象，或者直接是一个基类）得可能性，也就是说你得算法导致得结果是，不仅你得系统中存储了大量得数据
		// 同时也对数据进行了整理和层次化
		//但是在找到最后那个数据前,你每输入一个数据就会更新你得结果输出列表
		// ,这个列表不仅指示了最后可能输出哪个对象,同时也包含可能性得大小
		// :就是每个对象得激活程度(类比神经细胞是否达到兴奋值)


nero_s32int Process_StrengthenLink(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf,NeuronObject ** alreadyTransferNero)
{
	nero_s32int Strengthen,i,j,flag,k,ifCreateObjInSAP;
	nero_s32int UpperObjKind,findKind;

// #define Process_TemporaryNUM   7500    //just used  in  fuc  Process_StrengthenLink
	nero_us32int     Process_tmpObiUsed;						//record    how many objs  store  in
// static NeuronObject Process_tmpObi[Process_TemporaryNUM];
	tmpObiUsed=0;

	/*参数检查*/
	if (objs == NULL  || godNero ==NULL  ||  objNum <2 || conf ==NULL)
	{
		return nero_msg_ParameterError;
	}
	ifCreateObjInSAP=0;
	flag=0;
	// 首先判断对象数组是否在临时区域中已经有衍生对象,根据数组中得子对象看看是否有指向临时区域得上层概念，如果数据数组中几个子对象
	// 同时指向一个临时对象，且输入顺序是一致得,thats it
	//first ,find  all  all  UpperObj  In SAPool  ,return the  list(save in  Process_tmpObi)

	Process_tmpObiUsed= FindUpperObjInSAPool(  objs,objNum, SAGodNero, Process_tmpObi);
		// 如果数据数组中得子对象都是指向未知类型得上层概念，直接生成临时衍生对象就好
		// 如果是已知类型，即是说数据数组中【部分连续得子对象】指向同一个已知类型得上层对象，那么加强这几个对象对概上层对象得链接强度
		// （这样得情况就是当达到一定链接强度后可以生成永久对象得例子。）---其他不需要加强得de数据，到最后往往就是垃圾数据，丢弃
				// 你不需要考虑这样处理如何区分分类还是筛选新对象得区别了，因为如果你是在做分类得工作，那么在学习完后，分类得
				// 				数据流得上层类就是无类别的数据，那么往往数据流就是预测最后一个数据。相反得，如果是生成新对象得化，一定是有
				// 				类型得情况，不然怎么生成新对象呢！！

		// 你别忘记一点：代码运行到这里，在Process_ObjForecast之后，已经表明在永久区域中
		// 这些子对象无法被已有得高级衍生类识别，那么他们就可能有2种情况：
		// 1：这些数据是一些数据流，没有事先指定类别（无监督学习），不管能否被归类，
		// 2：这些数据事先指定了类别（监督学习），但没有已经created 得衍生对象,
		//
		// ******************这个函数只处理事先指定类别（监督学习）得情况*****************************//////////
		// else if (conf->addLevelObjAlways == 1    &&  ifHasUnknowObj == 0)-------这个分支中处理先指定了类别（监督学习）得情况





	// see 系统运行逻辑记录350页

	// 因为是监督学习，数据流中就包含着类别信息,所以自己指定一个类,(花有红花，绿花等不同类得花，这里只能根据数据流得数据类型判断是花，而不知道是红花，绿花)
	/////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	findKind= nero_judgeNewObjKind( objs, objNum);
	if(NeuronNode_ForNone  ==     findKind   )
		return nero_msg_fail;

	if(Process_tmpObiUsed <=0)
	{
		//需要生成一个新得临时对象
		ifCreateObjInSAP =  1;
		flag  =  0;
		// printf("ifCreateObjInSAP=1,findKind=%d\n",findKind);
	}
	UpperObjKind=nero_GetNeroKind(  (Process_tmpObi[0]))  ;
	for(i=0,ifCreateObjInSAP=1;i<Process_tmpObiUsed;i++)
	{

		if(  UpperObjKind  ==     findKind   )
		{

			// 既然a1是属于基类a，那么将所以a得衍生类
			// 中出现子对象X1,Y1,Z1得其中一个得,就将其在
			// ///////////////////1//////////////a得outputlist列表中得位置往前移动一位///////////////////////////////////////
			/////2////// 同时，加强x1  list中指向得所有属于a类得实例得fiber链接强度///////////
			// //3///////////同样得，对于x1这个子对象来说，既然a1是a，那么将x1对象outputlist中属于a基类实例得对象位置都往前面移动一位
			// 所以我们得分类算法就是：遍历样本数据得子对象x，y，z得outputlist中出现a，b，c实例得次数，这个次数就是该对象属于某个类得可能性大小
			// 往前面移动得意义在于，如果x1这个对象得出现极大概率指向基类a，那么x1得输出列表中有很多属于a类得对象，而这些对象中链接强度最大得那个往往在列表得
			// 前部，也即是说，列表最前面得那个属于a类得对象得链接强度是最大得（因为这个fiber是最先出现得也是强化次数最多得）

			k=  nero_ifMakeUpWithTheseObjsInOrder((Process_tmpObi[i]),objs,objNum);
			if(k ==  NeroYES)
			{
				for(j=0;j<objNum;j++)
				{

					// 1:加强objs 中子对象得 list中指向得所有 属于 UpperObjKind类得实例(Process_tmpObi[i])得fiber链接强度
					nero_StrengthenLinkWithK(  &(objs[j]),UpperObjKind,(Process_tmpObi[i]));

					//2: 将Process_tmpObi[i]  将 UpperObjKind类得outputlist列表中得位置往前移动一位

					nero_MovingForwardOneStep( (Process_tmpObi[i]), SAGodNero,UpperObjKind);

				}

				// now  :判断是否需要生成新得永久obj
				// 关键是判断Process_tmpObi[i]数组中是否已经有对象
				// 可以包含这个objs[]中得所有对象
						// 1：显然这个包含得意思是，Process_tmpObi[i]得子对象和数组objs[]得对象
						// 		一一对应，且顺序一致,如果找不到这样得对象就需要生成一个新得临时对象
						// 2：当objs[]中得对象指向Process_tmpObi[i]得强度达到一定值
				// 这里需要明确一点：所有在临时区域中得非baseobj都是临时得对象
				// 可能在一段时间后被删除，那Process_StrengthenLink函数得部分意义
				// 就在于判断何时将临时对象转还为永久对象

				ifCreateObjInSAP=0;
				//查看链接强度,判断是否可以生成新得永久对象
				flag=getNeroTransferTag((Process_tmpObi[i]));//检查是否已经有转移标记
				if(flag != Nero_TransferToNeroPool)
					flag=nero_checkIfCreateObjInNP(Process_tmpObi[i],objs,objNum);
				else
					flag =0;
				if(flag == NeroYES)
				{
					flag =1;
					* alreadyTransferNero = (Process_tmpObi[i]);
					setNeroTransferTag(   (Process_tmpObi[i]),Nero_TransferToNeroPool);
				}
				//可以结束循环了
				break;
			}
		}

	}

	if(ifCreateObjInSAP ==  1  &&  UpperObjKind != NULL )
	{
		//需要生成一个新得临时对象

		// NeuronObject * nero_CreateObjInSAP(NeuronObject *Obis[],nero_s32int objNum,nero_s32int basekind,NeuronObject *godNero);
		nero_CreateObjInSAP( objs,objNum,UpperObjKind,SAGodNero);
		flag  =  0;
	}

	// ////////////最后，既然是临时对象，必须有遗忘得机制/////////////////////////
			// 一种解决方案是[定期减弱机制---in  short  time]：
			// 	在数据训练过程中，每当一个子对象被识别(这个识别过程得算法指的是：只要是子对象数组中部分连续对象和  SAPool中衍生对象中得部分连续子对象
			// 相同就可以)出来是和特定得临时区域得一个上层对象相关联时，首先是加强他们得链接强度
			// 其次，判断子对象得outputlist，减弱但不解除子对象指向得在临时区域中得其他临时对象，这意味着，如果
			// 该自对象大多数指向一个衍生概念时，会有赢者通吃得效应，相反得，如果，在所有样本中，它所指向得不同得
			// 上层衍生对象得次数相近时，结果就是永远不会有一个强度特别大得链接出来

			// 一种解决方案是[定期减弱机制2---in  long  time]：
				// chean  the whole   SAPool  per  one time

	if (flag  ==  1)
	{
/*	        printf("time to createNewObj\n");*/



		return  Process_msg_CreateNewObj;
	}
	return   nero_msg_ok;
}

#ifdef   Nero_DeBuging08_12_16_
nero_s32int Process_StrengthenLink_old(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf)
{

	nero_s32int Strengthen,i,j,flag;
	/*参数检查*/
	if (objs == NULL  || godNero ==NULL  ||  objNum <2 || conf ==NULL)
	{
		return nero_msg_ParameterError;
	}

	flag=1;
	for (i=0;i<objNum-1;i++)
	{


/*		for (j=i+1;j<objNum;j++)*/
/*		{*/

/*			Strengthen= nero_StrengthenLink(objs[i],objs[j]);*/
/*			if (Strengthen != Fiber_StrengthenMax)*/
/*			{*/
/*				flag=0;*/
/*			}*/
/*			*/
/*		}*/

/*			printf("change Strengthen obj  %x  ->>%x.\n",objs[i],objs[i+1]);*/
			Strengthen= nero_StrengthenLink(objs[i],objs[i+1]);

			if (Strengthen != Fiber_StrengthenMax)
			{
				flag=0;
/*				printf("flag  change\n");*/
			}



	}

	if (flag  ==  1)
	{
/*	        printf("time to createNewObj\n");*/
		return  Process_msg_CreateNewObj;
	}

	return   nero_msg_ok;
}

#endif

void * thread_for_Sys_Pic(void *arg)
{
/*
ActNero NeroPool[MaxNeroNum];

nero_us32int nextAvailableNeroInPool;*/

/*usleep(n) //n微秒*/
/*Sleep（n）//n毫秒*/
/*sleep（n）//n秒*/
	nero_us32int flag=0;
	neroConf.neroTime=1;

	/*做额外的初始化*/
	forecastInfo_st.DeBugMsg=0;
	forecastInfo_st.timeToMerage=0;
	forecastInfo_st.activateForecastObj=NULL;
	forecastInfo_st.waitForRecognise=NULL;

	forecastInfo_st.headOfUpperLayer.obj=NULL;
	INIT_LIST_HEAD(&(forecastInfo_st.headOfUpperLayer.p));
	forecastInfo_st.headOfUpperLayer.times=0;

	forecastInfo_st.headOfSameLayer.obj=NULL;
	INIT_LIST_HEAD(&(forecastInfo_st.headOfSameLayer.p));
	forecastInfo_st.headOfSameLayer.times=0;

	forecastInfo_st.headOfLowerLayer.obj=NULL;
	INIT_LIST_HEAD(&(forecastInfo_st.headOfLowerLayer.p));
	forecastInfo_st.headOfLowerLayer.times=0;

	forecastInfo_st.controlMsg.expectedKind =NeuronNode_ForNone;
	forecastInfo_st.controlMsg.baseORDerivative =1;
	forecastInfo_st.controlMsg.Refreshed =0;
	forecastInfo_st.controlMsg.DurationTime =0;


	dataFlowRstRcdInfo.actualNums =0;
	while(1)
	{
		/*死循环*/
		sleep(1);
		(neroConf.neroTime)++;
		flag++;
		if (flag==8)
		{
			printf("已经使用的nero数量:%d,剩余:%d,GodNero=%x   \n",neroConf.UsedNeroNum,MaxNeroNum-neroConf.UsedNeroNum,GodNero);


			#ifdef Nero_DeBuging14_01_14_
				// printf  msg  by  obj
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = GodNero;
				sprintf(neroObjMsgWithStr_st.str,"test:");
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
			#endif
			#ifdef Nero_DeBuging09_01_14_
				// print  one  obj  link
				neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
				neroObjMsg_st.fucId = 2;
				neroObjMsg_st.Obi = GodNero;
				int  tmp2222=0;
				printf("nero   msg:%x,%x \n",GodNero,&tmp2222);
				msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);
			#endif
	 		#ifdef Nero_DeBuging10_01_14_
				// print  all  of  the  kind  obj
				{
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				// nero_s32int xxxxxx=NeuronNode_ForChWord;
				nero_s32int xxxxxx=2003;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				}
	     	#endif

	 		#ifdef Nero_DeBuging10_01_14_
				// print  all  of  the  kind  obj
				{
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				// nero_s32int xxxxxx=NeuronNode_ForChWord;
				nero_s32int xxxxxx=NeuronNode_ForChWord;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
						}
	     	#endif
	 		#ifdef Nero_DeBuging10_01_14
				// print all  nero used  msg
				neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;//Log_printAllNeroMsg
				neroObjMsg_st.fucId = 4;
				neroObjMsg_st.Obi = GodNero;
				msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);
	     	#endif

			flag=0;
		}
/*		printf("neroTime:%d.\n",neroConf.neroTime);*/
	}
}



/*struct NeroObjForecastList*/
/*{*/
/*	struct list_head p;*/
/*	NeuronObject * obj;*/
/*	nero_s32int Strengthen;//在一次预测过程中可能一个对象被多次预测*/
/*	nero_s32int times;//在整个预测成功中，该节点存在的时间长度，*/
/*};*/

/*struct DataFlowForecastInfo*/
/*{*/
/*	NeuronObject ** objs;//实际对象指针*/
/*	nero_s32int objNum;//实际对象个数，也是objs这个数组的有效长度，数组长度必须大于objNum，不然越界	*/
/*	nero_s32int objPoint;//指向一个objs中可以读取的位置*/
/*        struct NeroObjForecastList   *headOfUpperLayer;//指向第一个预测对象	*/
/*	struct NeroObjForecastList   *headOfLowerLayer;//指向第一个预测对象*/
/*	struct NeroObjForecastList   *headOfSameLayer;//指向第一个预测对象		*/
/*};*/
/*更新列表预测列表
		关于合适的清空预测列表的方式有俩种：
				1：设置一个DataFlowProcess运行次数变量，一旦达到一个值就清空
				        也可以认为是一个存在的时间长度值
				2：寻找一个条件，达到条件就清空
				这里用第一种方法，NeroObjForecastList结构里面的time变量来表示
				*/
nero_s32int Process_UpdataForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * newObj)
{
		struct list_head * listPoint;
		struct list_head * listheadPoint;

        if (forecastInfo == NULL  || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL)
        {


	 		#ifdef Nero_ProcessERROR_Msg
			printf("UpdataForecastList ProcessERROR_Msg\n");
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi = NULL;
			sprintf(neroObjMsgWithStr_st.str,"Process_IfHasNextObjToread参数错误");
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
			#endif
			return nero_msg_ParameterError;
        }
        /*实际功能如下：
        1：更新列表预测列表中每个结构的time变量
        2：将forecastInfo  objPoint-1位置的对象的输出列表加入到预测列表当中去
        3：将time变量大于一定值的节点从列表中除去


        */
		#ifdef Nero_DeBuging09_01_14_
		neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
		neroObjMsg_st.fucId = 2;
		neroObjMsg_st.Obi = newObj;
		msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);
		#endif


        UpdataLastTimeINForecastList(forecastInfo);
        //讲newObj得输出列表得对象按指向得层次,分别加入到headOfUpperLayer  headOfSameLayer
       AddNewObjToForecastList( forecastInfo, newObj);

/*        #define  CleanForecastList_msgTest*/
        #ifdef CleanForecastList_msgTest_
	        static nero_us32int count_=0;
	        count_++;
	        printf("count_=%d.\n",count_);
        #endif

        // CleanForecastList( forecastInfo);


         #ifdef CleanForecastList_msgTest11111111111
        printf("count_=%d.-------------\n",count_);
        #endif


        #ifdef Nero_DeBuging01_03_17_
		listPoint =  &(forecastInfo->headOfUpperLayer.p );
		listheadPoint=listPoint;
		listPoint = listPoint->next;

		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;
		neroObjMsgWithStr_st.Obi = newObj;
		sprintf(neroObjMsgWithStr_st.str,"Process_UpdataForecastList: new node has add from outputListHead of  %x,listNUMs=%d \n",newObj,forecastInfo->headOfUpperLayer.times);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		while(listPoint !=  listheadPoint)
		{
			// printf("Process_ObjsClassiFication:list is empty....listPoint=%x,nums(%d)  \n",listPoint,forecastInfo->headOfUpperLayer.times);
			// return NULL;

			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi = newObj;
			sprintf(neroObjMsgWithStr_st.str,"			  add to list: %d\n",   ((struct NeroObjForecastList *)listPoint )->obj);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);

			listPoint = listPoint->next;
		}
		#endif



}
/*移除all节点  in  ForecastList
*/
void CleanForecastList(struct DataFlowForecastInfo  * forecastInfo)
{
        struct NeroObjForecastList   * findObiPoint;
        struct list_head  * p;
        struct list_head  * head;
/*        struct NeroObjForecastList * tmplistNode;*/
/*        #define Debug_CleanForecastList*/
        // #define Debug_CleanForecastList2
         #ifdef Debug_CleanForecastList
        printf("headOfUpperLayer-----------\n");
		#endif
        forecastInfo->objPoint =0;
        forecastInfo->activateForecastObj= NULL;
        forecastInfo->waitForRecognise= NULL;
        forecastInfo->waitForRecogniseObjPos =0;
        forecastInfo->start =0;
        forecastInfo->end =0;
        forecastInfo->timeToMerage =0;
        forecastInfo->DeBugMsg =0;
        forecastInfo->DeBugMsgTwo =0;



		#ifdef Nero_DeBuging24_01_14_
		nero_s32int i=0;
		p=(struct list_head  *)forecastInfo->headOfUpperLayer.p.next;
		head=(struct list_head  *)&(forecastInfo->headOfUpperLayer.p);
		while(p != head  &&  p != NULL)
		{
			p=p->next;
			i++;
		}
		if (p != head)
		{
			printf("链表出错,c=%d\n",i);
		}
		else
			printf("all counts=%d\n",i);

		#endif

        p=(struct list_head  *)(forecastInfo->headOfUpperLayer.p.next);
        head=(struct list_head  *)&(forecastInfo->headOfUpperLayer.p);
        while(p != head )
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                if (findObiPoint->times  > NeroMaxLastTimeINForecastList  ||  forecastInfo->controlMsg.Refreshed == 1 )
                {
                         /*移除这个节点*/
                        __list_del_entry((struct list_head *)p);
                        if (findObiPoint)
                        {
                                 p=p->next;
                                 #ifdef Debug_CleanForecastList2
                                printf("headOfUpperLayer-----------\n");
	                       		 #endif
                                free(findObiPoint);
                                (forecastInfo->headOfUpperLayer.times)--;
                                 #ifdef Debug_CleanForecastList2
                                printf("预测链表缩减。。。。。。。Refreshed=%d\n",forecastInfo->controlMsg.Refreshed);
		              			  #endif
		             		   continue;
                        }
                }

                p=p->next;

        }
         #ifdef Debug_CleanForecastList_
        printf("headOfLowerLayer-----------start------start------start------start------\n");
		#endif
        p=(struct list_head  *)forecastInfo->headOfLowerLayer.p.next;
         head=(struct list_head  *)&(forecastInfo->headOfLowerLayer.p);
        while(p != head)
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                if (findObiPoint->times  > NeroMaxLastTimeINForecastList ||  forecastInfo->controlMsg.Refreshed == 1 )
                {

                         /*移除这个节点*/
                        __list_del_entry((struct list_head *)p);
                        if (findObiPoint)
                        {
                                 p=p->next;
                                 #ifdef Debug_CleanForecastList2_
                                printf("headOfLowerLayer-----------\n");
	                       		 #endif
                                free(findObiPoint);
                                (forecastInfo->headOfLowerLayer.times)--;
                                 #ifdef Debug_CleanForecastList2_
                                printf("预测链表缩减。。。。。。。\n");
		             			   #endif
		             		   continue;
                        }


                }
                p=p->next;

        }




         #ifdef Debug_CleanForecastList_
        printf("headOfSameLayer-----start------\n");
		#endif
        p=(struct list_head  *)forecastInfo->headOfSameLayer.p.next;
         head=(struct list_head  *)&(forecastInfo->headOfSameLayer.p);
/*          i=0;*/
        while(p != head)
        {


                // printf("xunhuan:head:%x,Refreshed=%d,nums(%d)\n",head,forecastInfo->controlMsg.Refreshed,forecastInfo->headOfSameLayer.times);


                findObiPoint=(struct NeroObjForecastList   *) p;
				#ifdef Nero_DeBuging24_01_14_

      			  printf(" counts=%d  p=%x,prev=%x   next=%x\n",++i,p,p->prev,p->next);
       			 #endif
                if (findObiPoint->times  > NeroMaxLastTimeINForecastList ||  forecastInfo->controlMsg.Refreshed == 1 )
                {
                        /*移除这个节点*/
                        __list_del_entry((struct list_head *)p);
                        if (findObiPoint)
                        {
                                 p=p->next;
                                 #ifdef Debug_CleanForecastList2_
                                printf("headOfSameLayer-----------\n");
	                    	    #endif
                                free(findObiPoint);
                                (forecastInfo->headOfSameLayer.times)--;
                                 #ifdef Debug_CleanForecastList2_
                                // printf("预测链表缩减。。。。。。。\n");
								// printf("Process_ObjsClassiFication:list is empty....listPoint=%x,nums(%d)",listPoint,forecastInfo->headOfUpperLayer.times);
                                printf("预测链表缩减。。Refreshed=%d,nums(%d)\n",forecastInfo->controlMsg.Refreshed,forecastInfo->headOfSameLayer.times);
		               			 #endif
		               			 continue;
                        }
                }
  		#ifdef Nero_DeBuging24_01_14_

        printf(" p=%x,prev=%x   next=%x\n",p,p->prev,p->next);
        #endif
        p=p->next;

        }
                // printf(" end of CleanForecastList...........................\n");

}
void AddNewObjToList(struct DataFlowForecastInfo  * forecastInfo,nero_s32int FiberType,NeuronObject * Obj,nero_us32int  FiberStrengthen)
{

        struct list_head  * listHead;
                        /*加入列表*/

        if (FiberType == Fiber_PointToUpperLayer)
        {
             listHead=(struct list_head  *)&(forecastInfo->headOfUpperLayer);
        }
        else if(FiberType == Fiber_PointToSameLayer)
        {
                listHead=(struct list_head  *)&(forecastInfo->headOfSameLayer);
        }
        else
                listHead=NULL;


		#ifdef Nero_DeBuging10_01_14_
        if(nero_GetNeroKind(Obj) == 2012)
        {
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 3;//
			neroObjMsgWithStr_st.Obi = NULL;
			// printf("    FiberStrengthen=%d.\n",FiberStrengthen);
			sprintf(neroObjMsgWithStr_st.str," AddNewObjToList:   FiberStrengthen=%d,add=%x\n",FiberStrengthen,Obj);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		}
		#endif
        if (listHead !=NULL)
        {

			/*首先判断是否已经有这个概念*/
			struct NeroObjForecastList   *  findNodeINlist;
			findNodeINlist=Process_IfHasThisObjINList( listHead,Obj);

			if (findNodeINlist == NULL)
			{
				#ifdef Nero_DeBuging24_01_14
					printf("预测链表增长。。。。Obj=%x,kind=%d,FiberStrengthen=[%d],isbase=%d\n",Obj,nero_GetNeroKind(Obj),FiberStrengthen,nero_isBaseObj(Obj));
					neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
					neroObjMsgWithStr_st.fucId = 1;
					neroObjMsgWithStr_st.Obi = Obj;
					sprintf(neroObjMsgWithStr_st.str,"222预测链表增长 FiberStrengthen=%d",FiberStrengthen);
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif
				AddNodeIntoForecastList(listHead,Obj,  FiberStrengthen);
            }
            else
            {
                    /*对于刚被查询的概念，就是新输入的，但是列表中
                    已经有的概念延长起存在时间，已避免反复添加删除
                    */
                    (findNodeINlist->Strengthen)  +=FiberStrengthen;

                    (findNodeINlist->times)--;
					// printf("预测链表no增长。。。Obj=%x,kind=%d,FiberStrengthen=[%d][+%d],isbase=%d\n",Obj,nero_GetNeroKind(Obj),(findNodeINlist->Strengthen),FiberStrengthen,nero_isBaseObj(Obj));

            }

        }

}
void AddNewObjToForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * newObj)
{

	struct NeroObjForecastList   * findObiPoint;
	NerveFiber   * p;
	NeuronObject * Obj;
	nero_s32int i,FiberType,kind;
	NerveFiber * tmpFiber1;

    if (forecastInfo== NULL   || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL ||  newObj == NULL)
    {


		#ifdef Nero_DeBuging01_03_17_
			// printf("AddNewObjToForecastList  ProcessERROR\n");
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi = NULL;
			sprintf(neroObjMsgWithStr_st.str,"AddNewObjToForecastList:参数错误");
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif
		return ;
    }
 	#ifdef Nero_DeBuging01_03_17_
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;
		neroObjMsgWithStr_st.Obi = newObj;
		sprintf(neroObjMsgWithStr_st.str,"AddNewObjToForecastList: add newObj outputList:%x",newObj);
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	#endif
	/*控制替换的深度，这里只替换一层*/


		// 这里的条件控制实在是太操蛋了
		kind=nero_GetNeroKind(newObj);
		switch(kind)
		{
			case NeuronNode_ForNone:
			case NeuronNode_ForGodNero:
			case NeuronNode_ForData:
			case NeuronNode_ForUndefined:
			case NeuronNode_ForComplexDerivative:
				return;
				break;
			default:
				break;
		}

	// if ( kind  < NeuronNode_MinNewDerivativeClassId )
	{


         p=newObj->outputListHead;
        /*        printf("                p=%x,newObj=%x.\n",p,newObj);*/

        while(p != NULL)
        {
/*                printf("                p=%x,Obj=%x.\n", p,p->obj);*/
            Obj=p->obj;
            FiberType=getFiberType(p);
            if (Obj != NULL /* &&  nero_isBaseObj(Obj) != 1 */&& getFiberPointToPool(p ) == Fiber_ObjInNeroPool )
            {
                    AddNewObjToList( forecastInfo,FiberType,Obj,/*(p->msg1 & 0x000000ff) */  getFiberStrengthen(newObj,Obj));


				#ifdef Nero_DeBuging24_01_14_
					neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
					neroObjMsgWithStr_st.fucId = 1;
					neroObjMsgWithStr_st.Obi = NULL;
					sprintf(neroObjMsgWithStr_st.str,"AddNewObjToForecast:kind=%d,add=%x,point to add:%x,FiberStrengthen=%d",nero_GetNeroKind(newObj),newObj,Obj,getFiberStrengthen(newObj,Obj));
					// sprintf(neroObjMsgWithStr_st.str,"AddNodeIntoForecastList:预测链表增长,kind=%d,add=%x,FiberStrengthen=%d,ForecastList_Add=%x",nero_GetNeroKind(Obj),Obj,FiberStrengthen,newListNode);
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif

            }
            // getFiberStrengthen


            p=p->next;

        }
     }
    /*判断是不是要把newObj本身也加入预测列表：*/
    // if (forecastInfo->waitForRecognise  !=NULL)
    // {

	    // AddNewObjToList( forecastInfo,Fiber_PointToSameLayer,forecastInfo->waitForRecognise);
	    // forecastInfo->waitForRecognise=NULL;
    // }

}
/*判断预测列表中是否已经有这个概念*/
struct NeroObjForecastList * Process_IfHasThisObjINList(struct list_head  * listHead,NeuronObject * Obj)
{
        struct NeroObjForecastList *  findForecastObj;
        struct list_head  * p;
        struct list_head  * head;
        head=listHead;
        p=listHead->next;

        if (listHead == NULL  || Obj== NULL  )
        {
                return  NULL;
        }
        while(p != head )
        {
                findForecastObj=(struct NeroObjForecastList   *) p;
                if (findForecastObj->obj  == Obj)
                {

/*                        printf("IfHasThisObjINList obj=%x.  Obj=%x\n",findForecastObj->obj,Obj);*/
                        return findForecastObj;
                }
                p=p->next;
        }

	 return  NULL;

}
void AddNodeIntoForecastList(struct list_head  * listHead,NeuronObject * Obj,nero_us32int  FiberStrengthen)
{

        /*直接插入列表头部*/
        struct NeroObjForecastList *newListNode=(struct NeroObjForecastList *)malloc(sizeof(struct NeroObjForecastList));
        newListNode->obj=Obj;
        newListNode->Strengthen=FiberStrengthen;
        newListNode->times=0;
        newListNode->start=-1;
        newListNode->end=-1;

		#ifdef Nero_DeBuging24_01_14_
			// printf("预测链表增长。。。。p->next=%x。。。\n",p->next);
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi = Obj;
			sprintf(neroObjMsgWithStr_st.str,"AddNodeIntoForecastList:预测链表增长,kind=%d,add=%x,FiberStrengthen=%d",nero_GetNeroKind(Obj),Obj,FiberStrengthen);
			// sprintf(neroObjMsgWithStr_st.str,"AddNodeIntoForecastList:预测链表增长,kind=%d,add=%x,FiberStrengthen=%d,ForecastList_Add=%x",nero_GetNeroKind(Obj),Obj,FiberStrengthen,newListNode);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif

        // (newListNode->Strengthen)=FiberStrengthen;

        list_add((struct list_head  *)newListNode, listHead);
   	#ifdef Nero_DeBuging24_01_14_
        struct list_head  * p;
        struct list_head  * head;
        nero_s32int i=0;
        head=listHead;
        p=listHead->next;
        while(p != head )
        {
                p=p->next;
                i++;
        }
	if (p != head)
	{
	        printf("链表出错,c=%d\n",i);
	}
	else
	        printf("counts=%d\n",i);

	#endif
        // newListNode=(struct NeroObjForecastList *)listHead;


        ////////////z这里丢了一行代码，请补上
}
/*更新列表预测列表中每个结构的time变量*/
void  UpdataLastTimeINForecastList(struct DataFlowForecastInfo  * forecastInfo)
{
        struct NeroObjForecastList   * findObiPoint;
        struct list_head  * p;
        struct list_head  * head;
        p=(struct list_head  *)(forecastInfo->headOfUpperLayer.p.next);
         head=(struct list_head  *)&(forecastInfo->headOfUpperLayer.p);
        while(p != head)
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                (findObiPoint->times)++;
                p=p->next;

        }
        p=(struct list_head  *)forecastInfo->headOfLowerLayer.p.next;
        head=(struct list_head  *)&(forecastInfo->headOfLowerLayer.p);
        while(p != head)
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                (findObiPoint->times)++;
                p=p->next;

        }
        p=(struct list_head  *)forecastInfo->headOfSameLayer.p.next;
        head=(struct list_head  *)&(forecastInfo->headOfSameLayer.p);
        while(p != head)
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                (findObiPoint->times)++;
                p=p->next;

        }
}

/*判断DataFlowForecastInfo中 objPoint之前的数据是否能够有子集形成衍生类

但是一旦找到一个衍生类就修改整个子集对象是错误的，因为可能后面还有对象是属于
这个衍生对象的，判断条件是对该衍生的子集判断已经包含了整个objs子集的数据
*/
NeuronObject * Process_IfFindDerivativeObj(struct DataFlowForecastInfo  * forecastInfo)
{
        nero_s32int i,start,end;
        NeuronObject * findobj=NULL;
        if (forecastInfo== NULL   || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL)
        {


 		#ifdef Nero_ProcessERROR_Msg
/*		printf("找不到子概念\n");*/
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"Process_IfFindDerivativeObj参数错误");
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif
		return NULL;
        }
        forecastInfo->start=0;
        forecastInfo->end=0;
        /*实际上就是判断从0到，objPoint-1位置是否有子集的衍生类别*/
        for (start=0;start<=(forecastInfo->objPoint-1);start++)
        {

                for (end=start+1;end<=(forecastInfo->objPoint-1) /*&& end>=start*/;end++)
                {
                         findobj=nero_IfHasObjFromMultiples4(&((forecastInfo->objs)[start]),(end-start+1));
                        if (findobj != NULL)
                        {
                                #ifdef Nero_DeBuging25_01_14_
                                if (forecastInfo->DeBugMsg ==52 )
                                {

                                        printf("52 findobj=%x.\n",findobj);
                                }
                                #endif
/*                                printf("findobj=%x.\n",findobj);*/

                                forecastInfo->end=end;
                                forecastInfo->start=start;
                                break;
                        }


                }

        }

        return findobj;

}
/*看看预测列表中能不能找到findObi,

为了控制替换的深度，对查找进行了控制

*/
struct NeroObjForecastList   * Process_CompareWithForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * findObi)
{
        struct NeroObjForecastList   *head;
        struct NeroObjForecastList   *findObiPoint;
        if (forecastInfo== NULL   || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL  || findObi==NULL )
        {


 		#ifdef Nero_ProcessERROR_Msg
/*		printf("找不到子概念\n");*/
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"Process_CompareWithForecastList参数错误");
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif
		return NULL;
        }
       /*只在headOfSameLayer中寻找*/
/*       findObiPoint=  FindObjInForecastList(&(forecastInfo->headOfUpperLayer), findObi);*/
/*       if (findObiPoint == NULL )*/
/*        if (nero_GetNeroKind(findObi) < NeuronNode_MinNewDerivativeClassId )*/
       {
            findObiPoint=  FindObjInForecastList(&(forecastInfo->headOfSameLayer), findObi);
       }
/*       else*/
/*                findObiPoint=NULL;*/

        return findObiPoint;

}
struct NeroObjForecastList   *  FindObjInForecastList(struct NeroObjForecastList   *head,NeuronObject * findObi)
{
        struct NeroObjForecastList   * findObiPoint;
        struct list_head  * p;

        p=(struct list_head  *)(head->p.next);
        findObiPoint=NULL;
        while(p != &(head->p))
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                if (findObiPoint->obj  ==  findObi)
                {
                        (findObiPoint->Strengthen)++;
                        break;
                }
                else
                      findObiPoint=NULL;
                p=p->next;

        }
        return findObiPoint;

}
NeuronObject * Process_IfHasNextObjToread(struct DataFlowForecastInfo  * forecastInfo)
{

        if (forecastInfo== NULL   || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL)
        {


 		#ifdef Nero_ProcessERROR_Msg
/*		printf("找不到子概念\n");*/
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"Process_IfHasNextObjToread参数错误");
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif
		return NULL;
        }
         // printf("objPoint=%d,objs=%x,forecastInfo->objNum=%d\n",forecastInfo->objPoint,forecastInfo->objs,forecastInfo->objNum);

        if (forecastInfo->objPoint  >=  (forecastInfo->objNum))
        {
                return NULL;
        }
        forecastInfo->objPoint = forecastInfo->objPoint +1;
        return ( forecastInfo->objs)[forecastInfo->objPoint  -1];

}
/*获取obj在forecastInfo->objs中的位置，从0开始*/
 nero_s32int getPosInObjs(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * obj)
 {

         nero_s32int i=0;
         for (;i<forecastInfo->objPoint;i++)
         {
                 if ( forecastInfo->objs[i]  ==  obj)
                 {
                        return i;
                 }
         }

        return nero_msg_unknowError;
 }
  nero_s32int SetStartAndEndForListNode(struct DataFlowForecastInfo  * forecastInfo, NeuronObject   *obj)
 {
/*                                 printf(" 7GetNewActivateForecastObj=%x\n",forecastInfo->activateForecastObj->obj);*/
         nero_s32int pos=0;


/*        if (forecastInfo->waitForRecognise !=NULL)*/
/*        {*/
/*		pos=forecastInfo->waitForRecogniseObjPos;              */
/*        }*/
/*        else*/
                pos=forecastInfo->objPoint -1;
                                if (pos >= 0)
                                {
                                        if ((forecastInfo->activateForecastObj)->start <0   ||  (forecastInfo->activateForecastObj)->end <0)
                                        {
                                                /*如果之前没有被识别过的话*/
                                                (forecastInfo->activateForecastObj)->start=pos;
                                                (forecastInfo->activateForecastObj)->end=pos;
                                        }
                                        else
                                        {
                                                /*如果恰好是end的下一个的话*/
                                                if ((forecastInfo->activateForecastObj)->end == (pos-1))
                                                {
                                                        (forecastInfo->activateForecastObj)->end =pos;
                                                }
                                                /*如果恰好是end的下一个的话*/
                                                if ((forecastInfo->activateForecastObj)->start == (pos+1))
                                                {
                                                        (forecastInfo->activateForecastObj)->start =pos;
                                                }
                                                /*其他情况先不管*/

                                        }

                                }
                                else
                                {

/*                                printf(" 9GetNewActivateForecastObj=%x\n",forecastInfo->activateForecastObj->obj);*/
                                return NeroNO;
                                }

/* printf(" 8GetNewActivateForecastObj=%x\n",forecastInfo->activateForecastObj->obj);*/
        return NeroYES;

 }
/*获取一个新的ActivateForecastObj对象

方法：判断findObiPoint的上层概念是否已经在headOfUpperLayer中存在，找到就是它了
*/
nero_s32int Process_GetNewActivateForecastObj(struct DataFlowForecastInfo  * forecastInfo,struct NeroObjForecastList   *findObiPoint)
{

                nero_s32int res;
        struct list_head  * p;
        struct list_head  * head;
        NeuronObject * obj,* tmpobj;
        struct NeroObjForecastList   *listNode;
        NerveFiber  * curFiber;
        struct NeroObjForecastList * newActivateForecastObj;

        obj=findObiPoint->obj;
        curFiber=obj->outputListHead;
        while(curFiber)
        {
            tmpobj=curFiber->obj;
            if (getFiberType(curFiber) == Fiber_PointToUpperLayer  &&  nero_isBaseObj(tmpobj) != 1  && getFiberPointToPool(curFiber ) == Fiber_ObjInNeroPool)
            {
                    newActivateForecastObj= Process_IfHasThisObjINList((struct list_head  *) &(forecastInfo->headOfUpperLayer),tmpobj);

                    if (newActivateForecastObj)
                    {



                            forecastInfo->activateForecastObj=newActivateForecastObj;
                            /*修改newActivateForecastObj的信息*/
                            newActivateForecastObj->Strengthen++;
/*                                printf(" 1GetNewActivateForecastObj=%x.obj=%x\n",forecastInfo->activateForecastObj->obj,newActivateForecastObj->obj);*/
                            res=SetStartAndEndForListNode( forecastInfo,obj);
/*                                printf(" 4GetNewActivateForecastObj =%x.\n",forecastInfo->activateForecastObj->obj);*/
                            if (res == NeroNO)
                            {
/*                                        printf(" 3GetNewActivateForecastObj =%x.\n",forecastInfo->activateForecastObj);*/
                                    forecastInfo->activateForecastObj=NULL;
                                   return NeroNO;
                            }
                            else
                            {

			    				#ifdef Nero_DeBuging09_01_14_
			/*    				printf(" 2GetNewActivateForecastObj =%x.\n",forecastInfo->activateForecastObj);*/
								neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
								neroObjMsgWithStr_st.fucId = 1;
								neroObjMsgWithStr_st.Obi = forecastInfo->activateForecastObj->obj;
								sprintf(neroObjMsgWithStr_st.str,"GetNewActivateForecastObj 取得新对象%x id=%d",newActivateForecastObj,forecastInfo->DeBugMsg);
								msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
								#endif

                            	return NeroYES;
                            }

                    }
            }

            curFiber=curFiber->next;

        }
         return NeroNO;
}
/*判断findObiPoint是不是activateForecastObj的等待输入的对象

因为start 到end一般来说是这个衍生概念的子概念的子集，那么，下一个子概念是不是这个findObiPoint
呢，
*/
nero_s32int Process_IfIsChildOfActivateForecastObj(struct DataFlowForecastInfo  * forecastInfo,struct NeroObjForecastList   *findObiPoint)
{
        nero_s32int res=NeroNO;
        NeuronObject *childred=findObiPoint->obj;
        if (forecastInfo->activateForecastObj)
        {
                res= nero_ifMakeUpWithTheseObjs(forecastInfo->activateForecastObj->obj,&childred,1);
                 #ifdef Nero_DeBuging27_01_14_
                printf("IfIsChildOfActivateForecastObj:====\n");
                printf("findObiPoint->obj=%x.\n",findObiPoint->obj);
                printf("res=%d.\n",res);
                printf("activateForecastObj=%x.start,end<%d,%d>====\n",forecastInfo->activateForecastObj->obj,forecastInfo->activateForecastObj->start,forecastInfo->activateForecastObj->end);
                #endif
        }


        if (res != NeroYES)
        {
                return NeroNO;
        }
         return res;
}
/*判断是否可以合并objs数组

主要就是看activateForecastObj从start 到end是否涵盖了整个activateForecastObj对象的子概念

这里必须严格按照高层概念的子概念的排列顺序来判断
*/
void Process_IsTimeToMerage(struct DataFlowForecastInfo  * forecastInfo)
{

        nero_s32int i=0,flag=NeroYES,len,firstCompare=1;
        struct list_head  * p;
        struct list_head  * head;
        NeuronObject * obj,* tmpobj;
        struct NeroObjForecastList   *listNode;
        NerveFiber  * curFiber;
        struct NeroObjForecastList * newActivateForecastObj;
/*        printf("forecastInfo->activateForecastObj=%x.\n",forecastInfo->activateForecastObj);*/
        if (forecastInfo->activateForecastObj)
        {


                obj=forecastInfo->activateForecastObj->obj;
                curFiber=obj->inputListHead;
                len=forecastInfo->activateForecastObj->end - forecastInfo->activateForecastObj->start+1;
                i=forecastInfo->activateForecastObj->start;



                while(curFiber  &&  i<=forecastInfo->activateForecastObj->end)
                {

                        tmpobj=curFiber->obj;
                        /*因该把第一个i调整到和第一个tmpobj相等，再按顺序比较*/
                        if (firstCompare ==1)
                        {
                                firstCompare=0;
                                while(tmpobj !=  (forecastInfo->objs)[i]  && i<=forecastInfo->activateForecastObj->end)
                                {
                                        i++;
                                }
                        }
                        if (tmpobj !=  (forecastInfo->objs)[i])
                        {
                                flag=NeroNO;
                                break;
                        }
                        curFiber=curFiber->next;
                        i++;
                }
                if ((i-1) == forecastInfo->activateForecastObj->end   &&   curFiber==NULL)
               // if (/*i == forecastInfo->activateForecastObj->end   && */  curFiber==NULL)
                {
        /*                 return flag;*/
                        if (flag  == NeroYES )
                        {
                                forecastInfo->timeToMerage=1;
                        }
                }
        /*         return NeroNO;*/
                #ifdef Nero_DeBuging27_01_14_
                printf("IsTimeToMerage:\n");
                printf("start=%d. end=%d  \n",forecastInfo->activateForecastObj->start,forecastInfo->activateForecastObj->end);
                printf("i=%d,curFiber=%x\n",i,curFiber);
                printf("timeToMerage=%d.\n\n",forecastInfo->timeToMerage);
                #endif
        }

                 #ifdef Nero_DeBuging27_01_14_
        else

                printf("无激活对象\n");
                #endif
}

	// int qsortCmp1(const void *a,const void *b);
	// 假设是对int排序的话,如果是升序,那么就是如果a比b大返回一个正值,小则负值,相等返回0,其他的依次类推

	// 这里要求降序排列
int qSortCmp1(const void *a,const void *b)
{
	// NerveFiber * fiberA;
	// NerveFiber * fiberB;
	int res;
	struct NeroObjForecastList * A;
	struct NeroObjForecastList * B;
	// nero_us32int fiberStrengthenA;
	// nero_us32int fiberStrengthenB;
	A = (struct NeroObjForecastList *)a;
	B = (struct NeroObjForecastList *)b;
	// printf("A->Strengthen=%d,B->Strengthen=%d\n",A->Strengthen,B->Strengthen);
	if(A == NULL || B == NULL)
	{

		return 0;
	}

	if(A->Strengthen  >  B->Strengthen)
		return  (-1);

	if(A->Strengthen  <  B->Strengthen)
		return  (1);

	return 0;
}

// 每次运行nero_s32int Process_UpdataForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * newObj)之前
// 都要判断这里的newObj是否符合要求，如果不符合，则直接丢弃也不拿来影响判断结果
//	判断的依据是根据forecastInfo_st.controlMsg.expectedKind 的值来判断newObj是否可以使用，或者需要修改类型
//the parameter checkObjPoint gave the positon of  (forecastInfo->objs)[checkObjPoint] ,which is  need to check out
nero_us32int  Process_ModifyObjsForForecastList(struct DataFlowForecastInfo  * forecastInfo,nero_us32int  checkObjPoint,nero_us32int expectedKind,NeuronObject * godNero)
{
	nero_us32int res,i,dataNum,flag,dataNum2;
	nero_us32int originalKind,targetKind,tmpkind;
	NeuronObject *  baseObj;
	NeuronObject *  expectedBaseObj;
	NeuronObject *  tmpObj;
	if(forecastInfo == NULL    )
	{

		// Refreshed == 0 mean the msg of controlMsg is old ,and DurationTime == 1 means the msg is outdate
    	printf("Process_ModifyObjsForForecastList1 :   parameter error\n");
		return nero_msg_fail;

	}
	if(    expectedKind <= NeuronNode_ForComplexDerivative )
	{

		// Refreshed == 0 mean the msg of controlMsg is old ,and DurationTime == 1 means the msg is outdate
    	// printf("Process_ModifyObjsForForecastList1 :   parameter error\n");
		return nero_msg_fail;

	}
	expectedBaseObj = nero_getBaseObjByKind(expectedKind,godNero);
	res = nero_msg_fail;
	//这里暂时不考虑双方数据个数不对等的情况
	// 注意一点：有些基础类的数据个数可能是可变的，
		// basekind，数据个数是不确定的
	if(expectedBaseObj)
		dataNum = nero_getObjDataNum(expectedBaseObj);
	else
	{
		printf("Process_ModifyObjsForForecastList2 :   expectedKind=%d\n",expectedKind);
		exit(0);
	}
	if(expectedKind  >  NeuronNode_ForComplexDerivative  )
		dataNum = dataNum -1;
	flag =  testBaseObjNum(expectedBaseObj, godNero);
	// printf("1---dataNum=(%d),forecastInfo->objNum=(%d)\n",dataNum,forecastInfo->objNum);
	if(dataNum ==  forecastInfo->objNum   ||   flag == 1 )
	{

		 // 根据数据的个数和结构的不同进行分类处理
		//先处理最简单的情况
		// printf("2---dataNum=(%d),forecastInfo->objNum=(%d)\n",dataNum,forecastInfo->objNum);
		if(dataNum ==  forecastInfo->objNum)
		{
			i = checkObjPoint;
			originalKind 	=nero_GetNeroKind( (forecastInfo->objs)[i] )  ;
			if(expectedKind >  NeuronNode_ForComplexDerivative )
				targetKind		= nero_getChildKind(expectedBaseObj,i+1 )   ;
			else
				targetKind		= nero_getChildKind(expectedBaseObj,i )   ;
			// printf("originalKind(%d),targetKind(%d),i=%d\n",originalKind,targetKind, i);
			if(   originalKind  !=  targetKind )
			{

				//现在尝试将originalKind 转化为 targetKind  obj
				// 能成功转化的前提是targetKind  obj的数据子对象正好是originalKind obj
				// 但是又分为两种情况：
				// 1：originalKind 完全就是targetKind的组成子数据
				// 2：originalKind 不是targetKind的组成子数据，但他们的组成子数据是相同的

				//现在只考虑第一种情况(子数据个数刚好就是1)
				// 实际上，targetKind应该已经有相应的以此originalKind obj为数据的实例了
				// you just need to find it
				// 根据nero_createObjFromMultiples的机制，新生成的对象，其子数据会指向上层对象，包括基类和实例

				tmpObj=nero_searchObjInOutputlistByKind((forecastInfo->objs)[i],targetKind,0,1);
				// printf("nero_searchObjInOutputlistByKind:tmpObj=%x,\n",tmpObj);
				if(tmpObj != NULL  &&  nero_isBaseObj(tmpObj ) ==  0 )
				{
					//if find the targetKind obj
					(forecastInfo->objs)[i]  =  tmpObj;
					res = nero_msg_ok;
					// break;
				}

			}

		}
	}

	return res;

}

/*

第一次运行Process_ObjsClassiFication(&forecastInfo_st);后，利用其返回值（referenceObj）,
修改objs的子数据的kind，来提升下次运行Process_ObjsClassiFication的准确率
那问题来了，修改到什么程度呢？因为很可能遇到referenceObj的数据结构和objs不一样的
情况，具体指的是其数目对不上，也可能出现objs中的对象无法转换为objs所要求的子数据kind
的情况
*/
nero_us32int Process_ModifyObjsForClassiFication(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * referenceObj,NeuronObject  *godNero)
{


	nero_us32int res,i,dataNum,flag,dataNum2;
	nero_us32int originalKind,targetKind,tmpkind;
	NeuronObject *  baseObj;
	NeuronObject *  tmpObj;
	if(forecastInfo == NULL  ||   referenceObj == NULL )
	{

		// Refreshed == 0 mean the msg of controlMsg is old ,and DurationTime == 1 means the msg is outdate
    	printf("Process_ModifyObjsForClassiFication :   parameter error\n");
		return nero_msg_fail;

	}
	res = nero_msg_fail;
	//这里暂时不考虑双方数据个数不对等的情况
	// 注意一点：有些基础类的数据个数可能是可变的，
		// 如果referenceObj是实例，数据个数是确定的
		// 如果referenceObj是basekind，数据个数是不确定的

	dataNum = nero_getObjDataNum(referenceObj);
	if(nero_GetNeroKind(referenceObj)  >  NeuronNode_ForComplexDerivative   &&  nero_isBaseObj(referenceObj) == 1)
		dataNum = dataNum -1;
	flag =  testBaseObjNum(referenceObj, GodNero);
	// printf("1---dataNum=(%d),forecastInfo->objNum=(%d)\n",dataNum,forecastInfo->objNum);
	if(dataNum ==  forecastInfo->objNum   ||   flag == 1 )
	{
		//这里暂时不考虑referenceObj是实例的情况
		if(nero_isBaseObj(referenceObj)   ==  1 )
		{
			 // 根据数据的个数和结构的不同进行分类处理
			//先处理最简单的情况
			// printf("2---dataNum=(%d),forecastInfo->objNum=(%d)\n",dataNum,forecastInfo->objNum);
			if(dataNum ==  forecastInfo->objNum)
			{
				for(i=0;i<dataNum;i++)
				{
					originalKind 	=nero_GetNeroKind( (forecastInfo->objs)[i] )  ;
					targetKind		= nero_getChildKind(referenceObj,i+1 )   ;
    				printf("originalKind(%d),targetKind(%d),i=%d\n",originalKind,targetKind, i);
					if(   originalKind  !=  targetKind )
					{

						//现在尝试将originalKind 转化为 targetKind  obj
						// 能成功转化的前提是targetKind  obj的数据子对象正好是originalKind obj
						// 但是又分为两种情况：
						// 1：originalKind 完全就是targetKind的组成子数据
						// 2：originalKind 不是targetKind的组成子数据，但他们的组成子数据是相同的

						//现在只考虑第一种情况(子数据个数刚好就是1)
						// 实际上，targetKind应该已经有相应的以此originalKind obj为数据的实例了
						// you just need to find it
						// 根据nero_createObjFromMultiples的机制，新生成的对象，其子数据会指向上层对象，包括基类和实例

						// baseObj = nero_getBaseObjByKind(targetKind , godNero );
						// dataNum2 = nero_getObjDataNum(baseObj);;
						// if(dataNum2 ==1  ||    (dataNum2 ==2)  &&    targetKind > NeuronNode_ForComplexDerivative )

						tmpObj=nero_searchObjInOutputlistByKind((forecastInfo->objs)[i],targetKind,0,1);
						printf("nero_searchObjInOutputlistByKind:tmpObj=%x,\n",tmpObj);
						if(tmpObj != NULL  &&  nero_isBaseObj(tmpObj ) ==  0 )
						{
							//if find the targetKind obj
							(forecastInfo->objs)[i]  =  tmpObj;
							res = nero_msg_ok;
							break;
						}

					}

				}

			}
		}

	}



	return res;
}



// 分类的准确含义是什么？仅仅是找出来一个最相似的basekind么，还是包括find out与传入的objs最类似的上层衍生obj
// it doesnot mattar ,if need to find a basekind ,just return the basekind obj.
NeuronObject *  Process_ObjsClassiFication(struct DataFlowForecastInfo  * forecastInfo)
{

/*
在nero_createObjFromMultiples中新得衍生类对象得数据对象（子对象）只指向了新得衍生类，并没有指向抽象基类
假如加入指向抽象基类得fiber，且这个fiber将 record 每个子对象在整个sys运行过程中该子对象 指向该抽象基类 得 次数
那个这个冗余数据将加快分类中查询链接次数得操作 (you has add this  fiber in  fuc)
但是问题是：在某一个时刻你无法保证输出列表中的顺序就是完全按照链接程度来排列的。因为这需要足够大的数据才行
*/
 // in fuc Process_ObjForecast  ,you has put all the upper obj into list of headOfUpperLayer,  headOfLowerLayer
// or headOfSameLayer,so you just need to search these list

	NeuronObject *tmpobj;
	NeuronObject *  matchObj;
	nero_us32int matchKind=NeuronNode_ForNone;
	nero_us32int     Process_tmpObiUsed=0;	//record    how many objs  store  in
	struct list_head * listPoint;
	struct list_head * listheadPoint;
	matchObj=NULL;
	nero_us32int flag,i;
	nero_8int tmpstr[150];
	nero_8int tmpstr2[350];

	NerveFiber  *  curFiber;

	if(forecastInfo == NULL  ||   ( forecastInfo->controlMsg.Refreshed == 0 && forecastInfo->controlMsg.DurationTime == 1 ) )
	{
		// Refreshed == 0 mean the msg of controlMsg is old ,and DurationTime == 1 means the msg is outdate
    	printf("Process_ObjsClassiFication :   parameter error,Refreshed(%d),(%d)\n",forecastInfo->controlMsg.Refreshed,forecastInfo->controlMsg.DurationTime );
		return NULL;
	}
/* struct NeroObjForecastControl
{
	nero_s32int expectedKind;//外界期望输出得对象类型
	nero_s32int baseORDerivative;//是基类还是衍生对象得实例,
								 // 1 ：基类，
								 // 0 :衍生类
	nero_s32int Refreshed;		//该信息是否在本次DataFlowProcess运行前被刷新过
								 // 1 ：has  Refreshed
								 // 0 :old msg  --------set 0  at the end of fuc DataFlowProcess
	nero_s32int  DurationTime;  //持续时间,就是指该expectedKind得有效期
								// 0: Duration all the time  until changed
								// 1: just be used  one time ,so if(Refreshed == 1  &&  DurationTime ==1),that mean ,expectedKind is Invalid 无效的
			/////////////DurationTime比Refreshed有更高的优先级，就是说，//////////////////
			/////////////即使Refreshed为0但DurationTime为0时，该标记依旧有效/////////////////
			//////////////但是一旦Refreshed为1，则必须在次函数中情kong list//////////////////////
};*/
	// 怎么进行排序呢？
	// first:put all obj that meets the  condition  into a new list or array,
	// second:sort list
	// 考虑到搜索目标的性质，这里只需要搜索list headOfUpperLayer

	// #define Process_TemporaryNUM   7500    //just used  in  fuc  Process_StrengthenLink
	// static NeuronObject  * Process_tmpObi[Process_TemporaryNUM];
	listPoint =  &(forecastInfo->headOfUpperLayer.p );
	listheadPoint=listPoint;
	listPoint = listPoint->next;
	if(listPoint ==  listheadPoint)
	{
		printf("Process_ObjsClassiFication:list is empty....listPoint=%x,nums(%d)  \n",listPoint,forecastInfo->headOfUpperLayer.times);
		return NULL;
	}
	// printf("Process_ObjsClassiFication:list i....listPoint=%x,headOfUpperLayer.nums(%d)  \n",listPoint,forecastInfo->headOfUpperLayer.times);
	// printf("		start  matchObj....\n");
	while(listPoint != NULL  &&  Process_tmpObiUsed < Process_TemporaryNUM  &&  listPoint != listheadPoint )
	{

		matchObj =  ( (struct NeroObjForecastList *)listPoint )->obj ;
		if(matchObj == NULL)
		{

			printf("Process_ObjsClassiFication:matchObj is empty.. \n");
			exit(0);
		}
		flag = nero_isBaseObj(matchObj);
		matchKind = nero_GetNeroKind(matchObj);

		#ifdef Nero_DeBuging10_01_14_
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 3;//Log_printFormattedMsg
			neroObjMsgWithStr_st.Obi = NULL;

			switch(matchKind)
			{
				// (neroObjMsgWithStr_st.str)[0]=0;
				case 2012:
					curFiber=matchObj->inputListHead;
					sprintf(neroObjMsgWithStr_st.str,"初始列表-----matchObj:%x,kind=%d,isbase=%d,ForecastList_Add=%x,data(%d):",matchObj,matchKind,flag,listPoint,nero_getObjDataNum(matchObj));
					msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
					while( flag == 0 &&  curFiber != NULL  &&  curFiber->obj != NULL)
					{
						// printf("初始列表-----初始列表-----初始列表-----初始列表-----.. \n");
						IO_getZhInNero(tmpstr,curFiber->obj);
						sprintf(neroObjMsgWithStr_st.str,"%s",tmpstr);
						msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
						curFiber=curFiber->next;
					}

					sprintf(neroObjMsgWithStr_st.str,",Strengthen=%d\n",( (struct NeroObjForecastList *)listPoint )->Strengthen);
					break;

				default:
					sprintf(neroObjMsgWithStr_st.str,"初始列表-----matchObj:%x,kind=%d,isbase=%d,Strengthen=%d\n",matchObj,matchKind,flag,( (struct NeroObjForecastList *)listPoint )->Strengthen);
					break;

			}
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
		#endif

		//
		if(	flag ==  forecastInfo->controlMsg.baseORDerivative   ||  3 ==  forecastInfo->controlMsg.baseORDerivative )
		{
			// if(forecastInfo->controlMsg.expectedKind  != NeuronNode_ForUndefined)
			{

				// if( matchKind == forecastInfo->controlMsg.expectedKind )
				{
					Process_forecastListNode[Process_tmpObiUsed ] = *( (struct NeroObjForecastList *)listPoint);
					// Process_tmpObi[Process_tmpObiUsed ] =  matchObj;
					Process_tmpObiUsed++;
					// printf(":matchObj:%x,kind=%d\n",matchObj,matchKind);

				}
			}
			// else
			// {
				// Process_tmpObi[Process_tmpObiUsed ] =  matchObj;
				// Process_forecastListNode[Process_tmpObiUsed ] = *( (struct NeroObjForecastList *)listPoint);
				// Process_tmpObiUsed++;
				// printf(":matchObj:%x,kind=%d\n",matchObj,matchKind);
			// }
		}


		listPoint= listPoint->next;
	}


	if( Process_tmpObiUsed  >=  Process_TemporaryNUM)
	{
   		 printf(" Process_ObjsClassiFication  : Process_tmpObiUsed  use out ................  \n");

	}
	#ifdef Nero_DeBuging10_01_14_

		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 3;//Log_printFormattedMsg
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"Befor Sort:\n");
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	// 打印出排序前后的所有数据
	for(i=0;i<Process_tmpObiUsed;i++)
	{
		matchObj =   (Process_forecastListNode[i]).obj;
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 3;//Log_printFormattedMsg
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"add:%x 	kind:%d 	isbase=%d,Strengthen=%d \n",matchObj,nero_GetNeroKind(matchObj) , nero_isBaseObj(matchObj),(Process_forecastListNode[i]).Strengthen);
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	}
	#endif

	// 假设是对int排序的话,如果是升序,那么就是如果a比b大返回一个正值,小则负值,相等返回0,其他的依次类推
	qsort(Process_forecastListNode,Process_tmpObiUsed,sizeof(Process_forecastListNode[0]),qSortCmp1);

	#ifdef Nero_DeBuging10_01_14_
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 3;//Log_printFormattedMsg
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"After Sort:\n");
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	// 打印出排序前后的所有数据
	for(i=0;i<Process_tmpObiUsed;i++)
	{
		matchObj=    (Process_forecastListNode[i]).obj;
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 3;//Log_printFormattedMsg
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"add:%x 	kind:%d 	isbase=%d,Strengthen=%d \n",matchObj,nero_GetNeroKind(matchObj) , nero_isBaseObj(matchObj),(Process_forecastListNode[i]).Strengthen);
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	}
	#endif




	//check out the result:
    // printf("the most possible  matchObj:%x,kind =%d,isbase=%d,Process_tmpObiUsed=%d \n",Process_tmpObi[0],nero_GetNeroKind(Process_tmpObi[0]) , nero_isBaseObj(Process_tmpObi[0]),Process_tmpObiUsed);

	#ifdef Nero_DeBuging10_01_14_
		matchObj=    (Process_forecastListNode[0]).obj;
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 3;//
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"matchObj:%x,kind =%d,isbase=%d \n",matchObj,nero_GetNeroKind(matchObj) , nero_isBaseObj(matchObj));
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
	#endif


    // return Process_tmpObi[0];
	if(Process_tmpObiUsed >= 1)
		return (Process_forecastListNode[0]).obj;

	else
	 	return NULL;
}

		// 这个函数的初衷是看看数组objs能否组成一个对象，该对象在sys已经存在，并且他的部分子对象在
		// 数组objs中
		// 但是现在希望加入新的功能：就是根据Forecast  Control   struct NeroObjForecastControl的
		// 信息尝试找出数组objs最可能属于那个类的对象，算法原理就是根据objs每个子对象指向的上层类
		// 次数的多少来判断,当然这个上层类必须满足结构struct NeroObjForecastControl指定的要求
void Process_ObjForecast(struct DataFlowForecastInfo  * forecastInfo)
{
        struct NeroObjForecastList   *findObiPoint;
        NeuronObject * findForecastObj;
	nero_s32int i,j,/*hasAddObj,hasNewObj,*/res1,objNum,OldobjNum/*,res2*/;
	NeuronObject * tmpObi;
/*	NeuronObject ** objs=NULL;*/
	/*现在首先尝试判断子概念子集是否有衍生概念*/
	/*但是是一旦找到子集的衍生概念是重新运行整个程序呢，还是修改后继续呢*/
	/*还是继续吧*/
	/*因为后面只是需要objs, j这俩个变量，而objs不需要重新申请也可以使用，只需要修改j就好了
	但是子概念和高层概念预测链表还是作为全局的好，这样的话当前的预测链表还能影响下次的东东
	 */
	 #define ObjForecast_DeBug_msg_

	nero_s32int ObjForecast_DeBug_Count=0;

    CleanForecastList( forecastInfo);

	 //get a  obj in  objs for  Forecast
	while( (tmpObi=Process_IfHasNextObjToread(forecastInfo))  !=   NULL)
	{
	 	ObjForecast_DeBug_Count++;
	 	forecastInfo->DeBugMsgTwo=ObjForecast_DeBug_Count;

	    /*与预测链表,just compare with headOfSameLayer  list进行比较，看能不能找到tmpObi	forecastInfo_st.activateForecastObj=NULL;				*/
		findObiPoint=Process_CompareWithForecastList(forecastInfo,tmpObi);
		if (findObiPoint != NULL)
		{
                #ifdef ObjForecast_DeBug_msg

                if (forecastInfo->DeBugMsg <= Nero_TestCount)
                {

                        printf("findObi=%x objNum=%d  ObjForecastTimes=%d _______\n",findObiPoint->obj,forecastInfo->objNum,forecastInfo->DeBugMsgTwo);
                }
                #endif


		        /*判断tmpObi是不是activateForecastObj的等待输入的对象*/
		        res1= Process_IfIsChildOfActivateForecastObj( forecastInfo, findObiPoint);
                #ifdef ObjForecast_DeBug_msg
                if (forecastInfo->DeBugMsg <= Nero_TestCount)
                {
                        printf("res1=%d \n",res1);
                }
                #endif
		        if (NeroYES  == res1)
		        {
/*		                if (forecastInfo->waitForRecognise != NULL)*/
/*		                {*/
/*		                        SetStartAndEndForListNode( forecastInfo, forecastInfo->waitForRecognise);*/
/*		                        forecastInfo->waitForRecognise=NULL;*/
/*		                }*/

		                /*更新activateForecastObj*/
		                SetStartAndEndForListNode( forecastInfo, tmpObi);

		        }
		        else if(NeroNO  == res1)
		        {
		        		//find  new  ActivateForecastObj   if  findObiPoint  is not  the  child  of current ForecastObj
		                Process_GetNewActivateForecastObj( forecastInfo, findObiPoint);
		                /*获取新的activateForecastObj*/
		                #ifdef ObjForecast_DeBug_msg
		                printf("NewActivate=%x objPoint=%x. \n",forecastInfo->activateForecastObj,forecastInfo->objPoint-1);
		                #endif
		        }
		        else
		                continue;
		        /*设置修改列表条件*/
                        Process_IsTimeToMerage(forecastInfo)  ;

		         /*修改*/
		      if (forecastInfo->timeToMerage ==1)
		      {
					#ifdef ObjForecast_DeBug_msg
					/*                                if (forecastInfo->DeBugMsg ==52 )*/
					{
						printf("timeToMerage------------- id=%d\n",forecastInfo->DeBugMsg);
						printf("activateForecastObj=%x.\n",forecastInfo->activateForecastObj);
						#ifdef Nero_DeBuging09_01_14
						neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
						neroObjMsgWithStr_st.fucId = 1;
						neroObjMsgWithStr_st.Obi = forecastInfo->activateForecastObj;
						sprintf(neroObjMsgWithStr_st.str,"Merage激活对象 id=%d",forecastInfo->DeBugMsg);
						msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
						#endif

                    }
                    #endif
                                //put       activateForecastObj  into  list objs  list
		                Process_MerageObjsList(forecastInfo);
		              /*修改过后就重新开始  循环------------bug*/
		                forecastInfo->activateForecastObj->start=-1;
		                forecastInfo->activateForecastObj->end=-1;
		                forecastInfo->activateForecastObj=NULL;
		                // forecastInfo->objPoint=0;
		                forecastInfo->timeToMerage=0;
		                continue;
		      }

		}
		else
		{
		        // forecastInfo->waitForRecognise=tmpObi;
		        /*让预测列表更新后（加入tmpObi）重新识别一遍tmpObi*/
		        // forecastInfo->waitForRecogniseObjPos=forecastInfo->objPoint-1;
		        // forecastInfo->objPoint=forecastInfo->waitForRecogniseObjPos;

		}
		// #ifdef ObjForecast_DeBug_msg
		// printf("before Updata tmpObi=%x.\n",tmpObi);
                // printf("objNum=%d objPoint=%d \n\n",forecastInfo->objNum,forecastInfo->objPoint);
        // #endif
         // newObj得输出列表得对象按指向得层次,分别加入到headOfUpperLayer  headOfSameLayer
		Process_UpdataForecastList(forecastInfo,tmpObi);
	}


	 	/*最后还有清理本次运行的垃圾数据，以免误导下次运行的结果*/
		ReSetForecastList( forecastInfo);
}
void Process_MerageObjsList(struct DataFlowForecastInfo  * forecastInfo)
{

		nero_s32int OldobjNum=forecastInfo->objNum;
		nero_s32int objNum=(forecastInfo->activateForecastObj->end-forecastInfo->activateForecastObj->start)+1;/*被替换掉的子集的对象个数*/

		nero_s32int start,end,i,j;
		forecastInfo->objNum=OldobjNum- objNum+1;   /*新的objNum值*/


		/*结束条件是end后面剩余的对象数OldobjNum-（forecastInfo_st.end+1)  */
		start=forecastInfo->activateForecastObj->start;
		end=forecastInfo->activateForecastObj->end;
		#ifdef Nero_DeBuging27_01_14_
		printf("MerageObjsList:\n");
		printf("start=%d. end=%d  \n",start,end);
		printf("OldobjNum=%d.  len=%d  new objNum=%d\n",OldobjNum,objNum,forecastInfo->objNum);
		printf("结束条件=%d.\n",(OldobjNum-(end+1)));
		#endif
       for (j=start,i=0;i<=(OldobjNum-(end+1));j++,i++)
       {


               if (j == start )
               {
                       (forecastInfo->objs)[j]=forecastInfo->activateForecastObj->obj;

               }
               else
                      (forecastInfo->objs)[j]=(forecastInfo->objs)[end+i];
       }



}
void ReSetForecastList(struct DataFlowForecastInfo  * forecastInfo)
{
       struct NeroObjForecastList   * findObiPoint;
        struct list_head  * p;
        struct list_head  * head;
/*        struct NeroObjForecastList * tmplistNode;*/
/*        #define Debug_CleanForecastList*/
/*        #define Debug_CleanForecastList2*/
	forecastInfo->timeToMerage=0;
	forecastInfo->end=-1;
	forecastInfo->start=-1;
        forecastInfo->waitForRecognise=NULL;



        p=(struct list_head  *)(forecastInfo->headOfUpperLayer.p.next);
        head=(struct list_head  *)&(forecastInfo->headOfUpperLayer.p);
        while(p != head )
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                findObiPoint->start=-1;
                findObiPoint->end=-1;
                p=p->next;

        }

        p=(struct list_head  *)forecastInfo->headOfLowerLayer.p.next;
         head=(struct list_head  *)&(forecastInfo->headOfLowerLayer.p);
        while(p != head)
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                findObiPoint->start=-1;
                findObiPoint->end=-1;
                p=p->next;

        }




        p=(struct list_head  *)forecastInfo->headOfSameLayer.p.next;
         head=(struct list_head  *)&(forecastInfo->headOfSameLayer.p);
/*          i=0;*/
        while(p != head)
        {

                findObiPoint=(struct NeroObjForecastList   *) p;

                findObiPoint->start=-1;
                findObiPoint->end=-1;

                p=p->next;

        }






}
void Process_ObjForecast_old(struct DataFlowForecastInfo  * forecastInfo)
{
        struct NeroObjForecastList   *findObiPoint;
        NeuronObject * findForecastObj;
	nero_s32int i,j,/*hasAddObj,hasNewObj,res1,*/objNum,OldobjNum/*,res2*/,dataNum;
	NeuronObject * tmpObi;
/*	NeuronObject ** objs=NULL;*/
	/*现在首先尝试判断子概念子集是否有衍生概念*/
	/*但是是一旦找到子集的衍生概念是重新运行整个程序呢，还是修改后继续呢*/
	/*还是继续吧*/
	/*因为后面只是需要objs, j这俩个变量，而objs不需要重新申请也可以使用，只需要修改j就好了
	但是子概念和高层概念预测链表还是作为全局的好，这样的话当前的预测链表还能影响下次的东东
	 */


	while( (tmpObi=Process_IfHasNextObjToread(forecastInfo))  !=   NULL)
	{

		/*与预测链表进行比较，看能不能找到tmpObi*/
		findObiPoint=Process_CompareWithForecastList(forecastInfo,tmpObi);


		/*如果找到tmpObi。则判断是否能够找到子集的衍生概念
		（既然是衍生概念，其实	Process_IfFindDerivativeObj只要在headOfUpperLayer中找
		子集的衍生概念）
		   找不到则直接在原来基础上更新列表，进行下次循环*/
		if (findObiPoint != NULL)
		{
/*		        printf("findObiPoint=%x,obj=%x\n",findObiPoint,findObiPoint->obj);*/
			/*判断有没有衍生概念能够被识别*/

                        #ifdef Nero_DeBuging25_01_14_
                        static nero_us32int al_=0;
                        al_++;
                        if (coutOferror_Msg_ ==52 )
                        {

                                printf("findObi=%x.al_=%d  allLen=%d\n",findObiPoint->obj,al_,forecastInfo->objNum);
                        }
                        #endif
			findForecastObj=Process_IfFindDerivativeObj(forecastInfo);
			if (findForecastObj != NULL)
			{
				#ifdef Nero_DeBuging09_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = (forecastInfo->objs)[(forecastInfo->objPoint-1)] ;
				sprintf(neroObjMsgWithStr_st.str,"IfFindDerivativeObj找到上层对象 当前对象为obj =%x  len=%d id=%d",neroObjMsgWithStr->Obi,forecastInfo->objPoint-1,coutOferror_Msg_);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);


				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = findForecastObj;
				sprintf(neroObjMsgWithStr_st.str,"IfFindDerivativeObj找到上层对象 obj =%x id=%d",findForecastObj,coutOferror_Msg_);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
				#endif
                                /*需要修改
                                不需要该预测列表，因为随着时间的推移会自动在后面清理掉
		               初始化forecastInfo_st重新开始*/
		                forecastInfo->objPoint=0;
		                OldobjNum=forecastInfo->objNum;
		                objNum=(forecastInfo->end-forecastInfo->start)+1;/*被替换掉的子集的对象个数*/
		                forecastInfo->objNum=OldobjNum- objNum+1;   /*新的objNum值*/

                                /*结束条件是end后面剩余的对象数OldobjNum-（forecastInfo_st.end+1)


                                */
                                printf("调整后新长度是=%d.\n",forecastInfo->objNum);
		               for (j=forecastInfo->start,i=0;i<=(OldobjNum-(forecastInfo->end+1));j++,i++)
		               {
		                         #ifdef DataFlowProcess_error_Msg_
		                        if (j >=  forecastInfo->objNum  || j>=dataNum)
		                        {

		                                 printf("OldobjNum= %d   end=%d  start=%d\n",OldobjNum,forecastInfo->end,forecastInfo->start);
		                                printf("错误，越界了j=%d.dataNum=%d  forecastInfo->objNum=%d   i=%d\n",j,dataNum,forecastInfo->objNum,i);

		                                exit(0);
		                        }
		                        #endif

		                       if (j == forecastInfo->start )
		                       {
		                               (forecastInfo->objs)[j]=findForecastObj;

		                       }
		                       else
		                              (forecastInfo->objs)[j]=(forecastInfo->objs)[forecastInfo->end+i];
		               }
			}
		}

		/*更新列表预测列表
		关于合适的清空预测列表的方式有俩种：
				1：设置一个DataFlowProcess运行次数变量，一旦达到一个值就清空
				        也可以认为是一个存在的时间长度值
				2：寻找一个条件，达到条件就清空
				这里用第一种方法，NeroObjForecastList结构里面的time变量来表示
				*/
/*                printf("tmpObi=%x\n",tmpObi);*/
		Process_UpdataForecastList(forecastInfo,tmpObi);
	}
}









