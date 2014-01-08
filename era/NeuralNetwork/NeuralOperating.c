
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "NeuralNetwork.h"
#include "NeuralOperating.h"
#include "../tools/readUTF8File.h"

/*#include "../common/error.h"*/


void *thread_for_Operating_Pic(void *arg)
{
	int x=0;
	int timeToWaitCandy=0;
	char *fileName=NULL;
	long MsgId;
	struct { long MsgId; char text[100]; } OperatingMsg;
	struct ZhCharArg * arg1;
	struct DataFlowProcessArg * arg2;
	key_t ipckey;
	int Operating_mq_id;

	int received;

	/* Generate the ipc key */
/*	Operating_ipckey="/home/ub/shareSpace/Operating_ipckey";*/
	
	 
/*	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
	#define IPCKEY 0x111
	ipckey = ftok(Operating_ipckey, IPCKEY);
/*	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
/*	printf("Operating_ipckey key is %d\n", ipckey);*/
/*	*/
	/* Set up the message queue */
	Operating_mq_id = msgget(ipckey,IPC_CREAT);// IPC_CREAT
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
	printf("Operating_ipckey Message identifier is %d\n", Operating_mq_id);
	while(x == 0)
	{
		sleep(1);
/*		printf("wait for Operating msg......\n");*/
		received = msgrcv(Operating_mq_id, &OperatingMsg, sizeof(OperatingMsg), 0, MSG_NOERROR);
		printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
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
			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_CreateNetNet:\n");
			#endif
			break;
		case MsgId_Nero_addZhCharIntoNet:
			arg1=(struct ZhCharArg *)OperatingMsg.text;
			nero_addZhCharIntoNet( GodNero,arg1->chChar, arg1->charCounts);
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_addZhCharIntoNet:\n");
			#endif			
			
			
			break;	
		case MsgId_Nero_DataFlowProcess :
			arg2=(struct DataFlowProcessArg *)OperatingMsg.text;
			DataFlowProcess(arg2->DataFlow,arg2->dataKind,arg2->dataNum,  GodNero, arg2->conf);
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_DataFlowProcess:\n");
			#endif			
			
			/*show  neroNet*/
			#ifdef  Nero_DeBuging03_12_13
			 createNeroNetDotGraphForWords(GodNero, "data/wordspic.dot");
			printf("createNeroNetDotGraph   done.\n");	
			#endif				

			#ifdef  Nero_DeBuging03_12_13
			system("xdot data/wordspic.dot");
			#endif
				
			break;				
			
	
		default:			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_NONE:  \n");
			 printf("mymsg=%s (%d)\n", OperatingMsg.text, received);	
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
	nero_s32int i,j,hasAddObj/*,hasNewObj*/,res1;
	NeuronObject * tmpObi;
	NeuronObject ** objs=NULL;
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
		tmpObi =nero_IfHasNeuronObject(DataFlow[i],dataKind[i], GodNero);
		
		#ifdef Nero_DeBuging21_12_13_
		if (tmpObi == NULL  )
		{
			printf("找不到子概念\n");
		}
		else 
		{
			printf("找到子概念\n");
		}			
		#endif
		/*如果不存在则尝试将该对象加入网络*/
		if (tmpObi == NULL  && conf->addNewObj == 1)
		{
			tmpObi=  nero_addNeroByData(DataFlow[i],dataKind[i]);
			#ifdef Nero_DeBuging21_12_13_
			if (tmpObi != NULL  )
			{
				printf("添加子概念成功\n\n");
				nero_printNeroLink("log/ObjLink.log",(void *)tmpObi);
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
	/*将这几个对象形成层次结构*/
	/*其实就是将这几个对象形成一个新的对象，见神经网络记录 sheet   5系统概略图*/
	/*
		你要做两件事情，1:看看子概念的子集有没有组成概念----------------这个是非常麻烦的，
					也是整个系统中最关键的机制，比如你输入一个句子
					怎么在大量句子中生成新词呢
				2:整个数组形成新概念
	
	
	
	
	*/

	if (conf->addLevelObjAlways == 1   ||  (hasAddObj !=1 && conf->addLevelObj == 1))
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
		
		nero_createObjFromMultiples( objs, j);
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
		
		res1=Process_StrengthenLink(objs,j,GodNero, conf);
		
		
		/*如果发现强度足够高时则生成新概念*/
		/*如果子概念分别为a b c,而 b c  已经组成了概念，那么这个由a b c  组成的概念和b c
		组成的概念是什么关系呢
		*/
		
		/*一旦形成新的概念，就需要对相应的连接的连接强度做一些修改，怎么样的修改呢？？*/
		if (res1  ==  Process_msg_CreateNewObj)
		{
			/*首先创建一个新概念，然后把这些子概念之间的链接强度归零*/
			nero_createObjFromMultiples( objs, j);
			/*强度暂时先不归0，因为这样的结果还不清楚*/
		}
		
		
		
	
	}
	if (objs)
	{
		free(objs);
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
	for (i=0;i<objNum;i++)
	{
	
	
		for (j=i+1;j<objNum;j++)
		{

			Strengthen= nero_StrengthenLink(objs[i],objs[j]);
			if (Strengthen != Fiber_StrengthenMax)
			{
				flag=0;
			}
			
		}
		
		
	}
	if (flag  ==  1)
	{
		return  Process_msg_CreateNewObj;
	}

	return   nero_msg_ok;
}






















