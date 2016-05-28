
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
/*        printf("i=%d.\n",tmpConf->addLevelObjAlways);*/
        switch(kind)
        {
        
         case Conf_Modify_addLevelObjAlways:
                neroConf.addLevelObjAlways=tmpConf->addLevelObjAlways;
                 printConfChangMsg(Conf_Modify_addLevelObjAlways);
                
                break;
         case Conf_Modify_CreateNewBaseObjKind:
                neroConf.CreateNewBaseObjKind=tmpConf->CreateNewBaseObjKind;
                   printConfChangMsg(Conf_Modify_CreateNewBaseObjKind);
              
                break;       
           case Conf_Modify_ReSet:
					resetNeroConf();
                   printConfChangMsg(Conf_Modify_ReSet);
              
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
	printf("Operating_ipckey key is %d\n", ipckey);
/*	*/
	/* Set up the message queue */
	Operating_mq_id = msgget(ipckey,0);// IPC_CREAT
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
/*		printf("wait for Operating msg......\n");*/
		received = msgrcv(Operating_mq_id, &OperatingMsg, sizeof(OperatingMsg), 0, MSG_NOERROR);
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
			#ifdef Nero_DeBugInOperating_Pic
			 printf("received  ok:\n");
			#endif
		MsgId=OperatingMsg.MsgId;
		
		switch(MsgId)
		{
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
			 printf("MsgId_Nero_CreateNetNet:\n");
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
				printf("系统未初始化\n");			
			
			}			
			
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
	nero_s32int i,j,hasAddObj/*,hasNewObj*/,res1,objNum,OldobjNum,res2;
	NeuronObject * tmpObi,* tmpBaseObi;
	NeuronObject ** objs=NULL;
/*        struct NeroObjForecastList   *findObiPoint; */
/*        NeuronObject * findForecastObj;	*/
/*        struct NeroObjForecastList   *findForecastObjPoint; 	*/

	static nero_us32int coutOferror_Msg_=0;
	coutOferror_Msg_=coutOferror_Msg_+1;
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

		#ifdef Nero_DeBuging21_12_13_
		if (tmpObi == NULL  )
		{
				
				#ifdef Nero_DeBuging09_01_14
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
			printf("找到子概念\n");
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
				#ifdef Nero_DeBuging09_01_14
				//~ printf("添加子概念成功\n\n");
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
			        #ifdef Nero_DeBuging09_01_14_
			        printf("DataFlow[i]=%s.\n",DataFlow[i]);
/*				printf("添加子概念失败,dataKind=%d\n\n",dataKind[i]);*/
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
/*			printf("objs[j]=%x.tmpObi=%x\n",objs[j],tmpObi);*/
			j++;
		}
		
		
		
		
	}
	
/*	return 0;*/


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
                printf("\n");
        }
       
        #endif
	forecastInfo_st.objs=objs;
	forecastInfo_st.objNum=j;
	forecastInfo_st.objPoint=0;
	forecastInfo_st.DeBugMsg=coutOferror_Msg_;
/*	forecastInfo_st.head=NULL;//在thread_for_Sys_Pic(void *arg)中被初始化*/
		
	
	/***************************************************************/
	if (  Nero_TestCount >= coutOferror_Msg_ )
	{
	        Process_ObjForecast(&forecastInfo_st);
	}
        /***************************************************************/
/*        printf("coutOferror=%d.\n",coutOferror_Msg_);*/
        
	#ifdef DataFlowProcess_error_Msg_
	
	if (coutOferror_Msg_ <= Nero_TestCount )
	{
               
                printf("DataFlowForecastInfo:\n    objNum=%d\n    objPoint=%d ",forecastInfo_st.objNum,forecastInfo_st.objPoint);
                printf("\n    UpperLayerobjNum=%d\n    SameLayerobjNum=%d\n    LowerLayerobjNum=%d\n    dataNum=%d\n\n ",forecastInfo_st.headOfUpperLayer.times,forecastInfo_st.headOfSameLayer.times,forecastInfo_st.headOfLowerLayer.times,dataNum);
        }

        tmpc=0;
        #ifdef DataFlowProcess_error_Msg
        if (coutOferror_Msg_ <= Nero_TestCount )
        {
                
               for (;tmpc<forecastInfo_st.objNum;tmpc++)
               {
  				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId = 1;
				neroObjMsgWithStr_st.Obi = objs[tmpc];
				sprintf(neroObjMsgWithStr_st.str,"识别结束后打印识别对象obj =%x id=%d",objs[tmpc],coutOferror_Msg_);
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);	                     
               } 
        }
        printf("\n");
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
	 /*这个开关打开的时间说明可以进行新基类（抽象概念）创建了*/  
/*	 printf("1 CreateNewBaseObjKind=%d.  add=%x\n",conf->CreateNewBaseObjKind,conf);*/
        if (conf->CreateNewBaseObjKind == 1 )
        {
             
              #ifdef Nero_DeBuging06_02_14_
              int tmpi;
              for (tmpi=0;tmpi<objNum;tmpi++)
              {
                      printf("list  obj%c  kind=%d.\n",tmpi,nero_GetNeroKind(objs[tmpi]));
              }
              #endif
              printf("\n");
              /*判断是否可以进行新基类（抽象概念）创建*/
              res2=Process_IfCreateNewBaseObj(objs,objNum,GodNero,conf);
              /*开始添加基类 */
              if (res2 == NeroYES)
              {
                      tmpBaseObi=nero_CreateNewBaseObj(objs,objNum,GodNero, conf);
              }
              else
              {
                        /*如果不需要添加新的基类，那就是修改基类了*/
              
                        nero_ModifyBaseKind(objs,objNum,GodNero,conf);
              }
              
              /*很显然对于那些特殊高层衍生概念的创建createObjFromMultiples会出现问题
              事实上：队伍数字这个抽象概念来说，如果输入了一个字符1，这个这个字符很可能在
              Process_ObjForecast(&forecastInfo_st);已经被替换为数字类型的概念，也可能
              不被替换，显然这需要上下文来判断
              */
           
              /*这里大概需要保证如果objNum-1=1时这个对象必须是不存在的，如果存在的话
              在Process_ObjForecast(&forecastInfo_st);已经被替换为数字类型的概念
              */
              /*这时候你发现，一个概念出现了同时指向俩个基类的情况了*/
              // printf("nero_createObjFromMultiples  1111   \n");
              tmpObi =nero_createObjFromMultiples( &(objs[1]), objNum-1);
              
                // #ifdef Nero_DeBuging09_01_14
                printf("开始u创建了新类 objNum=%d\n",objNum); 
                if (tmpObi)
                {
                       printf("创建了新类 %x---kind=%d.objNum=%d\n\n",tmpObi,nero_GetNeroKind(tmpObi),objNum); 
                }
                
                // #endif
                

              /*立马关闭开关，以免影响下次操作*/
              conf->CreateNewBaseObjKind = 0;
        }
        else if (conf->addLevelObjAlways == 1   )
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
				// printf("nero_createObjFromMultiples  2\n");
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
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_  before  StrengthenLink=%d.\n",coutOferror_Msg_);
	#endif		
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
			// printf("nero_createObjFromMultiples  3\n");
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
	printf("coutOferror_Msg_   1:%d.\n",coutOferror_Msg_);
	#endif	
	if (objs)
	{
		free(objs);
	}
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   2:%d.\n",coutOferror_Msg_);
	#endif	
	for (j=0;j<dataNum;j++)
	{
		if (DataFlow[j])
		{
			free(DataFlow[j]);
		}
	}
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   3:%d.\n",coutOferror_Msg_);
	#endif	
	if (dataKind)
	{
		free(dataKind);
	}
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   4:%d.\n",coutOferror_Msg_);
	#endif	
	if (DataFlow)
	{
		free(DataFlow);
	}
	
	#ifdef DataFlowProcess_error_Msg_
	printf("coutOferror_Msg_   5:%d.\n",coutOferror_Msg_);
	#endif	
	return nero_msg_ok;

}
/*判断是否需要创建新基类*/
nero_s32int  Process_IfCreateNewBaseObj(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf)
{
        NeuronObject * tmp;
        nero_s32int i,mark,objKind,baseKind,baseDataKind;
        NeuronObject *Obi,*tmpObi;
	NerveFiber *curFiber;

	/*参数检查*/
	if (objs == NULL  || godNero ==NULL  ||  objNum <2 || conf ==NULL)
	{
		return nero_msg_ParameterError;
	}

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
	        if (baseDataKind >=NeuronNode_MinNewDerivativeClassId)
	        {
	                /*判断基类名和objs[0]是否相同*/
/*	                printf("IfCreateNewBaseObj:  baseDataKind=%d.\n",baseDataKind);*/
/*	                printf("getBaseObjName= %x  objs[0]=%x.\n",getBaseObjName(Obi,godNero),objs[0]);*/
	                if (getBaseObjName(Obi,godNero) ==  objs[0])
	                {
/*	                         printf("找到了相同名的基类\n");*/
	                         return NeroNO;
	                }
	                
	        }
	
	
	}
        
        baseKind=nero_judgeNewObjKind(&(objs[1]),objNum-1);
/*        printf("IfCreateNewBaseObj :baseKind=%d.\n",baseKind);*/
        /*如果baseKind =NeuronNode_ForComplexDerivative，
        说明找不到合适的类来
        匹配数据，需要创建新类
        */
        if (NeuronNode_ForComplexDerivative == baseKind    )
        {
                return NeroYES;
        }
        
        
        
         return NeroNO;
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
/*	        printf("time to createNewObj\n");*/
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
       AddNewObjToForecastList( forecastInfo, newObj);
       
/*        #define  CleanForecastList_msgTest*/
        #ifdef CleanForecastList_msgTest
        static nero_us32int count_=0;
        count_++;
        printf("count_=%d.\n",count_);
        #endif	  
        
        CleanForecastList( forecastInfo);
        
        
         #ifdef CleanForecastList_msgTest
        printf("count_=%d.-------------\n",count_);
        #endif	         
}
void CleanForecastList(struct DataFlowForecastInfo  * forecastInfo)
{
        struct NeroObjForecastList   * findObiPoint;
        struct list_head  * p; 
        struct list_head  * head; 
/*        struct NeroObjForecastList * tmplistNode;*/
/*        #define Debug_CleanForecastList*/
/*        #define Debug_CleanForecastList2*/
         #ifdef Debug_CleanForecastList
        printf("headOfUpperLayer-----------\n");
	#endif	    
	
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
                if (findObiPoint->times  > NeroMaxLastTimeINForecastList)
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
                                printf("预测链表缩减。。。。。。。\n");
		                #endif	  
		                continue;                             
                        }  
                }

                p=p->next;
        
        }
         #ifdef Debug_CleanForecastList
        printf("headOfLowerLayer-----------\n");
	#endif	
        p=(struct list_head  *)forecastInfo->headOfLowerLayer.p.next;
         head=(struct list_head  *)&(forecastInfo->headOfLowerLayer.p);
        while(p != head) 
        {
                findObiPoint=(struct NeroObjForecastList   *) p;
                if (findObiPoint->times  > NeroMaxLastTimeINForecastList)
                {
                        
                         /*移除这个节点*/
                        __list_del_entry((struct list_head *)p);
                        if (findObiPoint)
                        {
                                 p=p->next;
                                 #ifdef Debug_CleanForecastList2
                                printf("headOfLowerLayer-----------\n");
	                        #endif	
                                free(findObiPoint);
                                (forecastInfo->headOfLowerLayer.times)--;
                                 #ifdef Debug_CleanForecastList2
                                printf("预测链表缩减。。。。。。。\n");
		                #endif	  
		                continue;                             
                        }                       
                        
                        
                }
                p=p->next;
        
        }

         

	
         #ifdef Debug_CleanForecastList
        printf("headOfSameLayer-----------\n");
	#endif	
        p=(struct list_head  *)forecastInfo->headOfSameLayer.p.next;
         head=(struct list_head  *)&(forecastInfo->headOfSameLayer.p);
/*          i=0;*/
        while(p != head) 
        {
 
                findObiPoint=(struct NeroObjForecastList   *) p;
	#ifdef Nero_DeBuging24_01_14_
        
        printf(" counts=%d  p=%x,prev=%x   next=%x\n",++i,p,p->prev,p->next);
        #endif 
                if (findObiPoint->times  > NeroMaxLastTimeINForecastList)
                {
                        /*移除这个节点*/
                        __list_del_entry((struct list_head *)p);
                        if (findObiPoint)
                        {
                                 p=p->next;
                                 #ifdef Debug_CleanForecastList2
                                printf("headOfSameLayer-----------\n");
	                        #endif
                                free(findObiPoint);
                                (forecastInfo->headOfSameLayer.times)--;
                                 #ifdef Debug_CleanForecastList2
                                printf("预测链表缩减。。。。。。。\n");
		                #endif	  
		                continue;                             
                        }
                }
  	#ifdef Nero_DeBuging24_01_14_
        
        printf(" counts=%d  p=%x,prev=%x   next=%x\n",i,p,p->prev,p->next);
        #endif               
                p=p->next;
        
        }        

}
void AddNewObjToList(struct DataFlowForecastInfo  * forecastInfo,nero_s32int FiberType,NeuronObject * Obj)
{

        struct list_head  * listHead;                
                        /*加入列表*/

/*                        printf("                FiberType=%d.\n",FiberType);*/
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
                        if (listHead !=NULL)
                        {
                         
                                /*首先判断是否已经有这个概念*/
                                 struct NeroObjForecastList   *  findNodeINlist;
                                 findNodeINlist=Process_IfHasThisObjINList( listHead,Obj);
                                
                                if (findNodeINlist == NULL)
                                {
                  		        #ifdef Nero_DeBuging24_01_14_
                                        printf("预测链表增长。。。。p->next=%x。。。\n",p->next);
				        neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				        neroObjMsgWithStr_st.fucId = 1;
				        neroObjMsgWithStr_st.Obi = Obj;
				        sprintf(neroObjMsgWithStr_st.str,"预测链表增长 id=%d id2=%d",forecastInfo->DeBugMsg,forecastInfo->DeBugMsgTwo);
				        msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);								
				        #endif                                         
                                         AddNodeIntoForecastList(listHead,Obj);
                                }
                                else
                                {
                                        /*对于刚被查询的概念，就是新输入的，但是列表中
                                        已经有的概念延长起存在时间，已避免反复添加删除
                                        */
                                        
                                        (findNodeINlist->times)--;
/*                                        printf("findNodeINlist->times=%d.\n",findNodeINlist->times);*/
                                        
                                }

                        }

}
void AddNewObjToForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * newObj)
{
 
        struct NeroObjForecastList   * findObiPoint;
        NerveFiber   * p; 
        NeuronObject * Obj;
	nero_s32int i,FiberType;
	NerveFiber * tmpFiber1;
	
        if (forecastInfo== NULL   || forecastInfo->objPoint > forecastInfo->objNum  ||  forecastInfo->objs == NULL ||  newObj == NULL)
        {
        
        
 		#ifdef Nero_ProcessERROR_Msg
		printf("AddNewObjToForecastList  ProcessERROR\n");
		neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
		neroObjMsgWithStr_st.fucId = 1;
		neroObjMsgWithStr_st.Obi = NULL;
		sprintf(neroObjMsgWithStr_st.str,"AddNewObjToForecastList参数错误");
		msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
		#endif
		return ; 
        }	
 	#ifdef Nero_ProcessERROR_Msg
	neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
	neroObjMsgWithStr_st.fucId = 1;
	neroObjMsgWithStr_st.Obi = newObj;
	sprintf(neroObjMsgWithStr_st.str,"AddNewObjToForecastList  newObj");
	msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
	#endif	
	/*控制替换的深度，这里只替换一层*/
	if (nero_GetNeroKind(newObj) < NeuronNode_MinNewDerivativeClassId )
	{
	        
	
	
	
	
	
                p=newObj->outputListHead;
        /*        printf("                p=%x,newObj=%x.\n",p,newObj);*/

                while(p != NULL) 
                {
        /*                printf("                p=%x,Obj=%x.\n", p,p->obj);*/
                        Obj=p->obj;
                        FiberType=getFiberType(p);
                        if (Obj != NULL  &&  nero_isBaseObj(Obj) != 1)
                        {
                                AddNewObjToList( forecastInfo,FiberType,Obj);
                                
                        }
                        
                        
                        p=p->next;
                
                }
        }
        /*判断是不是要把newObj本身也加入预测列表：*/
        if (forecastInfo->waitForRecognise  !=NULL)
        {
                
                
                AddNewObjToList( forecastInfo,Fiber_PointToSameLayer,forecastInfo->waitForRecognise);
                forecastInfo->waitForRecognise=NULL;
        }
 
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
void AddNodeIntoForecastList(struct list_head  * listHead,NeuronObject * Obj)
{

        /*直接插入列表头部*/
        struct NeroObjForecastList *newListNode=(struct NeroObjForecastList *)malloc(sizeof(struct NeroObjForecastList));
        newListNode->obj=Obj;
        newListNode->Strengthen=0;
        newListNode->times=0;
        newListNode->start=-1;
        newListNode->end=-1;        
        
        
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
        newListNode=(struct NeroObjForecastList *)listHead;
        (newListNode->times)++;

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
        
        if (forecastInfo->objPoint  ==  (forecastInfo->objNum))
        {
                return NULL; 
        }
        forecastInfo->objPoint=forecastInfo->objPoint +1;
/*        printf("objPoint=%d,objs=%x\n",forecastInfo->objPoint-1,forecastInfo->objs);*/
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
                if (getFiberType(curFiber) == Fiber_PointToUpperLayer  &&  nero_isBaseObj(tmpobj) != 1)
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

	 
	while( (tmpObi=Process_IfHasNextObjToread(forecastInfo))  !=   NULL)
	{
	 ObjForecast_DeBug_Count++;
	 forecastInfo->DeBugMsgTwo=ObjForecast_DeBug_Count;
	 
	        /*与预测链表进行比较，看能不能找到tmpObi
		
		forecastInfo_st.activateForecastObj=NULL;
		
		*/	
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
		              /*修改过后就重新开始  循环*/
		                forecastInfo->activateForecastObj->start=-1;
		                forecastInfo->activateForecastObj->end=-1;
		                forecastInfo->activateForecastObj=NULL;
		                forecastInfo->objPoint=0;
		                forecastInfo->timeToMerage=0;
		                continue;          
		      }
		        
		}
		else
		{
		        forecastInfo->waitForRecognise=tmpObi;
		        /*让预测列表更新后（加入tmpObi）重新识别一遍tmpObi*/
		        forecastInfo->waitForRecogniseObjPos=forecastInfo->objPoint-1;
		        forecastInfo->objPoint=forecastInfo->waitForRecogniseObjPos;
		
		}	
		#ifdef ObjForecast_DeBug_msg
		printf("before Updata tmpObi=%x.\n",tmpObi);
                printf("objNum=%d objPoint=%d \n\n",forecastInfo->objNum,forecastInfo->objPoint);
                #endif     
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
	nero_s32int i,j,/*hasAddObj,hasNewObj,res1,*/objNum,OldobjNum/*,res2*/;
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









