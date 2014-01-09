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
/*    MSG_NAME(2, add_data)*/
/*    MSG_NAME(3, del_data)*/
/*    MSG_NAME(4, sort_data)*/
END_ONE_ARG_MESSAGE_MAP








time_t   now;         //实例化time_t结构
struct  tm  *timenow;         //实例化tm结构指针

/*nero_8int  logFile[500]="../log/log.txt";*/
nero_8int  logFile[FILEPATH_MAX]="/tmp/log.txt";
nero_8int  file_path_getcwd[FILEPATH_MAX]="/tmp";/*保存当前目录*/

nero_s32int Log_printNeroObjMsg(void * arg)
{
	nero_8int  str[500];
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/
	sprintf(str,"Log:%s		something\n",asctime(timenow));		
/*	printf("str %s  and logFile=%s",str,logFile);*/

	
	
	
	
	

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
/*		printf("wait for Operating msg......\n");*/
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
	key_t ipckey;
	nero_s32int Log_mq_id;
	nero_s32int fucId;
	nero_s32int received;
	nero_s32int i = 0;

	const nero_s32int size_message_map = 
	      sizeof(nero_msg_print_map) / sizeof(struct one_arg_message_entry );   //求得表长
	

	 
	 getcwd(file_path_getcwd,FILEPATH_MAX);
	 /*设置日志目录*/
	 sprintf(logFile,"%s/log/log.txt",file_path_getcwd);
	 printf("file_path_getcwd: %s  and logFile=%s",file_path_getcwd,logFile);

	 if(!access(logFile, F_OK))  /*文件不存在就创建*/
	  {  
		
	  }  
	  else  
	  {  
	    createFile(logFile);
	  }  	

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
/*		printf("wait for Operating msg......\n");*/
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











