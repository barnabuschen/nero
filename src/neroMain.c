#ifndef   FILENAME
#define   FILENAME
#include "common/type.h"
#endif

#include <arpa/inet.h>
#include <netinet/in.h>

#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>

#include "NeuralNetwork/NeuralNetwork.h"
#include "common/Neuron.h"
#include "neroMain.h"
#include "common/fileOperating.h"
/*#include "graph/bmpRead.h"*/
#include "tools/readUTF8File.h"
#include "tools/createDot.h"
#include "tools/Nero_IO.h"
#include "NeuralNetwork/NeuralOperating.h"
#include "tools/Nero_IO.h"
#include "tools/Nero_Task.h"
// #include <cairo.h>
#include "neroMain.h"
extern nero_s32int readUTF8FileData(nero_8int * FileName);
extern nero_s32int CreateActNeroNet();
extern void createNeroNetDotGraph(NeuronObject *GodNero,  char *fileName);
extern void  testDataIn201608();
int x=50;
int y=50;

int recoverPic=0;


struct    PrintNeroLInkTree_St    objTreeSt;
static struct  NeuronObjectMsgWithStr_    neroObjMsgWithStr_st;
static struct  NeuronObjectMsg_    neroObjMsg_st;

static nero_8int  file_path_getcwd[FILEPATH_MAX];/*保存当前目录*/



void ProInitialization();
void main()
{

		// printf(" \n \n \n \n \n \nthe size of  ActivationNeuron is: %llu  \n ",  sizeof(ActNero));
// struct rlimit resource_limit;
// getrlimit(RLIMIT_STACK, &resource_limit);
// printf("STACK: soft_limit - %ld hard_limit - %ld\n", resource_limit.rlim_cur, resource_limit.rlim_max);

		// testDataIn201608();

		 // readIrisFileForData("/home/jty/nero/nero/src/data/iris/iris.data");

		 // exit(0);
		 
		ProInitialization();
}


//整个系统的启动函数
void ProInitialization()
{
		/*	int res;*/
		pthread_t a_thread;
		Operating_ipckey="/tmp/Operating_ipckey3";
		NeuronObject * tmpobj;
			NerveFiber *tmpFiber;
		createFile(Operating_ipckey);
		/*	printf("ProInitialization strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		key_t ipckey = ftok(Operating_ipckey, IPCKEY);
		/*	 Set up the message queue */
		Operating_mq_id = msgget(ipckey,IPC_CREAT);// IPC_CREAT
			// printf("ProInitialization Operating strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
		printf("ProInitialization :Operating_mq_id=%d\n", Operating_mq_id);	
		/*res =*/ pthread_create(&a_thread, NULL,thread_for_Operating_Pic, NULL);
		
		
		IO_ipckey="/tmp/IO_ipckey2";
		createFile(IO_ipckey);
		/*	printf("ProInitialization strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		ipckey = ftok(IO_ipckey, IPCKEY);
		/*	 Set up the message queue */
		IO_mq_id = msgget(ipckey,IPC_CREAT);// IPC_CREAT
		/*	printf("ProInitialization IO strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		printf("ProInitialization IO_mq_id=%d\n", IO_mq_id);	
		/*res =*/ pthread_create(&a_thread, NULL,thread_for_IO_Pic, NULL);
		
		
		Log_ipckey="/tmp/Log_ipckey2";
		createFile(Log_ipckey);
		/*	printf("ProInitialization strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		ipckey = ftok(Log_ipckey, IPCKEY);
		/*	 Set up the message queue */
		Log_mq_id = msgget(ipckey,IPC_CREAT);// IPC_CREAT
		/*	printf("ProInitialization Log strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		/*	printf("ProInitialization Log identifier is %d\n", Log_mq_id);	*/
		/*res =*/ pthread_create(&a_thread, NULL,thread_for_Log_Pic, NULL);
		
		Sys_ipckey="/tmp/Sys_ipckey";
		createFile(Sys_ipckey);
		/*	printf("ProInitialization strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		/*	ipckey = ftok(Sys_ipckey, IPCKEY);*/
		/*	 Set up the message queue */
		/*	Sys_mq_id = msgget(ipckey,IPC_CREAT);// IPC_CREAT*/
		/*	printf("ProInitialization Sys strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		/*	printf("ProInitialization Sys identifier is %d\n", Sys_mq_id);	*/
		/*res =*/ pthread_create(&a_thread, NULL,thread_for_Sys_Pic, NULL);
		
		
		// return;
			// sleep(5);

				nero_s32int xxxxxx,i;

		// testDebugForFileOperating();

		/*建立网络*/
		initNeroNetWork( );    
		sleep(4);	
		printf("initNeroNetWork ok\n");
		printf("ProInitialization ok\n");	


		// return;


		/*do   more  */
		nero_8int * fileName1="/data/taskFile.sh";
		ReadTaskFromTxt( fileName1  );
		printf("end of  fuc ReadTaskFromTxt\n");

		// nero_8int * fileName2="/data/taskFileForCreateIO.sh";
		// ReadTaskFromTxt( fileName2  );	
		// printf("end of  fuc ReadTaskFromTxt\n");



		// nero_8int * fileName4="/data/dataStringCreate.sh";
		// CreatedWordsIntoFile(fileName4);

		nero_8int * fileName3="/data/iris/LearningFileForIris.sh";
		ReadTaskFromTxt( fileName3  );
		printf("end of  fuc ReadTaskFromTxt\n");

		nero_8int * fileName4="/data/iris/TestingDataForIris.sh";
		ReadTaskFromTxt(fileName4);
		sleep(1);	

		// 在taskFile.sh中输入生成了英文字母得基类，现在需要生成英文单词得基类
		// 为了尽快看到结果，这里用整数来替换iris中得小数

		// sleep(10);	
		printf("\n\n\nTime  to  Search  Msg:::\n\n\n\n");

		#ifdef Nero_DeBuging14_01_14_
			// printf  msg  by  obj
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;//打印某个具体obj得信息  Log_printSomeMsgForObj
			neroObjMsgWithStr_st.Obi = GodNero;
			sprintf(neroObjMsgWithStr_st.str,"GodNero  add:%x",GodNero);
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
		#endif
		#ifdef Nero_DeBuging09_01_14_
			// print  one  obj  link
			neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
			neroObjMsg_st.fucId = 2;
			neroObjMsg_st.Obi = GodNero;
			int  tmp2222=0;
			// printf("nero   msg:%x,%x \n",GodNero,&tmp2222);
			msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);			
		#endif	


			sleep(4);
			
	 	#ifdef Nero_DeBuging10_01_14_
				// 尝试将kind=2013的所有衍生对象都进行outputlist的msg输出
				xxxxxx=2012;
				tmpobj=nero_getBaseObjByKind(xxxxxx,GodNero);
				if(tmpobj)
				{
					tmpFiber=tmpobj->outputListHead;
					while(tmpFiber != NULL  && tmpFiber->obj != NULL )
					{
						neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
						neroObjMsg_st.fucId = 3;//Log_printNeroObjLinkTree
						neroObjMsg_st.Obi = tmpFiber->obj;
						msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);

						tmpFiber=tmpFiber->next;
					}	
					printf("kind=2013  over...................................\n");	
				}
				else
					printf("kind=2013 cannot find.......................\n");
	    #endif	



			
		i=2012;
		{
 		#ifdef Nero_DeBuging10_01_14
				for(i=62;i<= 2016   ;i++)
				{
			// print  all  of  the  kind  obj
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;/*打印某个类别下面的所有的衍生类    Log_printAllKindOf*/
				neroObjMsgWithStr_st.Obi =NULL;
				 xxxxxx=i;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
				}
     	#endif

		}		

		for(;;)
		{		/*打印某个类别下面的所有的衍生类*/
				printf("...\n");				
				
				#ifdef Nero_DeBuging10_01_14
				// print  all  of  the  kind  obj
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;//Log_printAllKindOf
				neroObjMsgWithStr_st.Obi =NULL;
				 xxxxxx=NeuronNode_ForChWord;
				  // xxxxxx=NeuronNode_ForLayering;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
	     		#endif	
								// printf("\r");
				sleep(35);

		}
}
/*建立nero系统，他是发送给 消息给 void * thread_for_Operating_Pic(void *arg)建立网络  */
void initNeroNetWork( )
{

/*	int res;*/
	struct ZhCharArg arg1;
	struct DataFlowProcessArg arg2;


	struct { long type; char text[100]; } mymsg;
	readUTF8FileData("data/ChUnicode");
	printf("initNeroNetWork....\n");
	mymsg.type =MsgId_Nero_CreateNetNet;
	/*res=*/msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
/*	printf("msgsnd strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
/*	printf("msgsnd chars-%d.\n",res);*/

	/*一下步就是将字符信息加入网络 */

	//the msg  in  chChar  is  utf8  
	arg1.chChar=chChar;
	arg1.charCounts=charCounts;
	memcpy(&(mymsg.text),&arg1,sizeof(struct ZhCharArg));
	mymsg.type =MsgId_Nero_addZhCharIntoNet;
	/*res=*/msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);



	/*将一些词加入网络 */
	Utf8Word  wordsHead;

	#ifdef  Nero_DeBuging03_12_13_
	Utf8Word  MultiBytewordsHead;
/*	readUTF8FileForWords("data/词库" ,& MultiBytewordsHead);*/
        readUTF8FileForWords("data/ceshi2" ,& MultiBytewordsHead);
/*	readUTF8FileForWords("data/现代汉语常用词汇表utf8.txt" ,& MultiBytewordsHead);*/
/*	readUTF8FileForWords("data/实验词汇" ,& MultiBytewordsHead);*/
	nero_AddWordsIntoNet( GodNero,& MultiBytewordsHead);
	#endif


/*	printWords(&wordsHead);		*/
	/*字库*/
	#ifdef  Nero_DeBuging03_12_13_
	readUTF8FileForWords("data/ceshi2" ,& wordsHead);
	nero_AddWordsIntoNet( GodNero,& wordsHead);
	#endif


	#ifdef  Nero_DeBuging20_12_13_
{
	void **DataFlow;
	nero_s32int *dataKind;
	Utf8Word  *wP;
	char *linc;
	nero_s32int dataNum,k,countOfWord,m;
	// readUTF8FileForWords("data/词库" ,& wordsHead);
	readUTF8FileForWords("data/现代汉语常用词汇表utf8.txt" ,& wordsHead);
	/*将Utf8Word转化为一个数组，每个单位是一个词*/
	wP=wordsHead.next;
	countOfWord=0;
	while (wP)
	{
	// printf("wP->num=%d.\n",wP->num);
		countOfWord++;
		wP=wP->next;

	}
	(DataFlow)=(void **)malloc(sizeof(void *)*countOfWord);
	(dataKind)=(nero_s32int *)malloc(sizeof(nero_s32int *)*countOfWord);
	for (k=0,wP=wordsHead.next;k<countOfWord  &&  (wP != NULL);k++)
	{
		DataFlow[k]=(void *)malloc((sizeof( char)*(wP->num * 3+1)));
		linc=(char *)DataFlow[k];

		for (m=0;m<wP->num;m++)
		{
			memcpy(&(linc[m*3]), &((wP->words)[m]), (3));
		}

		linc[wP->num * 3]=0;
		dataKind[k]=NeuronNode_ForChWord;
		#ifdef  Nero_DeBuging20_12_13_
		printf("wP->num=%d.\n",wP->num);
		printf("len=%d,%s.\n\n",sizeof(linc),linc);
		#endif
		wP=wP->next;
	}
	#ifdef  Nero_DeBuging20_12_13_
	printf("++++++++++countOfWord=%d.\n",countOfWord);
	// printf("len=%d,%s.\n\n",sizeof(linc),linc);
	#endif		
	dataNum=countOfWord;
	neroConf.addLevelObjAlways = 0;


	arg2.dataNum=dataNum;
	arg2.dataKind=dataKind;
	arg2.conf=&neroConf;
	arg2.DataFlow=DataFlow;
	memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
	mymsg.type =MsgId_Nero_DataFlowProcess ;
	msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
}
	#endif


	

	// #ifdef  Nero_DeBuging20_12_13
	// sleep(4);
	void **DataFlow;
	nero_s32int *dataKind;
	Utf8Word  *wP;
	char *linc;
	nero_s32int dataNum,k,countOfWord,m;
	readUTF8FileForWords("data/词库" ,& wordsHead);

	/*将Utf8Word转化为一个数组，每个单位是一个词*/
		wP=wordsHead.next;
		countOfWord=0;
		while (wP)
		{
			countOfWord++;
			wP=wP->next;
		}
		(DataFlow)=(void **)malloc(sizeof(void *)*countOfWord);
		(dataKind)=(nero_s32int *)malloc(sizeof(nero_s32int *)*countOfWord);
		for (k=0,wP=wordsHead.next;k<countOfWord  &&  (wP != NULL);k++)
		{
			DataFlow[k]=(void *)malloc((sizeof( char)*(wP->num * 3+1)));
			linc=(char *)DataFlow[k];

			for (m=0;m<wP->num;m++)
			{
				memcpy(&(linc[m*3]), &((wP->words)[m]), (3));
			}

			linc[wP->num * 3]=0;
			dataKind[k]=NeuronNode_ForChWord;
			#ifdef  Nero_DeBuging20_12_13_
			printf("wP->num=%d.\n",wP->num);
			printf("len=%d,%s.\n\n",sizeof(linc),linc);
			#endif
			wP=wP->next;
		}	
		dataNum=countOfWord;
		neroConf.addLevelObjAlways = 0;
		arg2.dataNum=dataNum;
		arg2.dataKind=dataKind;
		arg2.conf=&neroConf;
		arg2.DataFlow=DataFlow;
		printf("词库  msg  send....\n");

		memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
		mymsg.type =MsgId_Nero_DataFlowProcess ;
		dataKind=msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
		printf("  msg  send  return:%d\n",dataKind);

	// #endif



	#ifdef  Nero_DeBuging10_16_16_
{	void **DataFlow;
	nero_s32int *dataKind;
	Utf8Word  *wP;
	char *linc;
	nero_s32int dataNum,k,countOfWord,m;
	readUTF8FileForWords("data/EnglishWords" ,& wordsHead);
	/*将Utf8Word转化为一个数组，每个单位是一个词*/
		wP=wordsHead.next;
		countOfWord=0;
		while (wP)
		{
			countOfWord++;
			wP=wP->next;
		}
		(DataFlow)=(void **)malloc(sizeof(void *)*countOfWord);
		(dataKind)=(nero_s32int *)malloc(sizeof(nero_s32int *)*countOfWord);
		for (k=0,wP=wordsHead.next;k<countOfWord  &&  (wP != NULL);k++)
		{
			DataFlow[k]=(void *)malloc((sizeof( char)*(wP->num * 3+1)));
			linc=(char *)DataFlow[k];

			for (m=0;m<wP->num;m++)
			{
				memcpy(&(linc[m*3]), &((wP->words)[m]), (3));
			}

			linc[wP->num * 3]=0;
			dataKind[k]=NeuronNode_ForChWord;
			#ifdef  Nero_DeBuging20_12_13_
			printf("wP->num=%d.\n",wP->num);
			printf("len=%d,%s.\n\n",sizeof(linc),linc);
			#endif
			wP=wP->next;
		}	
		dataNum=countOfWord;
		neroConf.addLevelObjAlways = 0;
		arg2.dataNum=dataNum;
		arg2.dataKind=dataKind;
		arg2.conf=&neroConf;
		arg2.DataFlow=DataFlow;
		memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
		mymsg.type =MsgId_Nero_DataFlowProcess ;
		msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
	}
	#endif





}