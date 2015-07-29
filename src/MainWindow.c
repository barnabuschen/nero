#ifndef   FILENAME
#define   FILENAME
#include "common/type.h"
#endif

#include <arpa/inet.h>
#include <netinet/in.h>



#include <stdlib.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include<pthread.h>
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


#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>


#include "NeuralNetwork/NeuralNetwork.h"
/*#include "common/Neuron.h"*/
#include "MainWindow.h"
#include "common/fileOperating.h"
/*#include "graph/bmpRead.h"*/
#include "tools/readUTF8File.h"
#include "tools/createDot.h"
#include "tools/Nero_IO.h"
#include "NeuralNetwork/NeuralOperating.h"
#include "tools/Nero_IO.h"
#include "tools/Nero_Task.h"
#include <cairo.h>

extern nero_s32int readUTF8FileData(nero_8int * FileName);
extern nero_s32int CreateActNeroNet();
extern void createNeroNetDotGraph(NeuronObject *GodNero,  char *fileName);

gint x=50;
gint y=50;
GtkWidget * table;//主界面的table
GtkWidget * buttoms[buttomNum];
GtkWidget * srollWindows[textNum];
GdkWindow *paintwindow;
GtkWidget *darea;
GtkWidget *Mainwindow;
GtkWidget *dareaForComper[15];
cairo_t  *currentCair ;
int recoverPic=0;
cairo_t  *SavecurrentCair ;

GtkWidget *fixed;
cairo_surface_t *image;
GtkWidget *notebook;

/*搜索面板中的text控件*/
GtkWidget *textViewForSearch;
GtkTextBuffer *textViewForSearchBuff;
GtkWidget *textViewForSearchFixedInsideBox;
GtkWidget * ceshibuttoms;


//tree  搜索面板中的各个变量
	 GtkWidget   *typeChoseRadio1, *typeChoseRadio2,*typeChoseRadio0,* typeChoseRadioBox,*typeChoseEntry;
	 	 GtkWidget   *outputTypeRadio1, *outputTypeRadio0,*outputTypeRadio2,*outputTypeRadioBox,*outputTypeEntry;
	GtkWidget  *    lables[widgetsNum];
	gchar  *  				lableTexts[widgetsNum] ;
		gchar  *  				radioButtonTexts[widgetsNum] ;	
	
GtkWidget *textViewForTreeCreates[widgetsNum];
GtkTextBuffer *textBuffForTreeCreates[widgetsNum];
	#define  ChoseBaseObj   1
	#define  ChoseDerivativeObj   2	
	
	#define  ChoseOutputAddress  1
	#define  ChoseOutputType   2	
	#define  ChoseOutputData  3


static struct    PrintNeroLInkTree_St    objTreeSt;
static struct  NeuronObjectMsgWithStr_    neroObjMsgWithStr_st;
static struct  NeuronObjectMsg_    neroObjMsg_st;

static nero_8int  file_path_getcwd[FILEPATH_MAX];/*保存当前目录*/

LineMan *manAllLineFromNeo;


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
	printf("initNeroNetWork ok\n");

	/*do   more  */
	//JustDoTask();/*生成  数   和  阿拉伯数字 的概念*/
	 ReadTaskFromTxt();
	// CreateBaseKindOfShu();
/* 	 ModifyBaseKindOfShu();*/


        printf("ProInitialization ok\n");
}
void frame_callback(GtkWindow *window, GdkEvent *event, gpointer data)
{


}
gint mouseEnter( GtkWidget *widget, GdkEvent *event, gpointer data )
{

	return 0;
}
void hello( GtkWidget *widget, gpointer data )
{
/*

GtkWidget *Mainwindow;
GtkWidget *dareaForComper[15];
cairo_t  *currentCair ;
int recoverPic=0;
cairo_t  *SavecurrentCair ;

GtkWidget *fixed;
cairo_surface_t *image;
ceshibuttoms
textViewForSearchFixedInsideBox
GtkWidget *textViewForSearch;
*/



	GtkWidget *mywidget=textViewForSearchFixedInsideBox;

	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(mywidget));
	spin_myWidget_draw (mywidget, &cr);



}
/*打算在这个函数里面读取文本中的信息搜索请求*/
/*请求的信息格式，仿照Nero_Task里面*/
void drow1( GtkWidget *widget, gpointer data )
{
/*struct { long type; char text[100]; } mymsg;*/
/*		memset(mymsg.text, 0, 100);  //Clear out the space */
/*		strcpy(mymsg.text,"测试" );//newfilename就是发送的字符串*/
/*		mymsg.type = 1;*/
/*		msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);*/
/*		*/
/*		msgsnd( IO_mq_id, &mymsg, sizeof(mymsg), 0);*/
/*		msgsnd( Log_mq_id, &mymsg, sizeof(mymsg), 0);*/








}
//~ widget传进来的就是buff的指针而不是iew的指针

void tab_textViewForSearchChanged( GtkWidget *widget, gpointer data )
{
 	 GtkTextIter /*iter,*/start,end;
	 gtk_text_buffer_get_start_iter (textViewForSearchBuff,&start);
	 gtk_text_buffer_get_end_iter (textViewForSearchBuff,&end);
	 gtk_text_buffer_apply_tag_by_name(textViewForSearchBuff,"深粉红",&start,&end);


}
//~ widget传进来的就是buff的指针而不是iew的指针
void tab_textViewForTreeCreateChanged( GtkWidget *widget, gpointer data )
{
	
	gint    flag;
		gchar *    text;
 	 GtkTextIter /*iter,*/start,end;
	 gtk_text_buffer_get_start_iter (widget,&start);
	 gtk_text_buffer_get_end_iter (widget,&end);
	 gtk_text_buffer_apply_tag_by_name(widget,"深粉红",&start,&end);

	flag =data;
	text = gtk_text_buffer_get_text (widget, &start,&end,FALSE);
	switch(flag)
	{
		case 1:
							//~ printf("ob  address  changed :%s\n", text);	
							objTreeSt.obAddress    =    mystrToInt(text ) ;
						     break;
			case 2:
								objTreeSt.treeDepth    =    atoi(text ) ;
							break;	
		case 3:
							objTreeSt.treeMaxdegree =   atoi(text ) ;
							break;		
			case 4:
								if(text  !=  NULL)
										strcpy(objTreeSt.outputMsgFile, text);
								break;	
		case 5:
								if(text  !=  NULL)
										strcpy(objTreeSt.outputTreeFile, text);
								break;		
		case 6:
								if(text  !=  NULL)
										strcpy(objTreeSt.obTypeName, text);
								break;			
			case 7:
								if(text  !=  NULL)
										//~ strcpy(objTreeSt.obData, text);
										objTreeSt.obData    =    (int  *)mystrToInt(text ) ;
								break;		
		default:break;
		}
}
void tab_SetSearchNeroMsgViewText(gchar *  str)
{

	gtk_text_buffer_set_text (textViewForSearchBuff, str, -1);




}
void tab_searchNeroMsg( GtkWidget *widget, gpointer data )
{
	NeuronObject *obj;
	struct  NeuronObjectMsgWithStr_   mymsg;
/*	 gtk_text_buffer_get_iter_at_offset(textViewForSearchBuff, &iter, 0);*/
/*	 gtk_text_buffer_insert_with_tags_by_name(textViewForSearchBuff, &iter, "Colored Text\n", -1, "blue_fg", "lmarg",  NULL);*/
 	 GtkTextIter /*iter,*/start,end;
	 gtk_text_buffer_get_start_iter (textViewForSearchBuff,&start);
	 gtk_text_buffer_get_end_iter (textViewForSearchBuff,&end);

	gchar *  str=gtk_text_buffer_get_text(textViewForSearchBuff,&start,&end,TRUE);
	printf("%s.\n",str);


	obj=   (NeuronObject *)strtol(str,NULL,16);
	printf("obj=:%x\n",(unsigned int )obj);

	if (str)
	{
		g_free (str);
	}


	mymsg.MsgId = MsgId_Log_PrintObjMsgWithStr;
	mymsg.fucId = 1;
	mymsg.Obi =obj;
	sprintf(mymsg.str,"查询对象信息\n");
	msgsnd( Log_mq_id, &mymsg, sizeof(mymsg), 0);


}
void tab_showBaseNeurontree( GtkWidget *widget, gpointer data )
{
	NeuronObject *obj;
	struct  NeuronObjectMsgWithStr_   mymsg;
/*	 gtk_text_buffer_get_iter_at_offset(textViewForSearchBuff, &iter, 0);*/
/*	 gtk_text_buffer_insert_with_tags_by_name(textViewForSearchBuff, &iter, "Colored Text\n", -1, "blue_fg", "lmarg",  NULL);*/
 	 GtkTextIter /*iter,*/start,end;
	 gtk_text_buffer_get_start_iter (textViewForSearchBuff,&start);
	 gtk_text_buffer_get_end_iter (textViewForSearchBuff,&end);

	gchar *  str=gtk_text_buffer_get_text(textViewForSearchBuff,&start,&end,TRUE);
	printf("%s.\n",str);

	//~ 这里需要吧指定的基类名（就是一个词），转换成相应的基类对象的地址
	//~ obj=   (NeuronObject *)strtol(str,NULL,16);
	//~ printf("obj=:%x\n",(unsigned int )obj);





	if (str)
	{
		g_free (str);
	}


	mymsg.MsgId = MsgId_Log_PrintObjMsgWithStr;
	mymsg.fucId = 1;
	mymsg.Obi =obj;
	sprintf(mymsg.str,"tab_showBaseNeurontree\n");
	msgsnd( Log_mq_id, &mymsg, sizeof(mymsg), 0);


}
void drow2( GtkWidget *widget, gpointer data )
{
struct  NeuronObjectMsg_   mymsg;

        NeuronObject * tmp;
        nero_s32int i,mark,objKind,baseKind,baseDataKind,flag,ObjectKind,hasModify;
        NeuronObject *Obi,*tmpChildObi,* baseObj,* godNero;
	NerveFiber *curFiber,* tmpFiber;
                godNero=GodNero;

        curFiber=godNero->outputListHead;
        baseObj=NULL;
/*        printf("start show baseDataKind curFiber=%x\n",curFiber);*/
	for (;curFiber !=NULL ;curFiber=curFiber->next)
	{
	         Obi=curFiber->obj;
	         baseDataKind=nero_GetNeroKind(Obi);
/*	         printf("shoe baseDataKind=%d.\n",baseDataKind);       */
	        if (baseDataKind >=NeuronNode_MinNewDerivativeClassId)
	        {


                                #ifdef Nero_DeBuging10_01_14
                                /*打印基类的组成结构*/
                                tmpFiber=Obi->inputListHead;
                                printf("基类%d的组成结构\n",nero_GetNeroKind(Obi));
                                for (;tmpFiber !=NULL ;tmpFiber=tmpFiber->next)
                                {

                                        printf("        子结构kind=%d,性质：%d\n",nero_GetNeroKind(tmpFiber->obj), getFiberPointToObjNum(tmpFiber) );

                                }


                                #endif

 	                        #ifdef Nero_DeBuging10_01_14
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				nero_s32int xxxxxx=baseDataKind;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));


				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
                                 #endif



	        }


	}








 	#ifdef Nero_DeBuging10_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				nero_s32int xxxxxx=neroConf.NewNeroClassID-1;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));


				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);
        #endif
}
gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{

/*		if(recoverPic ==4)*/
/*			{*/
/*	DrawPictureWithLineManInVector(darea,manAllLineFromNeo);*/
/*			}*/

	return FALSE;
}
gint delete_eventInMainWin( GtkWidget *widget, GdkEvent *event, gpointer data )
{

/*	if(shmdt(pMapWithCandy)==-1)*/
/*		perror(" detach error ");*/
/*	else */
/*		g_print(" detach ok \n");*/



	g_print ("delete event occurred in main\n");
	gtk_main_quit();

/* 将TRUE改为FALSE,主窗口就会用一个 " d e l e t e _ e v e n t "信号,然后退出 */
	return(TRUE);
}
void myMainWindow(GtkWidget *window)
{
	recoverPic=0;
	Mainwindow=window;

	g_signal_connect (window,"delete-event",G_CALLBACK (delete_eventInMainWin),NULL);
	g_signal_connect(G_OBJECT(window), "configure-event",G_CALLBACK(frame_callback), NULL);
/*	g_signal_connect(window,"motion-notify-event",G_CALLBACK (delete_event),NULL)*/
	gtk_widget_add_events (window, GDK_POINTER_MOTION_MASK);
	g_signal_connect (window, "motion-notify-event",G_CALLBACK(mouseEnter), NULL);

	//gint i=1;
/*	int rows=2;*/
/*	int columns=1;*/
	//int buttomNum=4;
	int buttomID=0;
/*	gint xpadding=10;*/
/*	gint ypadding=10;*/

	table=gtk_grid_new();
/*	gtk_grid_insert_row(GTK_TABLE(table),xpadding);*/
/*	gtk_table_set_col_spacings(GTK_TABLE(table),ypadding);*/
	//gtk_container_add (GTK_CONTAINER (window), table);
	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);

	buttoms[buttomID]=gtk_button_new_with_label("bt1");
	g_signal_connect (buttoms[0], "clicked",G_CALLBACK(hello), NULL);
	//gtk_table_attach_defaults(GTK_TABLE(table),buttoms[0],0,1,0,1);
	//gtk_widget_set_usize(buttoms[buttomID],40,40);
	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 0, 0);
	gtk_widget_show (buttoms[0]);


	buttomID++;
	buttoms[buttomID]=gtk_button_new_with_label("show neo");
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(drow1), NULL);
	//gtk_table_attach_defaults(GTK_TABLE(table),buttoms[0],0,1,0,1);
	//gtk_widget_set_usize(buttoms[buttomID],40,40);
	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 60, 0);
	gtk_widget_show (buttoms[buttomID]);


	buttomID++;
	buttoms[buttomID]=gtk_button_new_with_label("tools sample");
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(drow2), NULL);
	//gtk_table_attach_defaults(GTK_TABLE(table),buttoms[0],0,1,0,1);
	//gtk_widget_set_usize(buttoms[buttomID],40,40);
	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 160, 0);
	gtk_widget_show (buttoms[buttomID]);

/*	 darea = gtk_drawing_area_new();*/
/*         gtk_fixed_put (GTK_FIXED (fixed),darea, 0, 30);*/
/*	g_signal_connect (G_OBJECT (darea), "draw",G_CALLBACK (draw_callback), NULL);*/
/*	gtk_window_get_size(GTK_WINDOW(window),&x,&y);*/
/*	 gtk_widget_set_size_request (darea, x*2/3, y);*/
/*	*/
/*	int dareaForComperID=0;*/
/*	dareaForComper[dareaForComperID]= gtk_drawing_area_new();;*/
/*         gtk_fixed_put (GTK_FIXED (fixed),dareaForComper[dareaForComperID], x*2/3, 30);*/
/*	gtk_window_get_size(GTK_WINDOW(window),&x,&y);*/
/*	 gtk_widget_set_size_request (dareaForComper[dareaForComperID], x*2/3, y);*/
/*	gtk_widget_show (dareaForComper[dareaForComperID]);	*/


/*	gtk_widget_show (darea);*/
/*	gtk_widget_show (fixed);*/



	/*GtkWidget **/notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook),TRUE);
	gtk_fixed_put (GTK_FIXED (fixed),notebook, 0, 30);
/*	gtk_container_add(GTK_CONTAINER(window), notebook);*/
	gtk_window_get_size(GTK_WINDOW(window),&x,&y);
	gtk_widget_set_size_request (notebook, x, y);
	gint count;

	for (count = 1; count <= 4; count++)
	{

		createTab1_InMainWindow( (window), count, notebook);
	}

	gtk_widget_show_all(window);









	gtk_widget_show_all(window);
	//工程信息初始化
	ProInitialization();
}

void SetUpSomeWordsIntoSys( GtkWidget *widget, gpointer data )
{

	/*从默认的配置文件中读取数据所在的文件，向void *thread_for_IO_Pic(void *arg)发送消息*/
	struct  IODataMsg_  DataIO_st;
	gchar * fileName="./NeroConfig/wordsFileName";
	int  countline,i;
	char * datafile;
	nero_8int  dataFilePath[FILEPATH_MAX];
	int * line=findAllLine(fileName,&countline);
	if (line)
	{
			free(line);
	}
	getcwd(file_path_getcwd,FILEPATH_MAX);


			/*现在可以想系统发送消息了*/
	DataIO_st.MsgId = MsgId_Nero_ConfModify;
	DataIO_st.fucId = 1;
	DataIO_st.operateKind =Conf_Modify_addLevelObjAlways;
	memcpy(DataIO_st.str,&neroConf,sizeof(NeroConf));
	((NeroConf *)DataIO_st.str)->addLevelObjAlways=1;
	msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);



/*-----------------------------*/
	/*将一些词加入网络 */
	Utf8Word  wordsHead;
	Utf8Word  MultiBytewordsHead;

	readUTF8FileForWords("data/实验词汇" ,& MultiBytewordsHead);
	nero_AddWordsIntoNet( GodNero,& MultiBytewordsHead);





/*--------------------------------*/
	((NeroConf *)DataIO_st.str)->addLevelObjAlways=0;
	msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);

}
/*给系统发送数据，*/
void GetDataForNeroSys( GtkWidget *widget, gpointer data )
{
	/*从默认的配置文件中读取数据所在的文件，向void *thread_for_IO_Pic(void *arg)发送消息*/
	struct  IODataMsg_  DataIO_st;
	gchar * fileName="./NeroConfig/wordsFileName";
	int  countline,i;
	char * datafile;
	nero_8int  dataFilePath[FILEPATH_MAX];
	int * line=findAllLine(fileName,&countline);
	if (line)
	{
			free(line);
	}
	getcwd(file_path_getcwd,FILEPATH_MAX);


	/*将一些词加入网络 */
	Utf8Word  wordsHead;
	Utf8Word  MultiBytewordsHead;

	readUTF8FileForWords("data/实验词汇" ,& MultiBytewordsHead);
/*	nero_AddWordsIntoNet( GodNero,& MultiBytewordsHead);*/


	for (i=0;i<countline;i++)
	{
		 datafile=getLineInFile(fileName,i+1);

		 if (strlen(datafile) >0  && strlen(datafile) <  FILEPATH_MAX)
		 {
		 	sprintf(dataFilePath,"%s/%s",file_path_getcwd,datafile);


			/*现在可以想系统发送消息了*/
			DataIO_st.MsgId = MsgId_IO_dataIO;
			DataIO_st.fucId = 1;
			DataIO_st.operateKind = 1;
			strcpy(DataIO_st.str,dataFilePath);//newfilename就是发送的字符串
			msgsnd(IO_mq_id, &DataIO_st, sizeof(DataIO_st), 0);
		 }

/*		printf("get file  %s\n",dataFilePath);*/
		if (datafile)
		{
			free(datafile);
		}

	}


}
void readUTF8File( GtkWidget *widget, gpointer data )
{
	GtkWidget * dialog= dialog = gtk_message_dialog_new (Mainwindow,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_CLOSE,
                                 "done");



	readUTF8FileData("data/ChUnicode");

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

}
void createToolsTab(GtkWidget *fixedInside)
{
	gchar *text;
	gint buttomID=1;
	GtkWidget * buttoms[buttomNum];
	GtkWidget *vbox = gtk_box_new(FALSE, 5);/*存放其他功能构建的容器*/;


	/*读取utf8编码的文件的按钮*/
	text = g_strdup_printf("读取utf8编码的文件");
	buttoms[buttomID]=gtk_button_new_with_label(text);
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(readUTF8File), NULL);
	gtk_fixed_put (GTK_FIXED (fixedInside), buttoms[buttomID], 0, 0);
	buttomID++;
	/*输入词汇概念的按钮*/
	text = g_strdup_printf("输入词汇概念的按钮");
	buttoms[buttomID]=gtk_button_new_with_label(text);
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(GetDataForNeroSys), NULL);
	gtk_fixed_put (GTK_FIXED (fixedInside), buttoms[buttomID], 0, 30);
	buttomID++;

	text = g_strdup_printf("设置指定词汇");
	buttoms[buttomID]=gtk_button_new_with_label(text);
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(SetUpSomeWordsIntoSys), NULL);
	gtk_fixed_put (GTK_FIXED (fixedInside), buttoms[buttomID], 0, 60);
	buttomID++;
/*	gtk_widget_set_size_request (vbox, 15, 15);*/



}
void CreateNeroNetWork_old( GtkWidget *widget, gpointer data )
{
	GtkWidget * dialog= dialog = gtk_message_dialog_new (Mainwindow,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_CLOSE,
                                 "done here");
/*struct { long type; char text[100]; } mymsg;*/

	readUTF8FileData("data/ChUnicode");


		#ifdef  Nero_DeBuging0
		printf("FileData   done.\n");
		#endif

	CreateActNeroNet();

		#ifdef  Nero_DeBuging0
		printf("CreateActNeroNet   done.\n");
		#endif
	/*一下步就是将字符信息加入网络 */
	nero_addZhCharIntoNet( GodNero,chChar, charCounts);
		#ifdef  Nero_DeBuging0
		printf("addZhCharIntoNet   done.\n");
		#endif


	/*将一些词加入网络 */
	Utf8Word  wordsHead;
/*	Utf8Word  MultiBytewordsHead;	*/
	#ifdef  Nero_DeBuging03_12_131_
/*	readUTF8FileForWords("data/词库" ,& MultiBytewordsHead);*/
	readUTF8FileForWords("data/现代汉语常用词汇表utf8.txt" ,& MultiBytewordsHead);
	nero_AddWordsIntoNet( GodNero,& MultiBytewordsHead);
	#endif
/*	printWords(&wordsHead);		*/
	/*字库*/
	#ifdef  Nero_DeBuging03_12_13_
	readUTF8FileForWords("data/ceshi2" ,& wordsHead);
	nero_AddWordsIntoNet( GodNero,& wordsHead);
	#endif

	#ifdef  Nero_DeBuging20_12_13
	void **DataFlow;
	nero_s32int *dataKind;
	Utf8Word  *wP;
	char *linc;
	nero_s32int dataNum,k,countOfWord,m;
	readUTF8FileForWords("data/ceshi2" ,& wordsHead);
	/*将Utf8Word转化为一个数组，每个单位是一个词*/
		wP=wordsHead.next;
		countOfWord=0;
		while (wP)
		{
/*		printf("wP->num=%d.\n",wP->num);*/
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
		neroConf.addLevelObjAlways = 1 ;
	DataFlowProcess(DataFlow,dataKind,dataNum,  GodNero,  &neroConf);
	#endif


	/*show  neroNet*/
	#ifdef  Nero_DeBuging03_12_13
/*	createNeroNetDotGraph(GodNero, "data/pic.dot");*/
	 createNeroNetDotGraphForWords(GodNero, "data/wordspic.dot");
	printf("createNeroNetDotGraph   done.\n");
	#endif



	printf("everything   done.\n");

	#ifdef  Nero_DeBuging03_12_13
	system("xdot data/wordspic.dot");
	#endif




	#ifdef  Nero_DeBuging11
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	#endif
	printf("everything   done.\n");



}
void  typeChoseRadiotoggledHandle( GtkToggleButton  *widget, gpointer data )
{
	
	
	printf("%s\n",data);
	
	
}
void  outputTypeRadiotoggledHandle( GtkToggleButton  *widget, gpointer data )
{
	
	
	printf("%s\n",data);
	
	
}
		//~ struct 
	//~ {
			//~ gint choseType;//默认是ChoseDerivativeObj
			//~ gint degreeOutputType;//默认是ChoseOutputAddress
			//~ gint *  obAddress;
			//~ gint 	 treeDepth;
			//~ gint    treeMaxdegree;
			//~ gchar    outputMsgFile[300];
			//~ gchar    outputTreeFile[300];
			//~ gchar    obTypeName[300];
			//~ gchar    obData[300];			
		//~ }objTreeSt; 
	 //~ 
void ToCreateTreeButtonClicked( GtkWidget *widget, gpointer data )
{
		printf("	choseType:%d\n	degreeOutputType:%d\n	obaddress:0x%x\n	treeMaxdegree:%d\n	outputMsgFile:%s\n	outputTreeFile:%s\n	obtypename:%s\n	obdata:0x%x\n",
																	objTreeSt.choseType,
																	objTreeSt.degreeOutputType,
																	 objTreeSt.obAddress,
																	objTreeSt.treeMaxdegree,
																	objTreeSt.outputMsgFile,
																	objTreeSt.outputTreeFile,
																	objTreeSt.obTypeName,
																	 objTreeSt.obData);
																	
						
						
	
		neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
		neroObjMsg_st.fucId = 3;
		neroObjMsg_st.Obi = (void *)&objTreeSt;
		//~ printf("nero_IfHasObjFromMultiples2 msg%x.\n",obj);
		msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);								


	
				
			
	}
void CreateNeroNetWork( GtkWidget *widget, gpointer data )
{

/*	int res;*/
	struct ZhCharArg arg1;
	struct DataFlowProcessArg arg2;
	GtkWidget * dialog= dialog = gtk_message_dialog_new (Mainwindow,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_CLOSE,
                                 "done here");


	struct { long type; char text[100]; } mymsg;




	readUTF8FileData("data/ChUnicode");


		#ifdef  Nero_DeBuging0
		printf("FileData   done.\n");
		#endif

	mymsg.type =MsgId_Nero_CreateNetNet;
/*	memset(mymsg.text, 0, 100);*/
/*	strcpy(mymsg.text,"测试" );*/
	/*res=*/msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
/*	printf("msgsnd strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
/*	printf("msgsnd chars-%d.\n",res);*/
		#ifdef  Nero_DeBuging0
		printf("CreateActNeroNet   done.\n");
		#endif
	/*一下步就是将字符信息加入网络 */
	arg1.chChar=chChar;
	arg1.charCounts=charCounts;
	memcpy(&(mymsg.text),&arg1,sizeof(struct ZhCharArg));
	mymsg.type =MsgId_Nero_addZhCharIntoNet;
	/*res=*/msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
/*	printf("msgsnd strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息*/
/*	printf("msgsnd chars-%d.\n",res);*/
		#ifdef  Nero_DeBuging0
		printf("addZhCharIntoNet   done.\n");
		#endif


	/*将一些词加入网络 */
	Utf8Word  wordsHead;
/*	Utf8Word  MultiBytewordsHead;	*/
	#ifdef  Nero_DeBuging03_12_131_
/*	readUTF8FileForWords("data/词库" ,& MultiBytewordsHead);*/
	readUTF8FileForWords("data/现代汉语常用词汇表utf8.txt" ,& MultiBytewordsHead);
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
	readUTF8FileForWords("data/ceshi2" ,& wordsHead);
	/*将Utf8Word转化为一个数组，每个单位是一个词*/
		wP=wordsHead.next;
		countOfWord=0;
		while (wP)
		{
/*		printf("wP->num=%d.\n",wP->num);*/
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
		neroConf.addLevelObjAlways = 1 ;


	arg2.dataNum=dataNum;
	arg2.dataKind=dataKind;
	arg2.conf=&neroConf;
	arg2.DataFlow=DataFlow;
/*	memset(mymsg.text, 0, 100);  */
	memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
	mymsg.type =MsgId_Nero_DataFlowProcess ;
	msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);

/*	DataFlowProcess(DataFlow,dataKind,dataNum,  GodNero,  &neroConf);*/
	#endif




	#ifdef  Nero_DeBuging11
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	#endif

}

void createMsgSearchTab(GtkWidget *fixedInside)
{
/*GtkWidget *textViewForSearch;*/
/*GtkTextBuffer *textViewForSearchBuff;*/
	 GtkTextIter /*iter,*/start,end;
	 GdkRGBA  rgba;
	 #define TabSize  150
	gint x=TabSize;
	gint y=TabSize;


/*	gtk_window_get_size(GTK_WINDOW(fixedInside),&x,&y);*/
/*	GtkWidget *fixedInside = gtk_fixed_new();*/
/*	gtk_widget_set_size_request (fixedInside, x/3, y/2);*/


	gchar *text;
	gint buttomID=1;
	GtkWidget * buttoms[buttomNum];
/*	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);;*/




	//~ 这个按钮的作用为：
	text = g_strdup_printf("search nero");
	buttoms[buttomID]=gtk_button_new_with_label(text);
	ceshibuttoms=buttoms[buttomID];

	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(tab_searchNeroMsg), NULL);
	gtk_fixed_put (GTK_FIXED (fixedInside), buttoms[buttomID], 0, 0);
	buttomID++;

	//~ 这个按钮的作用为：用来查询一个指定名称的类的所有子成员，并得到一个dot图
	text = g_strdup_printf("show Base Neuron tree");
	buttoms[buttomID]=gtk_button_new_with_label(text);
	ceshibuttoms=buttoms[buttomID];
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(tab_showBaseNeurontree), NULL);
	gtk_fixed_put (GTK_FIXED (fixedInside), buttoms[buttomID], 100, 0);
	buttomID++;


	/*view*/
	textViewForSearch = gtk_text_view_new ();
	textViewForSearchBuff = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForSearch));
	gtk_text_buffer_set_text (textViewForSearchBuff, "Hello, this is some text", -1);
	g_signal_connect (textViewForSearchBuff, "changed",G_CALLBACK(tab_textViewForSearchChanged), NULL);
/*	gtk_container_add(GTK_CONTAINER(vbox), textViewForSearch);*/

	/*tag  add*/
	 rgba.red=(gdouble)0xFF/(gdouble)255;
	 rgba.green=(gdouble)0x14/(gdouble)255;
	 rgba.blue=(gdouble)0x93/(gdouble)255;

/*	 rgba.red=(gdouble)0x0/(gdouble)255;*/
/*	 rgba.green=(gdouble)0x0/(gdouble)255;*/
/*	 rgba.blue=(gdouble)0x0/(gdouble)255;	 */
	 rgba.alpha=1;
	 //~ gtk_text_buffer_create_tag(textViewForSearchBuff, "blue_fg", "foreground", "blue", NULL);
	 //~ gtk_text_buffer_create_tag(textViewForSearchBuff, "lmarg", "left_margin", 5, NULL);
	 gtk_text_buffer_create_tag(textViewForSearchBuff, "深粉红", "foreground-rgba", &rgba, NULL);

	 gtk_text_buffer_get_start_iter (textViewForSearchBuff,&start);
	 gtk_text_buffer_get_end_iter (textViewForSearchBuff,&end);
	 gtk_text_buffer_apply_tag_by_name(textViewForSearchBuff,"深粉红",&start,&end);

/*	gtk_widget_set_size_request (vbox, 15, 15);*/
/*	gtk_fixed_put (GTK_FIXED (fixedInside), vbox, 0, 0);*/
	gtk_fixed_put (GTK_FIXED (fixedInside), textViewForSearch, 0, 35);




}
void createNeroTreeTab(GtkWidget *fixedInside)
{
		gchar *text;
	GtkWidget * buttoms[buttomNum];		
	gint buttomID=1;
	 GtkTextIter /*iter,*/start,end;
	gint i=0,typeChoseRadioBoxXPos=80,typeChoseRadioBoxYPos=20,padBetweenLable=45,chae=18,sendondLie=230,xpadForTextView=150;
	 GdkRGBA  rgba;
	 gchar   str1[]="NULL";
	  gchar   str2[]="OutPutMsgFile.txt";
	   gchar   str3[]="outputTreeFile.dot";
		//~ struct 
	//~ {
			//~ gint choseType;//默认是ChoseDerivativeObj
			//~ gint degreeOutputType;//默认是ChoseOutputAddress
			//~ gint *  obAddress;
			//~ gint 	 treeDepth;
			//~ gint    treeMaxdegree;
			//~ gchar    outputMsgFile[300];
			//~ gchar    outputTreeFile[300];
			//~ gchar    obTypeName[300];
			//~ gchar    obData[300];			
		//~ }objTreeSt; 
	 //~ 
		objTreeSt.choseType=ChoseDerivativeObj;
		objTreeSt.degreeOutputType=ChoseOutputAddress;
		objTreeSt.obAddress=NULL;
		objTreeSt.treeDepth=2;
		objTreeSt.treeMaxdegree=150;
	   memcpy(objTreeSt.outputMsgFile,str2,strlen(str2)+1);
		 memcpy(objTreeSt.outputTreeFile,str3,strlen(str3)+1);
	 memcpy(objTreeSt.obTypeName,str1,strlen(str1)+1);
	  //~ memcpy(objTreeSt.obData,str1,strlen(str1)+1);
//绘制标签
	lableTexts[i] =g_strdup_printf("type chosse:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] , 2, 0);
	i++;

	lableTexts[i] =g_strdup_printf("ob address:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,2, typeChoseRadioBoxYPos+chae+i*padBetweenLable);
	i++;
	lableTexts[i] =g_strdup_printf("tree depth:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,2, typeChoseRadioBoxYPos+chae+i*padBetweenLable);
	i++;
	lableTexts[i] =g_strdup_printf("tree degree:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,2, typeChoseRadioBoxYPos+chae+i*padBetweenLable);
	i++;
	lableTexts[i] =g_strdup_printf("msg file:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,2,typeChoseRadioBoxYPos+chae+i*padBetweenLable);
	i++;
	lableTexts[i] =g_strdup_printf("tree file:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,2, typeChoseRadioBoxYPos+chae+i*padBetweenLable);
	i++;
		lableTexts[i] =g_strdup_printf("ob type name:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,sendondLie, 24);
	i++;
		lableTexts[i] =g_strdup_printf("ob  data:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,sendondLie, 54);
	i++;
			lableTexts[i] =g_strdup_printf("degree output  type:");
	lables[i] =gtk_label_new (lableTexts[i] );
	 gtk_fixed_put (GTK_FIXED (fixedInside), lables[i] ,sendondLie, 84);
	i++;
	//绘制编辑框
	i=1;
	rgba.red=(gdouble)0xFF/(gdouble)255;
	rgba.green=(gdouble)0x14/(gdouble)255;
	rgba.blue=(gdouble)0x93/(gdouble)255;
	rgba.alpha=1;
		/*view*/
						textViewForTreeCreates[i]= gtk_text_view_new ();
						textBuffForTreeCreates[i] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForTreeCreates[i]));
						gtk_text_buffer_set_text (textBuffForTreeCreates[i], "0x0000", -1);
						g_signal_connect (textBuffForTreeCreates[i], "changed",G_CALLBACK(tab_textViewForTreeCreateChanged), i);

						 gtk_text_buffer_create_tag(textBuffForTreeCreates[i], "深粉红", "foreground-rgba", &rgba, NULL);
						 gtk_text_buffer_get_start_iter (textBuffForTreeCreates[i],&start);
						 gtk_text_buffer_get_end_iter (textBuffForTreeCreates[i],&end);
						 gtk_text_buffer_apply_tag_by_name(textBuffForTreeCreates[i],"深粉红",&start,&end);
						 
						gtk_fixed_put (GTK_FIXED (fixedInside), textViewForTreeCreates[i], xpadForTextView=typeChoseRadioBoxXPos+5
																, typeChoseRadioBoxYPos+chae+i*padBetweenLable+20);
						i++;
						textViewForTreeCreates[i]= gtk_text_view_new ();
						textBuffForTreeCreates[i] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForTreeCreates[i]));
						gtk_text_buffer_set_text (textBuffForTreeCreates[i], "2", -1);
						g_signal_connect (textBuffForTreeCreates[i], "changed",G_CALLBACK(tab_textViewForTreeCreateChanged), i);

						 gtk_text_buffer_create_tag(textBuffForTreeCreates[i], "深粉红", "foreground-rgba", &rgba, NULL);
						 gtk_text_buffer_get_start_iter (textBuffForTreeCreates[i],&start);
						 gtk_text_buffer_get_end_iter (textBuffForTreeCreates[i],&end);
						 gtk_text_buffer_apply_tag_by_name(textBuffForTreeCreates[i],"深粉红",&start,&end);
						 
						gtk_fixed_put (GTK_FIXED (fixedInside), textViewForTreeCreates[i], xpadForTextView=typeChoseRadioBoxXPos+5
																, typeChoseRadioBoxYPos+chae+i*padBetweenLable+20);
						i++;	
						textViewForTreeCreates[i]= gtk_text_view_new ();
						textBuffForTreeCreates[i] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForTreeCreates[i]));
						gtk_text_buffer_set_text (textBuffForTreeCreates[i], "150", -1);
						g_signal_connect (textBuffForTreeCreates[i], "changed",G_CALLBACK(tab_textViewForTreeCreateChanged), i);

						 gtk_text_buffer_create_tag(textBuffForTreeCreates[i], "深粉红", "foreground-rgba", &rgba, NULL);
						 gtk_text_buffer_get_start_iter (textBuffForTreeCreates[i],&start);
						 gtk_text_buffer_get_end_iter (textBuffForTreeCreates[i],&end);
						 gtk_text_buffer_apply_tag_by_name(textBuffForTreeCreates[i],"深粉红",&start,&end);
						 
						gtk_fixed_put (GTK_FIXED (fixedInside), textViewForTreeCreates[i], xpadForTextView=typeChoseRadioBoxXPos+5
																, typeChoseRadioBoxYPos+chae+i*padBetweenLable+20);
						i++;					
						textViewForTreeCreates[i]= gtk_text_view_new ();
						textBuffForTreeCreates[i] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForTreeCreates[i]));
						gtk_text_buffer_set_text (textBuffForTreeCreates[i], str2, -1);
						g_signal_connect (textBuffForTreeCreates[i], "changed",G_CALLBACK(tab_textViewForTreeCreateChanged), i);

						 gtk_text_buffer_create_tag(textBuffForTreeCreates[i], "深粉红", "foreground-rgba", &rgba, NULL);
						 gtk_text_buffer_get_start_iter (textBuffForTreeCreates[i],&start);
						 gtk_text_buffer_get_end_iter (textBuffForTreeCreates[i],&end);
						 gtk_text_buffer_apply_tag_by_name(textBuffForTreeCreates[i],"深粉红",&start,&end);
						 
						gtk_fixed_put (GTK_FIXED (fixedInside), textViewForTreeCreates[i], xpadForTextView=typeChoseRadioBoxXPos+5
																, typeChoseRadioBoxYPos+chae+i*padBetweenLable+20);
						i++;			
						textViewForTreeCreates[i]= gtk_text_view_new ();
						textBuffForTreeCreates[i] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForTreeCreates[i]));
						gtk_text_buffer_set_text (textBuffForTreeCreates[i], str3, -1);
						g_signal_connect (textBuffForTreeCreates[i], "changed",G_CALLBACK(tab_textViewForTreeCreateChanged), i);

						 gtk_text_buffer_create_tag(textBuffForTreeCreates[i], "深粉红", "foreground-rgba", &rgba, NULL);
						 gtk_text_buffer_get_start_iter (textBuffForTreeCreates[i],&start);
						 gtk_text_buffer_get_end_iter (textBuffForTreeCreates[i],&end);
						 gtk_text_buffer_apply_tag_by_name(textBuffForTreeCreates[i],"深粉红",&start,&end);
						 
						gtk_fixed_put (GTK_FIXED (fixedInside), textViewForTreeCreates[i], xpadForTextView=typeChoseRadioBoxXPos+5
																, typeChoseRadioBoxYPos+chae+i*padBetweenLable+20);
						i++;		
						textViewForTreeCreates[i]= gtk_text_view_new ();
						textBuffForTreeCreates[i] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForTreeCreates[i]));
						gtk_text_buffer_set_text (textBuffForTreeCreates[i], "NULL", -1);
						g_signal_connect (textBuffForTreeCreates[i], "changed",G_CALLBACK(tab_textViewForTreeCreateChanged), i);

						 gtk_text_buffer_create_tag(textBuffForTreeCreates[i], "深粉红", "foreground-rgba", &rgba, NULL);
						 gtk_text_buffer_get_start_iter (textBuffForTreeCreates[i],&start);
						 gtk_text_buffer_get_end_iter (textBuffForTreeCreates[i],&end);
						 gtk_text_buffer_apply_tag_by_name(textBuffForTreeCreates[i],"深粉红",&start,&end);
						 
						gtk_fixed_put (GTK_FIXED (fixedInside), textViewForTreeCreates[i], sendondLie+100,24);
						i++;			
						textViewForTreeCreates[i]= gtk_text_view_new ();
						textBuffForTreeCreates[i] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewForTreeCreates[i]));
						gtk_text_buffer_set_text (textBuffForTreeCreates[i], "0x0000", -1);
						g_signal_connect (textBuffForTreeCreates[i], "changed",G_CALLBACK(tab_textViewForTreeCreateChanged), i);

						 gtk_text_buffer_create_tag(textBuffForTreeCreates[i], "深粉红", "foreground-rgba", &rgba, NULL);
						 gtk_text_buffer_get_start_iter (textBuffForTreeCreates[i],&start);
						 gtk_text_buffer_get_end_iter (textBuffForTreeCreates[i],&end);
						 gtk_text_buffer_apply_tag_by_name(textBuffForTreeCreates[i],"深粉红",&start,&end);
						 
						gtk_fixed_put (GTK_FIXED (fixedInside), textViewForTreeCreates[i], sendondLie+100,54);
						i++;																												
		//~ 创建第一个_radio_buttons
	 typeChoseRadioBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
	 gtk_box_set_homogeneous (GTK_BOX (typeChoseRadioBox), TRUE);

	 typeChoseRadio0 = gtk_radio_button_new_with_label(NULL, "搜寻衍生对象");
    typeChoseRadio1 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (typeChoseRadio0), "搜寻基类对象");
    
   gtk_box_pack_start (GTK_BOX (typeChoseRadioBox), typeChoseRadio0, TRUE, TRUE, 2); 
   gtk_box_pack_start (GTK_BOX (typeChoseRadioBox), typeChoseRadio1, TRUE, TRUE, 2);
   
    g_signal_connect (typeChoseRadio0,"toggled",G_CALLBACK(typeChoseRadiotoggledHandle), "typeChoseRadio0");
    g_signal_connect (typeChoseRadio1,"toggled",G_CALLBACK(typeChoseRadiotoggledHandle), "typeChoseRadio1");
    
	gtk_widget_set_size_request (typeChoseRadioBox, 20, 20);
	gtk_fixed_put (GTK_FIXED (fixedInside), typeChoseRadioBox, typeChoseRadioBoxXPos, typeChoseRadioBoxYPos);
	
		//~ 创建第2个_radio_buttons  outputType
	 outputTypeRadioBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
	 gtk_box_set_homogeneous (GTK_BOX (outputTypeRadioBox), TRUE);

	outputTypeRadio0 = gtk_radio_button_new_with_label(NULL, "adress");
    outputTypeRadio1 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (outputTypeRadio0), "type");
    outputTypeRadio2 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (outputTypeRadio0), "data");
   gtk_box_pack_start (GTK_BOX (outputTypeRadioBox), outputTypeRadio0, TRUE, TRUE, 2); 
   gtk_box_pack_start (GTK_BOX (outputTypeRadioBox), outputTypeRadio1, TRUE, TRUE, 2);
   gtk_box_pack_start (GTK_BOX (outputTypeRadioBox), outputTypeRadio2, TRUE, TRUE, 2);
   
    g_signal_connect (outputTypeRadio0,"toggled",G_CALLBACK(outputTypeRadiotoggledHandle), "outputTypeRadio0");
    g_signal_connect (outputTypeRadio1,"toggled",G_CALLBACK(outputTypeRadiotoggledHandle), "outputTypeRadio1");
    g_signal_connect (outputTypeRadio2,"toggled",G_CALLBACK(outputTypeRadiotoggledHandle), "outputTypeRadio2");
    
	gtk_widget_set_size_request (outputTypeRadioBox, 20, 20);
	gtk_fixed_put (GTK_FIXED (fixedInside), outputTypeRadioBox, sendondLie+100, 110);	
	
	//按钮

	text = g_strdup_printf("ToCreateTree");
	buttoms[buttomID]=gtk_button_new_with_label(text);
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(ToCreateTreeButtonClicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixedInside), buttoms[buttomID], 290, 320);
	buttomID++;
}

void createCreateNeroTab(GtkWidget *fixedInside)
{
	gchar *text;
	gint buttomID=1;
	GtkWidget * buttoms[buttomNum];
	GtkWidget *vbox = gtk_box_new(FALSE, 5);/*存放其他功能构建的容器*/;



	text = g_strdup_printf("ToCreateNero");
	buttoms[buttomID]=gtk_button_new_with_label(text);
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(CreateNeroNetWork), NULL);
	gtk_container_add(GTK_CONTAINER(vbox), buttoms[buttomID]);


	gtk_widget_set_size_request (vbox, 15, 15);
	gtk_fixed_put (GTK_FIXED (fixedInside), vbox, 0, 0);


}
void createTab1_InMainWindow(GtkWidget * window,gint count,GtkWidget *notebook)
{
	gint x=50;
	gint y=50;

	static gint counts=0;
	gchar *text;
	counts++;

	gtk_window_get_size(GTK_WINDOW(window),&x,&y);
	GtkWidget *fixedInside = gtk_fixed_new();/*tab里面的容器*/
	gtk_widget_set_size_request (fixedInside, x/3, y/2);


	#ifdef _Nero_DeBuging1_cacel_
	switch(counts)
	{
	case 2:text = g_strdup_printf("createNero");createCreateNeroTab( fixedInside);break;
	case 3:
		text = g_strdup_printf("信息查询");
		createMsgSearchTab( fixedInside);
		textViewForSearchFixedInsideBox=fixedInside;
		break;
	case 1:text = g_strdup_printf("tools");createToolsTab(fixedInside);break;
	case 4:text = g_strdup_printf("createNeroTree");createNeroTreeTab(fixedInside);break;
	default:break;

	}
	#else
	switch(counts)
	{
	case 4:text = g_strdup_printf("createNero");createCreateNeroTab( fixedInside);break;
	case 2:text = g_strdup_printf("信息查询");createMsgSearchTab( fixedInside);
	textViewForSearchFixedInsideBox=fixedInside;break;
	case 3:text = g_strdup_printf("tools");createToolsTab(fixedInside);break;
	case 1:text = g_strdup_printf("createNeroTree");createNeroTreeTab(fixedInside);break;
	default:break;

	}
	#endif
	GtkWidget *label = gtk_label_new(text);/*tab的名字*/



	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), fixedInside, label);


}






void app_set_theme(const gchar *theme_path)
{
    static GtkCssProvider *provider = NULL;
    GFile *file;
    GdkScreen *screen;
    screen = gdk_screen_get_default();
    if(theme_path!=NULL)
    {
        file = g_file_new_for_path(theme_path);
        g_printf("Loading GTK3 CSS File: %s\n", theme_path);
        if(file!=NULL)
        {
            if(provider==NULL)
                provider = gtk_css_provider_new();
            gtk_css_provider_load_from_file(provider, file, NULL);
            gtk_style_context_add_provider_for_screen(screen,
                GTK_STYLE_PROVIDER(provider),
                GTK_STYLE_PROVIDER_PRIORITY_USER);
            gtk_style_context_reset_widgets(screen);
            g_printf("Loaded GTK3 CSS File Successfully!\n");
        }
    }
    else
    {
        if(provider!=NULL)
        {
            gtk_style_context_remove_provider_for_screen(screen,
                GTK_STYLE_PROVIDER(provider));
            g_object_unref(provider);
            provider = NULL;
        }
        gtk_style_context_reset_widgets(screen);
    }
}


enum {
  REGION_ENTRY,
  REGION_BUTTON_UP,
  REGION_BUTTON_DOWN
};


gboolean spin_myWidget_draw (GtkWidget *widget, cairo_t   *cr)
{
  GtkStyleContext *context;
	GdkRGBA  rgba;

	 rgba.red=(gdouble)0xFF/(gdouble)255;/*FF69B4*/
	 rgba.green=(gdouble)0x14/(gdouble)255;/*DeepPink FF93*/
	 rgba.blue=(gdouble)0x93/(gdouble)255;
	 rgba.alpha=1;


/*  context = gtk_widget_get_style_context (widget);*/

/*  gtk_style_context_push_animatable_region (context, GUINT_TO_POINTER (REGION_ENTRY));*/

/*  gtk_render_background (context,cr, 0, 0, 100, 30);*/
/*  gtk_render_frame (context,cr, 0, 0, 100, 30);*/

/*  gtk_style_context_pop_animatable_region (context);*/


gtk_widget_override_background_color(widget,GTK_STATE_FLAG_NORMAL,&rgba);

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






	#ifdef  Nero_DeBuging20_12_13
	void **DataFlow;
	nero_s32int *dataKind;
	Utf8Word  *wP;
	char *linc;
	nero_s32int dataNum,k,countOfWord,m;
	readUTF8FileForWords("data/词库" ,& wordsHead);
/*	readUTF8FileForWords("data/现代汉语常用词汇表utf8.txt" ,& wordsHead);*/
	/*将Utf8Word转化为一个数组，每个单位是一个词*/
		wP=wordsHead.next;
		countOfWord=0;
		while (wP)
		{
/*		printf("wP->num=%d.\n",wP->num);*/
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










