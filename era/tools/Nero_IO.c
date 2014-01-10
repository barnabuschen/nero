#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include<time.h>
#include "../NeuralNetwork/NeuralNetwork.h"
#include "../NeuralNetwork/NeuralOperating.h"
#include "../tools/readUTF8File.h"
#include "Nero_IO.h"
/*#include "../common/error.h"*/
#include "../common/fileOperating.h"


/*用来打印对象信息的函数族*/
BEGIN_ONE_ARG_MESSAGE_MAP(nero_msg_print_map)
    MSG_NAME(1, Log_printNeroObjMsg)
    MSG_NAME(2, Log_printNeroObjLink)
/*    MSG_NAME(3, Log_printAllKindOf)*/
/*    MSG_NAME(4, sort_data)*/
END_ONE_ARG_MESSAGE_MAP

BEGIN_TWO_ARG_MESSAGE_MAP(nero_msg_WithStr_map)
    MSG_NAME(1, Log_printSomeMsgForObj)
    MSG_NAME(2, Log_printAllKindOf)
/*    MSG_NAME(3, del_data)*/
/*    MSG_NAME(4, sort_data)*/
END_TWO_ARG_MESSAGE_MAP






time_t   now;         //实例化time_t结构
struct  tm  *timenow;         //实例化tm结构指针

/*nero_8int  logFile[500]="../log/log.txt";*/
nero_8int  logFile[FILEPATH_MAX]="/tmp/log.txt";
nero_8int  AllKindOfFile[FILEPATH_MAX]="/tmp/AllKindOfFile.txt";
nero_8int  file_path_getcwd[FILEPATH_MAX]="/tmp";/*保存当前目录*/
nero_8int  strTmp[1500];

/*打印某个类别下面的所有的衍生类*/
nero_s32int Log_printAllKindOf(void * obj_,void *str_)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind,ObjectKind2;
	nero_8int  strLinshi[500];
	NeuronObject * obj;
	NeuronObject * tmp;
	NeuronObject * BaseObi;
	NerveFiber  *  curFiber;
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/


		ObjectKind=*((nero_s32int *)(str_));
		curFiber=GodNero->outputListHead;
		for (;curFiber !=NULL;curFiber=curFiber->next)
		{
			BaseObi=curFiber->obj;
			ObjectKind2=nero_GetNeroKind(BaseObi);
			if (ObjectKind2 == ObjectKind)
			{
				break;
			}
		}		
		printf("Log_printAllKindOf  start\n");	
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
/*			sprintf(str,"Log_printAllKindOf:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),obj,ObjectKind);*/
/*			addLineToFile(logFile,str);	*/
			break;	
		case NeuronNode_ForChCharacter:
/*		case NeuronNode_ForChWord :*/
			curFiber=BaseObi->outputListHead;
			sprintf(str,"Log_printAllKindOf:%s		ObjectKind=%d的所有对象为：\n",asctime(timenow),ObjectKind);
			addLineToFile(AllKindOfFile,str);
			while(curFiber)
			{
				tmp=curFiber->obj;

					ObjectKind2=nero_GetNeroKind(tmp);
					switch(ObjectKind2)
					{
						case NeuronNode_ForChCharacter:
							IO_getZhInNero(str,tmp);
							break;
						case NeuronNode_ForChWord:
							IO_getWordsInNero(str,tmp);
							break;
						default:
					
					
							break;
					
					}
					sprintf(strLinshi,"		地址 %x  <%s>\n",tmp,str);
					addLineToFile(AllKindOfFile,strLinshi);

				curFiber=curFiber->next;
			}
				
			break;
		default:break;

		
		}

		
	printf("Log_printAllKindOf  done\n");
	return nero_msg_ok;
}


nero_s32int Log_printNeroObjLink(void * arg)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind,ObjectKind2;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	NerveFiber  *  curFiber;
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (obj)
	{
		ObjectKind=nero_GetNeroKind(obj);
			
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
			sprintf(str,"Log_printNeroObjLink:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),obj,ObjectKind);
			addLineToFile(logFile,str);	
			break;	
		case NeuronNode_ForChCharacter:
		case NeuronNode_ForChWord :
			curFiber=obj->outputListHead;
			sprintf(str,"Log_printNeroObjLink:%s		地址：%x的link对象,ObjectKind=%d\n",asctime(timenow),obj,ObjectKind);
			addLineToFile(logFile,str);
			while(curFiber)
			{
				tmp=curFiber->obj;
				
				
				if (nero_isBaseObj(tmp ) !=1)
				{
					
				
					ObjectKind2=nero_GetNeroKind(tmp);
					switch(ObjectKind2)
					{
						case NeuronNode_ForChCharacter:
							IO_getZhInNero(str,tmp);
							break;
						case NeuronNode_ForChWord:
							IO_getWordsInNero(str,tmp);
							break;
						default:
					
					
							break;
					
					}
					sprintf(strLinshi,"		-->%x  <%s>\n",tmp,str);
					addLineToFile(logFile,strLinshi);
				}	
				else
				{
					sprintf(strLinshi,"		该对象为基类，无需link\n");
					addLineToFile(logFile,strLinshi);
				
				}		
				curFiber=curFiber->next;
			}
				
			break;
		default:break;

		
		}

			
	}
	else
	{
		sprintf(str,"Log_printNeroObjLink:%s		空对象\n",asctime(timenow));	
		addLineToFile(logFile,str);	
	}

	return nero_msg_ok;
}





nero_s32int Log_printSomeMsgForObj(void * obj_,void *str_)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)obj_;
	NeuronObject * tmp;
	
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (obj &&  (nero_isBaseObj(obj ) !=1))
	{
		ObjectKind=nero_GetNeroKind(obj);
		
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
			if (strlen(str_) <400)
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,ObjectKind=%d,%s\n",asctime(timenow),obj,ObjectKind,str_);	
			else
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,ObjectKind=%d,非法的打印信息\n",asctime(timenow),obj,ObjectKind);	
			break;	
		case NeuronNode_ForChCharacter:
			tmp=obj->inputListHead->obj;/*衍生对象的第一个数据*/
			if (strlen(str_) <400)
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印字符对象《%c%c%c》,%s\n",asctime(timenow),obj,tmp->x,tmp->y,tmp->z,str_);	
			else
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印字符对象《%c%c%c》,非法的打印信息\n",asctime(timenow),obj,tmp->x,tmp->y,tmp->z);	
			break;
		case NeuronNode_ForChWord :
			IO_getWordsInNero(strLinshi,obj);
			if (strlen(str_) <400)
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印词组对象《%s》,%s\n",asctime(timenow),obj,strLinshi,str_);	
			else
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印词组对象《%s》,非法的打印信息\n",asctime(timenow),obj,strLinshi);	
			break;
		default:break;

		
		}

			
	}
	else
		sprintf(str,"Log_printSomeMsgForObj:%s		空对象或者为基类对象,%s\n",asctime(timenow),str_);	

	addLineToFile(logFile,str);
	return nero_msg_ok;
}

nero_s32int Log_printNeroObjMsg(void * arg)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (obj)
	{
		ObjectKind=nero_GetNeroKind(obj);
			
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),obj,ObjectKind);	
			break;	
		case NeuronNode_ForChCharacter:
			tmp=obj->inputListHead->obj;/*衍生对象的第一个数据*/
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,打印字符对象《%c%c%c》\n",asctime(timenow),obj,tmp->x,tmp->y,tmp->z);	
			break;
		case NeuronNode_ForChWord :
			IO_getWordsInNero(strLinshi,obj);
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,打印词组对象《%s》\n",asctime(timenow),obj,strLinshi);	
			break;
		default:break;

		
		}

			
	}
	else
		sprintf(str,"Log_printNeroObjMsg:%s		空对象\n",asctime(timenow));	

	addLineToFile(logFile,str);
	return nero_msg_ok;
}


void *thread_for_IO_Pic(void *arg)
{
	int x=0;
	long MsgId;
	struct { long MsgId; char text[100]; } IOMsg;

	key_t ipckey;
	int IO_mq_id;

	int received;

	/* Generate the ipc key */

	ipckey = ftok(IO_ipckey , IPCKEY);
/*	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
/*	printf("Operating_ipckey key is %d\n", ipckey);*/
/*	*/
	/* Set up the message queue */
	IO_mq_id = msgget(ipckey,0);// IPC_CREAT
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
	printf("IO_ipckey Message identifier is %d\n", IO_mq_id);
	while(x == 0)
	{
/*		sleep(1);*/
		printf("wait for IO msg......\n");
		received = msgrcv(IO_mq_id, &IOMsg, sizeof(IOMsg), 0, MSG_NOERROR);
		if (errno != 0)
		printf("msgrcv strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
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
		MsgId=IOMsg.MsgId;
		
		switch(MsgId)
		{
		case MsgId_Nero_CreateNetNet:

			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_CreateNetNet:\n");
			#endif
			break;
			
			
	
		default:			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_NONE:  \n");
			 printf("IO msg=%s (%d)\n", IOMsg.text, received);	
			#endif	
			break;
		}
		

	}
	printf("end to wait IO msg................................\n");
}


void *thread_for_Log_Pic(void *arg)
{
	nero_s32int x=0;
	long MsgId;
	struct { long MsgId; char text[100]; } LogMsg;
	struct  NeuronObjectMsg_  * NeroArgMsg_st;
	struct  NeuronObjectMsgWithStr_  * NeroWithStrArgMsg_st;
	key_t ipckey;
	nero_s32int Log_mq_id;
	nero_s32int fucId;
	nero_s32int received;
	nero_s32int i = 0;

	const nero_s32int size_message_map = 
	      sizeof(nero_msg_print_map) / sizeof(struct one_arg_message_entry );   //求得表长
	const nero_s32int size_message_map2 = 
	      sizeof(nero_msg_WithStr_map) / sizeof(struct two_arg_message_entry );   //求得表长
	

	 
	 getcwd(file_path_getcwd,FILEPATH_MAX);
	 /*设置日志目录*/
	 sprintf(logFile,"%s/log/log.txt",file_path_getcwd);
/*	 printf("file_path_getcwd: %s  and logFile=%s",file_path_getcwd,logFile);*/
	 createFile(logFile);
	 
	 sprintf(AllKindOfFile,"%s/log/AllKindOfFile.txt",file_path_getcwd);
	 createFile(AllKindOfFile);
	

	/* Generate the ipc key */

	ipckey = ftok(Log_ipckey , IPCKEY);
/*	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
/*	printf("Operating_ipckey key is %d\n", ipckey);*/
/*	*/
	/* Set up the message queue */
	Log_mq_id = msgget(ipckey,0);// IPC_CREAT
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
	printf("Log_ipckey Message identifier is %d\n", Log_mq_id);
	while(x == 0)
	{
/*		sleep(1);*/
		printf("wait for Log msg......\n");
		received = msgrcv(Log_mq_id, &LogMsg, sizeof(LogMsg), 0, MSG_NOERROR);
		if (errno != 0)
		{
			printf("msgrcv strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息			
		}

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
		MsgId=LogMsg.MsgId;

		switch(MsgId)
		{
		case MsgId_Log_PrintObjMsg:
/*			printf("MsgId_Log_PrintObjMsg \n");*/
/*			fucId=(nero_s32int)  (LogMsg.text[0])*/
			NeroArgMsg_st=(struct  NeuronObjectMsg_  * )&LogMsg;
			
			for( i = 0; i < size_message_map; i++)
			{
			    if( nero_msg_print_map[i].id == NeroArgMsg_st->fucId )
				 (*(nero_msg_print_map[i].operate) )(NeroArgMsg_st->Obi);
			}			
			
			
			 
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_Nero_CreateNetNet:\n");
			#endif
			break;
		case MsgId_Log_PrintObjMsgWithStr:
			NeroWithStrArgMsg_st=(struct  NeuronObjectMsgWithStr_  * )&LogMsg;
			
			for( i = 0; i < size_message_map2; i++)
			{
			    if( nero_msg_WithStr_map[i].id == NeroWithStrArgMsg_st->fucId )
				 (*(nero_msg_WithStr_map[i].operate) )(NeroWithStrArgMsg_st->Obi,NeroWithStrArgMsg_st->str);
			}			
			break;
	
		default:			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("LogId_Nero_NONE:  \n");
			 printf("Log msg=%s (%d)\n", LogMsg.text, received);	
			#endif	
			break;
		}
		

	}
	printf("end to wait Log msg................................\n");
}


/*从一个数组对象中提前词组，存与str中*/
nero_s32int IO_getWordsInNero(nero_8int str[],NeuronObject * obj)
{
	nero_s32int i,p;
	nero_us32int c=1;	
	nero_s32int ObjectKind;
	NeuronObject * tmp;
	NeuronObject * tmpObi;
/*	NerveFiber  *  curFiber;*/
	NerveFiber  *  tmpFiber1;
	if (str == NULL || obj == NULL)
	{
		return nero_msg_ParameterError;
	}


	tmpObi=obj;/*每一个衍生类*/
	ObjectKind=nero_GetNeroKind(tmpObi);	


	/*只是输出这个词汇有那几个字*/
	p=0;
	if (ObjectKind == NeuronNode_ForChWord )
	{
																	
		tmpFiber1=tmpObi->inputListHead;
		c=1;
		while(tmpFiber1)
		{

				tmp=tmpFiber1->obj->inputListHead->obj;
				sprintf(str+p,"%c%c%c",tmp->x,tmp->y,tmp->z);
				p+=3;
				
				tmpFiber1=tmpFiber1->next;
				c++;
		}
			if (c != 1)
			{
						
			}
			else
			{
				sprintf(str,"错误的词组对象");	
			}
	
	}
	else
	{
		sprintf(str,"错误的词组对象");						
	}

return nero_msg_ok;
}

/*从一个字符对象中提取，存与str中*/
nero_s32int IO_getZhInNero(nero_8int str[],NeuronObject * obj)
{
	
	nero_s32int ObjectKind;
	NeuronObject * tmp;
	NeuronObject * tmpObi;
	NerveFiber  *  tmpFiber1;
	if (str == NULL || obj == NULL)
	{
		return nero_msg_ParameterError;
	}


	tmpObi=obj;/*每一个衍生类*/
	ObjectKind=nero_GetNeroKind(tmpObi);	

	if (ObjectKind == NeuronNode_ForChCharacter )
	{
																	
		tmpFiber1=tmpObi->inputListHead;
		tmp=tmpFiber1->obj;
		sprintf(str,"%c%c%c",tmp->x,tmp->y,tmp->z);
	
	}
	else
	{
		sprintf(str,"错误的字符对象");						
	}

return nero_msg_ok;
}






