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

int x=50;
int y=50;
// GtkWidget * table;//主界面的table
// GtkWidget * buttoms[buttomNum];
// GtkWidget * srollWindows[textNum];
// GdkWindow *paintwindow;
// GtkWidget *darea;
// GtkWidget *Mainwindow;
// GtkWidget *dareaForComper[15];
// cairo_t  *currentCair ;
int recoverPic=0;
// cairo_t  *SavecurrentCair ;

// GtkWidget *fixed;
// cairo_surface_t *image;
// GtkWidget *notebook;

/*搜索面板中的text控件*/
// GtkWidget *textViewForSearch;
// GtkTextBuffer *textViewForSearchBuff;
// GtkWidget *textViewForSearchFixedInsideBox;
// GtkWidget * ceshibuttoms;


//tree  搜索面板中的各个变量
	//  GtkWidget   *typeChoseRadio1, *typeChoseRadio2,*typeChoseRadio0,* typeChoseRadioBox,*typeChoseEntry;
	//  	 GtkWidget   *outputTypeRadio1, *outputTypeRadio0,*outputTypeRadio2,*outputTypeRadioBox,*outputTypeEntry;
	// GtkWidget  *    lables[widgetsNum];
	gchar  *  				lableTexts[widgetsNum] ;
		gchar  *  				radioButtonTexts[widgetsNum] ;	
	
// GtkWidget *textViewForTreeCreates[widgetsNum];
// GtkTextBuffer *textBuffForTreeCreates[widgetsNum];



struct    PrintNeroLInkTree_St    objTreeSt;
static struct  NeuronObjectMsgWithStr_    neroObjMsgWithStr_st;
static struct  NeuronObjectMsg_    neroObjMsg_st;

static nero_8int  file_path_getcwd[FILEPATH_MAX];/*保存当前目录*/

// LineMan *manAllLineFromNeo;

void ProInitialization();
void main()
{


		ProInitialization();
}


//整个系统的启动函数
void ProInitialization()
{
		/*	int res;*/
		pthread_t a_thread;
		Operating_ipckey="/tmp/Operating_ipckey";
		createFile(Operating_ipckey);
		/*	printf("ProInitialization strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		key_t ipckey = ftok(Operating_ipckey, IPCKEY);
		/*	 Set up the message queue */
		Operating_mq_id = msgget(ipckey,IPC_CREAT);// IPC_CREAT
		/*	printf("ProInitialization Operating strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		/*	printf("ProInitialization Operating identifier is %d\n", Operating_mq_id);	*/
		/*res =*/ pthread_create(&a_thread, NULL,thread_for_Operating_Pic, NULL);
		
		
		IO_ipckey="/tmp/IO_ipckey";
		createFile(IO_ipckey);
		/*	printf("ProInitialization strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		ipckey = ftok(IO_ipckey, IPCKEY);
		/*	 Set up the message queue */
		IO_mq_id = msgget(ipckey,IPC_CREAT);// IPC_CREAT
		/*	printf("ProInitialization IO strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
		/*	printf("ProInitialization IO identifier is %d\n", IO_mq_id);	*/
		/*res =*/ pthread_create(&a_thread, NULL,thread_for_IO_Pic, NULL);
		
		
		Log_ipckey="/tmp/Log_ipckey";
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
		
		
		
		/*	sleep(1);*/
		/*建立网络*/
		initNeroNetWork( );    
		sleep(1);	
		printf("initNeroNetWork ok\n");
		printf("ProInitialization ok\n");	

		/*do   more  */
		// JustDoTask();/*生成  数   和  阿拉伯数字 的概念*/
		// ReadTaskFromTxt();


		#ifdef Nero_DeBuging14_01_14
			// printf  msg  by  obj
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi = GodNero;
			sprintf(neroObjMsgWithStr_st.str,"test:");
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
		#endif
		#ifdef Nero_DeBuging09_01_14
			// print  one  obj  link
			neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
			neroObjMsg_st.fucId = 2;
			neroObjMsg_st.Obi = GodNero;
			int  tmp2222=0;
			printf("nero   msg:%x,%x \n",GodNero,&tmp2222);
			msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);			
		#endif	
 		#ifdef Nero_DeBuging10_01_14
			// print  all  of  the  kind  obj
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				nero_s32int xxxxxx=NeuronNode_ForChCharacter;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));
				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
     	#endif	
		for(;;)
		{
								// printf("...\n");				
								sleep(1);
								// printf("\r");

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
	void **DataFlow;
	nero_s32int *dataKind;
	Utf8Word  *wP;
	char *linc;
	nero_s32int dataNum,k,countOfWord,m;
	// printf("Nero_DeBuging20_12_13:::::::\n");
	readUTF8FileForWords("data/词库" ,& wordsHead);
/*	readUTF8FileForWords("data/现代汉语常用词汇表utf8.txt" ,& wordsHead);*/
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
			#ifdef  Nero_DeBuging20_12_13
			printf("wP->num=%d.\n",wP->num);
			printf("len=%d,%s.\n\n",sizeof(linc),linc);
			#endif
			wP=wP->next;
		}
		dataNum=countOfWord;
		neroConf.addLevelObjAlways = 1 ;


	arg2.dataNum=dataNum;
	arg2.dataKind=dataKind;
	arg2.conf=&neroConf;
	arg2.DataFlow=DataFlow;
/*	memset(mymsg.text, 0, 100);  */
	memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
	mymsg.type =MsgId_Nero_DataFlowProcess ;
	msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);


	#endif






}