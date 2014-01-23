
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



	struct DataFlowForecastInfo  forecastInfo_st;	
	
	
			

void *thread_for_Operating_Pic(void *arg)
{
	int x=0,countRunTimes=0;
	int timeToWaitCandy=0;
	char *fileName=NULL;
	long MsgId;
	struct { long MsgId; char text[100]; } OperatingMsg;
	struct ZhCharArg * arg1;
	struct DataFlowProcessArg * arg2;
	key_t ipckey;
	int Operating_mq_id;

	int received,hasSetUpNeroSys;

	/* Generate the ipc key */
/*	Operating_ipckey="/home/ub/shareSpace/Operating_ipckey";*/
	
	 
/*	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
	#define IPCKEY 0x111
	ipckey = ftok(Operating_ipckey, IPCKEY);
/*	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
/*	printf("Operating_ipckey key is %d\n", ipckey);*/
/*	*/
	/* Set up the message queue */
	Operating_mq_id = msgget(ipckey,0);// IPC_CREAT
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
	printf("Operating_ipckey Message identifier is %d\n", Operating_mq_id);
	hasSetUpNeroSys=0;
/*	while( msgrcv(Operating_mq_id, &OperatingMsg, sizeof(OperatingMsg), 0, MSG_NOERROR) >1 );*/
	while(x == 0)
	{
/*		sleep(1);*/
/*		printf("wait for Operating msg......\n");*/
		received = msgrcv(Operating_mq_id, &OperatingMsg, sizeof(OperatingMsg), 0, MSG_NOERROR);
		if (errno != 0)
		printf("Operating strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
		if (received<1)
		{
			#ifdef Nero_DeBugInOperating_Pic
			 printf("received fail\n");
			#endif
			continue;
		}
		else
			#ifdef Nero_DeBugInOperating_Pic
			 printf("received  ok:\n");
			#endif
		MsgId=OperatingMsg.MsgId;
		
		switch(MsgId)
		{
		case MsgId_Nero_CreateNetNet:
			CreateActNeroNet();
			hasSetUpNeroSys=1;
			#ifdef Nero_DeBuging09_01_14
			 printf("MsgId_Nero_CreateNetNet:\n");
			#endif
			break;
		case MsgId_Nero_addZhCharIntoNet:
			arg1=(struct ZhCharArg *)OperatingMsg.text;
			nero_addZhCharIntoNet( GodNero,arg1->chChar, arg1->charCounts);
			#ifdef Nero_DeBuging09_01_14
			 printf("MsgId_Nero_addZhCharIntoNet:\n");
			#endif			
			
			
			break;	
		case MsgId_Nero_DataFlowProcess :
			arg2=(struct DataFlowProcessArg *)OperatingMsg.text;
			countRunTimes++;
			/*判断系统到底初始化没有*/
			if (hasSetUpNeroSys == 1)
			{
					
				
				DataFlowProcess(arg2->DataFlow,arg2->dataKind,arg2->dataNum,  GodNero, arg2->conf);
				#ifdef Nero_DeBuging09_01_14_
				 printf("MsgId_Nero_DataFlowProcess{%d}:\n",countRunTimes);
				#endif			
			
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
			else{
				printf("系统未初始化\n");			
			
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


nero_s32int DataFlowProcess(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf)
{
	nero_s32int i,j,hasAddObj/*,hasNewObj*/,res1,objNum/*,res2*/;
	NeuronObject * tmpObi;
	NeuronObject ** objs=NULL;
        struct NeroObjForecastList   *findObiPoint; 
        NeuronObject * findForecastObj;	
/*        struct NeroObjForecastList   *findForecastObjPoint; 	*/
	/*参数检查*/
	if (DataFlow == NULL  || dataKind ==NULL  ||  dataNum <1)
	{
		return nero_msg_ParameterError;
	}
/*	system("xdot data/wordspic.dot");*/
	(objs)=(NeuronObject **)malloc(sizeof(NeuronObject *)*dataNum);
	/*先不比对DataFlow  dataKind  dataNum*/
	/*断DataFlow中的数据是否在系统中已经存在该数据*/
	for (i=0,j=0,hasAddObj=0;i<dataNum;i++)
	{
	
				#ifdef   Nero_DeBuging04_01_14_
				char str[500];
				char str2[500];
/*				PrintUtf8 ttt;*/
/*				printf("\n");		*/
/*				ttt.tmp=*((ChUTF8 *)DataFlow);*/
/*				ttt.end=0;	*/
/*				printf("%s",(nero_s8int *)DataFlow[i]);	*/
				sprintf(str,"data/wordspic%d.dot",i);
				sprintf(str2,"xdot data/wordspic%d.dot",i);
				createNeroNetDotGraphForWords(GodNero, str);
				system(str2);
		
				#endif	
	
	
		/*先不管有句子的情况*/
		/*通过objs[j]里面的值就可以知道有没有在网络中找到这个对象*/

/*		objs[i]*/
		#ifdef Nero_DeBuging14_01_14_
		nero_us8int * tttttm=(char *)DataFlow[i];
		printf("寻找字符1：%x %x %x .\n",(int)tttttm[0],(int)tttttm[1],(int)tttttm[2]);
		#endif	
		tmpObi =nero_IfHasNeuronObject(DataFlow[i],dataKind[i], GodNero);

		#ifdef Nero_DeBuging21_12_13
		if (tmpObi == NULL  )
		{
				
				#ifdef Nero_DeBuging09_01_14_
				printf("找不到子概念\n");
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = tmpObi;
				sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中找不到该概念");
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
				#endif						
				
						
			
		}
		else 
		{
/*			printf("找到子概念\n");*/
		}			
		#endif
		/*如果不存在则尝试将该对象加入网络*/
		if (tmpObi == NULL  && conf->addNewObj == 1)
		{
			tmpObi=  nero_addNeroByData(DataFlow[i],dataKind[i]);
	
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
				#ifdef Nero_DeBuging09_01_14_
				printf("添加子概念成功\n\n");
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = tmpObi;
				sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中创建对象成功");
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
				printf("添加子概念失败\n\n");
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
			j++;
		}
		
		
		
		
	}
	
	/*现在首先尝试判断子概念子集是否有衍生概念*/
	/*但是是一旦找到子集的衍生概念是重新运行整个程序呢，还是修改后继续呢*/
	/*还是继续吧*/
	/*因为后面只是需要objs, j这俩个变量，而objs不需要重新申请也可以使用，只需要修改j就好了 
	但是子概念和高层概念预测链表还是作为全局的好，这样的话当前的预测链表还能影响下次的东东
	
	
	 */
/*struct NeroObjForecastList*/
/*{*/
/*	struct list_head p;*/
/*	NeuronObject * obj;*/
/*	nero_s32int Strengthen;//在一次预测过程中可能一个对象被多次预测*/
/*	nero_s32int times;//在整个预测成功中，该节点存在的时间长度*/
/*};*/

/*struct DataFlowForecastInfo*/
/*{*/
/*	NeuronObject ** objs;//实际对象指针*/
/*	nero_s32int objNum;//实际对象个数，也是objs这个数组的有效长度，数组长度必须大于objNum，不然越界	*/
/*	nero_s32int objPoint;//指向一个objs中可以读取的位置,初始为0，最大值为objNum*/
/*        struct NeroObjForecastList   *headOfUpperLayer;//指向第一个预测对象	*/
/*	struct NeroObjForecastList   *headOfLowerLayer;//指向第一个预测对象*/
/*	struct NeroObjForecastList   *headOfSameLayer;//指向第一个预测对象*/
/*	*/
/*	nero_s32int start;//start end 是objs中某个子集的起始位置，用来指示该位置有衍生概念*/
/*	nero_s32int end;*/
/*			*/
/*};*/
	/*初始化*/
	forecastInfo_st.objs=objs;
	forecastInfo_st.objNum=j;
	forecastInfo_st.objPoint=0;
/*	forecastInfo_st.head=NULL;//在thread_for_Sys_Pic(void *arg)中被初始化*/
	
	while( (tmpObi=Process_IfHasNextObjToread(&forecastInfo_st))  !=   NULL)
	{
		
		/*与预测链表进行比较，看能不能找到tmpObi*/	
		findObiPoint=Process_CompareWithForecastList(&forecastInfo_st,tmpObi);
		
	
		/*如果找到tmpObi。则判断是否能够找到子集的衍生概念		
		   找不到则直接在原来基础上更新列表，进行下次循环*/
		if (findObiPoint != NULL)
		{
			/*判断有没有衍生概念能够被识别*/
			findForecastObj=Process_IfFindDerivativeObj(&forecastInfo_st);
			if (findForecastObj != NULL)
			{
                                /*需要修改
                          	forecastInfo_st.objs=objs;要修改objs里面的值
	                        forecastInfo_st.objNum=j;
	                        forecastInfo_st.objPoint=0;                                      
                                不需要该预测列表，因为随着时间的推移会自动在后面清理掉              
		               初始化forecastInfo_st重新开始*/
		                forecastInfo_st.objPoint=0; 
		                j=forecastInfo_st.objNum;
		                forecastInfo_st.objNum=j-(forecastInfo_st.end-forecastInfo_st.start);    
		               for (j=forecastInfo_st.start,i=0;j<forecastInfo_st.objNum;j++,i++)
		               {
		                       if (j == forecastInfo_st.start )
		                       {
		                               (forecastInfo_st.objs)[j]=findForecastObj;
		                               
		                       }
		                       else
		                              (forecastInfo_st.objs)[j]=(forecastInfo_st.objs)[forecastInfo_st.end+i];
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
		
		Process_UpdataForecastList(&forecastInfo_st);
	}
	
	
	
	
	
	
	
	
	
	
	/*将这几个对象形成层次结构*/
	/*其实就是将这几个对象形成一个新的对象，见神经网络记录 sheet   5系统概略图*/
	/*
		你要做两件事情，1:看看子概念的子集有没有组成概念----------------这个是非常麻烦的，
					也是整个系统中最关键的机制，比如你输入一个句子
					怎么在大量句子中生成新词呢
				2:整个数组形成新概念
	
	
	
	
	*/

	if (conf->addLevelObjAlways == 1   )
	{
		/*这里必须说明的是，这个新生成的概念究竟是什么类型的，createObjFromMultiples内部会根据子类型自动指定*/
		/*但是这里不能用createObjFromMultiples，因为它里面有太多字符的东西，不够泛化*/
		
		
				#ifdef   Nero_DeBuging04_01_14_
				char str[500];
				char str2[500];
				sprintf(str,"data/wordspic%d.dot",3);
				sprintf(str2,"xdot data/wordspic%d.dot",3);
				createNeroNetDotGraphForWords(GodNero, str);
				system(str2);
		
				#endif			
		
		nero_createObjFromMultiples( objs, objNum);
				#ifdef   Nero_DeBuging04_01_14_
				char str[500];
				char str2[500];
				sprintf(str,"data/wordspic%d.dot",3);
				sprintf(str2,"xdot data/wordspic%d.dot",3);
				createNeroNetDotGraphForWords(GodNero, str);
				system(str2);
		
				#endif			
		
	}
	else
	{
	
		/*如果不把这些概念形成一个新的概念，就把他们联系起来，就是用输出链表连接起来
		对于已经连接的对象则加强连接强度
		*/
/*		printf("已经连接的对象则加强连接强度\n");*/

		/*这里面有一个问题，*/

		res1=Process_StrengthenLink(objs,objNum,GodNero, conf);
/*		printf("res1=%d.\n",res1);*/
		
		/*如果发现强度足够高时则生成新概念*/
		/*如果子概念分别为a b c,而 b c  已经组成了概念，那么这个由a b c  组成的概念和b c
		组成的概念是什么关系呢
		*/
		
		/*一旦形成新的概念，就需要对相应的连接的连接强度做一些修改，怎么样的修改呢？？*/
		if (res1  ==  Process_msg_CreateNewObj  && conf->addLevelObj == 1)
		{
			/*首先创建一个新概念，然后把这些子概念之间的链接强度归零*/
			tmpObi =nero_createObjFromMultiples( objs, objNum);
			/*强度暂时先不归0，因为这样的结果还不清楚*/
				#ifdef Nero_DeBuging09_01_14
				if (tmpObi)
				{
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = tmpObi;
				sprintf(neroObjMsgWithStr_st.str,"在DataFlowProcess中创建高级衍生对象成功");
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);								
				}

				#endif			
			
			
		}
		
		
		
	
	}
	if (objs)
	{
		free(objs);
	}
/*void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf*/	
	for (j=0;j<dataNum;j++)
	{
		if (DataFlow[j])
		{
			free(DataFlow[j]);
		}
	}
	if (dataKind)
	{
		free(dataKind);
	}
	if (DataFlow)
	{
		free(DataFlow);
	}
	return nero_msg_ok;

}








/*对数组中的概念进行增强连接（有序的）操作，如果没有连接的添加一个连接，*/
/*如果返回1 ,则表面所有连接的强度都已经达到最大值*/
nero_s32int Process_StrengthenLink(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf)
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
		return  Process_msg_CreateNewObj;
	}

	return   nero_msg_ok;
}



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
	forecastInfo_st.headOfUpperLayer.obj=NULL;
	INIT_LIST_HEAD(&(forecastInfo_st.headOfUpperLayer.p));

	forecastInfo_st.headOfSameLayer.obj=NULL;
	INIT_LIST_HEAD(&(forecastInfo_st.headOfSameLayer.p));
	
	forecastInfo_st.headOfLowerLayer.obj=NULL;
	INIT_LIST_HEAD(&(forecastInfo_st.headOfLowerLayer.p));
	
		
	/*struct NeroObjForecastList   *headOfUpperLayer;指向第一个预测对象	
	struct NeroObjForecastList   *headOfLowerLayer;指向第一个预测对象
	struct NeroObjForecastList   *headOfSameLayer;指向第一个预测对象*/	
	
	while(1)
	{
		/*死循环*/
		sleep(1);
		(neroConf.neroTime)++;
		flag++;
		if (flag==10)
		{
			printf("已经使用的nero数量:%d,剩余:%d\n",neroConf.UsedNeroNum,MaxNeroNum-neroConf.UsedNeroNum);
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
        if (forecastInfo  || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL)
        {
        
        
 		#ifdef Nero_ProcessERROR_Msg
/*		printf("找不到子概念\n");*/
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
        UpdataLastTimeINForecastList(forecastInfo);
       AddNewObjToForecastList( forecastInfo, newObj);
       CleanForecastList( forecastInfo);

}
void CleanForecastList(struct DataFlowForecastInfo  * forecastInfo)
{


}
void AddNewObjToForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * newObj)
{
 
        struct NeroObjForecastList   * findObiPoint;
        NerveFiber   * p; 
        NeuronObject * Obj;
	nero_s32int i,FiberType;
	NerveFiber * tmpFiber1;
	struct list_head  * listHead; 
        if (forecastInfo  || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL ||  newObj == NULL)
        {
        
        
 		#ifdef Nero_ProcessERROR_Msg
/*		printf("找不到子概念\n");*/
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"AddNewObjToForecastList参数错误");
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
		#endif
		return NULL; 
        }	
	
	

        p=newObj->outputListHead;
        while(p != NULL) 
        {
                Obj=p->obj;
                
                if (Obj != NULL  &&  nero_isBaseObj(Obj) != 1)
                {
                        FiberType=getFiberType(p);
                        /*加入列表*/
/* #define	Fiber_PointToUpperLayer	1*/
/*#define	Fiber_PointToLowerLayer	2*/
/*#define	Fiber_PointToSameLayer	3  */
                        
                        if (FiberType == Fiber_PointToUpperLayer)
                        {
                             listHead=(struct list_head  *)&(forecastInfo->headOfUpperLayer);
                        }
                        else if(FiberType == Fiber_PointToLowerLayer)
                        {
                                listHead=(struct list_head  *)&(forecastInfo->headOfSameLayer);
                        }
                        else
                                listHead=NULL;
                        
                        AddNodeIntoForecastList(listHead,Obj);
                }
                
                
                p=p->next;
        
        }
 
 
}
void AddNodeIntoForecastList(struct list_head  * listHead,NeuronObject * Obj)
{

        /*直接插入列表头部*/
        struct NeroObjForecastList *newListNode=(struct NeroObjForecastList *)malloc(sizeof(struct NeroObjForecastList));
        newListNode->obj=Obj;
        newListNode->Strengthen=0;
        newListNode->times=0;
        
        list_add((struct list_head  *)newListNode, listHead);

}
/*更新列表预测列表中每个结构的time变量*/
void  UpdataLastTimeINForecastList(struct DataFlowForecastInfo  * forecastInfo)
{
        struct NeroObjForecastList   * findObiPoint;
        struct list_head  * p; 
        
        p=(struct list_head  *)(forecastInfo->headOfUpperLayer.p.next);
        while(p != &(forecastInfo->headOfUpperLayer) 
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                (findObiPoint->times)++;
                p=p->next;
        
        }
        p=(struct list_head  *)forecastInfo->headOfLowerLayer.p.next;
        while(p != &(forecastInfo->headOfLowerLayer)) 
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                (findObiPoint->times)++;
                p=p->next;
        
        }
        p=(struct list_head  *)forecastInfo->headOfSameLayer.p.next;
        while(p != &(forecastInfo->headOfSameLayer)) 
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                (findObiPoint->times)++;
                p=p->next;
        
        }
}

/*判断DataFlowForecastInfo中 objPoint之前的数据是否能够有子集形成衍生类 */
NeuronObject * Process_IfFindDerivativeObj(struct DataFlowForecastInfo  * forecastInfo)
{
        nero_s32int i;
        NeuronObject * findobj=NULL;
        if (forecastInfo  || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL)
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
        for (i=forecastInfo->objPoint-2;i>=0;i--)
        {
                findobj=nero_IfHasObjFromMultiples4(&((forecastInfo->objs)[i]),(forecastInfo->objPoint-1-i+1));
                if (findobj != NULL)
                {
                        forecastInfo->start=i;
                        forecastInfo->end=forecastInfo->objPoint-i;
                        break;
                }
        }

        return findobj;

}
/*看看预测列表中能不能找到findObi*/
struct NeroObjForecastList   * Process_CompareWithForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * findObi)
{
        struct NeroObjForecastList   *head;
        struct NeroObjForecastList   *findObiPoint;        
        if (forecastInfo  || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL  || findObi==NULL )
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
       
       findObiPoint=  FindObjInForecastList(&(forecastInfo->headOfUpperLayer), findObi);
       if (findObiPoint == NULL )
       {
            findObiPoint=  FindObjInForecastList(&(forecastInfo->headOfSameLayer), findObi);   
       }
       
        return findObiPoint;

}
struct NeroObjForecastList   *  FindObjInForecastList(struct NeroObjForecastList   *head,NeuronObject * findObi)
{
        struct NeroObjForecastList   * findObiPoint;
        struct list_head  * p; 
        
        p=(struct list_head  *)head;
        findObiPoint=NULL;
        while(p != NULL) 
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

        if (forecastInfo  || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL)
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
        
        if (forecastInfo->objPoint  ==  (forecastInfo->objNum))
        {
                return NULL; 
        }
        forecastInfo->objPoint=forecastInfo->objPoint +1;
        return ( forecastInfo->objs)[forecastInfo->objPoint  -1]; 

}










