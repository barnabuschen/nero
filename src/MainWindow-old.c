#ifndef   FILENAME
#define   FILENAME
#include "common/type.h"
#endif

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

#include "common/Neuron.h"
#include "MainWindow.h"
#include "common/fileOperating.h"
#include "graph/bmpRead.h"
#include <cairo.h>

	gint x=50;
	gint y=50;
extern char *titlebmp2;
extern void printBmpMsg(PbmpHead head,PbmpMsg msg);
extern void getBmpMsg(char * FileName,PbmpHead head,PbmpMsg msg);
extern unsigned char * readBmpData(char * FileName,unsigned int * dataCount,bmpHead * head,bmpMsg * msg);
extern char * getLineInFile(char * FileName,int num);
extern void putBMPInZheng(unsigned char * newbmpdata,int w,int h);
extern unsigned char * putBMPRGBtoRGBA(unsigned char * bmpdata,int w,int h);
extern  void putBMPRGBtoBGR(unsigned char * bmpdata,int w,int h);
extern unsigned char * addLieToBMP(unsigned char * bmpdata,int w,int h,int lie);
extern unsigned char * ChangeRGBToRGBA(unsigned char * bmpdata,int w,int h,int *newW,int ifPutInZheng);
extern int getstrideOfBMP(int w);
extern void scanBMP(unsigned char * bmpdata,int w,int l);
extern void *  ReadNeuronData(NeoF * f,char * fileName,int *Filefd);
extern void addNeoToFile(NeoF * f,char * fileName,int  addNeoNUM);
/*extern int  CreateNewNeuronFile(NeoF * f,char * fileName);*/
extern void printNeuronFileMsg(NeoF * f);
extern int CreateLineManFromPoses(int PosCount,POS * p,LineMan *manAllLine);
extern int InitialLineManNode(LineMan *Node);
extern int  printListCnt(struct list_head  *cnt);
extern int  printPosList(struct list_head *headOfPosList);
extern void printAllLineIInPos(LineMan *manAllLineIInPos);
extern int   countNode(struct list_head  *cnt);
extern int InitiaNeuronMan(char * fileName,NeuronMan * nm);
extern int ReadDataNeuronMan(char * fileName,NeuronMan * nm);
extern int munmapNeuronMan(NeuronMan * nm);
extern int printNeuronManData(NeuronMan * nm);
extern int addNeoByLineMan(NeuronMan * nm,LineMan *manAllLine);
extern int GetLineManByNeo(int imageIndex,NeuronMan * nm,LineMan *manAllLine);
extern int findImageNode(NeuronMan * nm,int *index);



void DrawPictureWithLineManInVectorN(GtkWidget *darea,LineMan *manAllLine,int showLine)
{

	if(darea == NULL || manAllLine ==NULL || showLine<1)
	{
		printf("DrawPictureWithLineManInVectorN22 error  \n");
		return;
	}
/*printf("DrawPictureWithLineMan here1  \n");*/
	//首先清空画板
	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));
	double cccc=0x00;
	cairo_set_source_rgb(cr,cccc,cccc,cccc);
	cairo_paint (cr);
	//现在开始画线条
	if(cr == NULL)
	{
		printf("cairo_t error  \n");		return;
	}
	cairo_set_source_rgb(cr, 0xff, 0, 0);
	cairo_set_line_width(cr, 0.5);
	int coloutCount=manAllLine->count;
/*	printf("coloutCount  =%d \n",coloutCount);*/
	if(coloutCount>0)
	{	int i=0,j=0;
		PosList * curr;
		PosList * next;
		PosList *currList;
		LineMan *currLineManNode;
		for (i = 0; i < (coloutCount); i += 1)
		{
			if(i >(showLine-1))
			{
				break;
			}
			
			currLineManNode=(LineMan *)list_GetNUMn(&(manAllLine->p),(i+1));
			currList=currLineManNode->posHead;
			if(currList == manAllLine)
			{
				printf("list_GetNUMn error  \n");	
				break;
			}
			
			int posCount= countNode(&(currList->p));
/*			printf("posCount  =%d \n",posCount);*/
			POS currentPOS;
			currentPOS.x=0;
			currentPOS.y=0;
			for (j = 0; j < (posCount-1); j += 1)
			{
				  curr=(PosList *)list_GetNUMn(&(currList->p),(j+1));
				  next=(PosList *)list_GetNUMn(&(currList->p),(j+2));
				currentPOS.x+=((curr->pos).x);
				currentPOS.y+=((curr->pos).y);

					cairo_move_to(cr, (currentPOS.x),(currentPOS.y));
					cairo_rel_line_to(cr,((next->pos).x), ((next->pos).y));

/*				printf("	curr-=(%d,%d),next=(%d,%d)\n",((curr->pos).x), ((curr->pos).y),  ((next->pos).x),((next->pos).y));			*/
			}
		}
	}
	 cairo_stroke(cr); 

	cairo_destroy (cr);
	recoverPic=4;




}
void DrawPictureWithLineManInVector(GtkWidget *darea,LineMan *manAllLine)
{
	if(darea == NULL || manAllLine ==NULL)
	{
		printf("DrawPictureWithLineMan error  \n");
		return;
	}
/*printf("DrawPictureWithLineMan here1  \n");*/
	//首先清空画板
	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));
	double cccc=0x00;
	cairo_set_source_rgb(cr,cccc,cccc,cccc);
	cairo_paint (cr);
	//现在开始画线条
	if(cr == NULL)
	{
		printf("cairo_t error  \n");		return;
	}
	cairo_set_source_rgb(cr, 0xff, 0, 0);
	cairo_set_line_width(cr, 0.5);
	int coloutCount=manAllLine->count;
/*	printf("coloutCount  =%d \n",coloutCount);*/
	if(coloutCount>0)
	{	int i=0,j=0;
		PosList * curr;
		PosList * next;
		PosList *currList;
		LineMan *currLineManNode;
		for (i = 0; i < (coloutCount); i += 1)
		{

			currLineManNode=(LineMan *)list_GetNUMn(&(manAllLine->p),(i+1));
			currList=currLineManNode->posHead;
			if(currList == manAllLine)
			{
				printf("list_GetNUMn error  \n");	
				break;
			}
			
			int posCount= countNode(&(currList->p));
/*			printf("posCount  =%d \n",posCount);*/
			POS currentPOS;
			currentPOS.x=0;
			currentPOS.y=0;
			for (j = 0; j < (posCount-1); j += 1)
			{
				  curr=(PosList *)list_GetNUMn(&(currList->p),(j+1));
				  next=(PosList *)list_GetNUMn(&(currList->p),(j+2));
				currentPOS.x+=((curr->pos).x);
				currentPOS.y+=((curr->pos).y);

					cairo_move_to(cr, (currentPOS.x),(currentPOS.y));
					cairo_rel_line_to(cr,((next->pos).x), ((next->pos).y));

/*				printf("	curr-=(%d,%d),next=(%d,%d)\n",((curr->pos).x), ((curr->pos).y),  ((next->pos).x),((next->pos).y));			*/
			}
		}
	}
	 cairo_stroke(cr); 

	cairo_destroy (cr);
	recoverPic=4;
}
void DrawPictureWithLineMan(GtkWidget *darea,LineMan *manAllLine)
{
	if(darea == NULL || manAllLine ==NULL)
	{
		printf("DrawPictureWithLineMan error  \n");
		return;
	}
printf("DrawPictureWithLineMan here1  \n");
	//首先清空画板
	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));
	double cccc=0x00;
	cairo_set_source_rgb(cr,cccc,cccc,cccc);
	cairo_paint (cr);
	//现在开始画线条
	if(cr == NULL)
	{
		printf("cairo_t error  \n");		return;
	}
	cairo_set_source_rgb(cr, 0xff, 0, 0);
	cairo_set_line_width(cr, 0.5);
	int coloutCount=manAllLine->count;
	printf("coloutCount  =%d \n",coloutCount);
	
	if(coloutCount>0)
	{	int i=0,j=0;
		PosList * curr;
		PosList * next;
		PosList *currList;
		LineMan *currLineManNode;
		for (i = 0; i < (coloutCount-1); i += 1)
		{

			currLineManNode=(LineMan *)list_GetNUMn(&(manAllLine->p),(i+1));
			currList=currLineManNode->posHead;
			if(currList == manAllLine)
			{
				printf("list_GetNUMn error  \n");	
				break;
			}
			
			int posCount= countNode(&(currList->p));
			printf("posCount  =%d \n",posCount);
			for (j = 0; j < (posCount-1); j += 1)
			{
			  curr=(PosList *)list_GetNUMn(&(currList->p),(j+1));
			  next=(PosList *)list_GetNUMn(&(currList->p),(j+2));

				  cairo_move_to(cr, (double)((curr->pos).x), (double)((curr->pos).y));
				  cairo_line_to(cr, (double)((next->pos).x), (double)((next->pos).y));
		/*		  cairo_rel_move_to(cr, (double)curr->x, (double)curr->y);*/
		/*		  cairo_rel_line_to(cr, (double)curr->x, (double)curr->y);*/
	/*			printf("	curr-=(%d,%d),next=(%d,%d)\n",curr->x, curr->y,next->x,next->y);			*/
			}
 
		}
	}
	 cairo_stroke(cr); 

	cairo_destroy (cr);
	recoverPic=3;
}
void DrawPictureWithPoints(GtkWidget *darea,int coloutCount,POS * p)
{

	if(darea == NULL || p ==NULL)
	{
		printf("par error  \n");
		return;
	}
	//首先清空画板
	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));
/*	cairo_t *cr =currentCair;*/
/*	if(cr)*/
/*		cairo_destroy (cr);*/
/*	g_return_if_fail(ccr != NULL);*/
	double cccc=0x00;
	cairo_set_source_rgb(cr,cccc,cccc,cccc);
	cairo_paint (cr);
 

	//现在开始画线条
	if(cr == NULL)
	{
		printf("cairo_t error  \n");		return;
	}
	
	cairo_set_source_rgb(cr, 0xff, 0, 0);
	cairo_set_line_width(cr, 0.5);

	printf("coloutCount  =%d \n",coloutCount);
/*	cairo_move_to(cr, 40, 40);*/
	if(coloutCount>0)
	{	int i=0;
		POS * curr;
		POS * next;
/*		curr=( p +sizeof(POS)*i  );*/
/*		curr=( p +sizeof(POS)*i  );*/
/*		cairo_move_to(cr, (double)curr->x, (double)curr->y);*/
/*		coloutCount++;	*/
		for (i = 0; i < (coloutCount-1); i += 1)
		{
		  curr=( p +i  );//cairo_rel_line_to ()
		  next=( p +(i+1)  );//cairo_rel_line_to ()
		if(curr->x <0  || curr->y < 0 || next->x <0  || next->y < 0 )
		{
			continue;
		}
		
			  cairo_move_to(cr, (double)curr->x, (double)curr->y);
			  cairo_line_to(cr, (double)next->x, (double)next->y);
	/*		  cairo_rel_move_to(cr, (double)curr->x, (double)curr->y);*/
	/*		  cairo_rel_line_to(cr, (double)curr->x, (double)curr->y);*/
			printf("	curr-=(%d,%d),next=(%d,%d)\n",curr->x, curr->y,next->x,next->y); 
		}
	}


/*	SavecurrentCair=cairo_reference(cr);*/
	 cairo_stroke(cr); 
/*	cairo_save(cr); */
/*	cairo_paint (cr);*/
	cairo_destroy (cr);
	recoverPic=2;

}
void SubP(char * File1)
{
	int flagOfFile=0;
	#ifndef NoIncludeG_return_if_fail 
/*	g_return_if_fail(File1 != NULL);*/
	#endif
/*	char * sevean=getLineInFile("Data/config",1);*/
	char * sevean=File1;
	printf("SubP File is %s\n", File1);
	unsigned int  dataCount;
	bmpHead head;
	bmpMsg msg;
	unsigned char * bmpdata;
	int stride;
	if(File1)
	{
		flagOfFile=0;
		bmpdata=readBmpData(sevean,&dataCount,&head,&msg);
		printBmpMsg((PbmpHead)&head,(PbmpMsg)&msg);
		stride= getstrideOfBMP( msg.biWidth);
		#ifndef NoIncludeG_return_if_fail 
		g_return_if_fail(msg.biBitCount == 24);
		#endif
	}
	else
	{
		flagOfFile=1;
		bmpdata=&((*(pMapWithCandy)).data);
		msg.biHeight=(*(pMapWithCandy)).height;
		msg.biWidth=(*(pMapWithCandy)).width;
		stride=(*(pMapWithCandy)).widthStep;
/*		dataCount=(*(pMapWithCandy)).imageSize;*/

	}



	int newW;
///////////////////////
	if(flagOfFile == 0)
	{
			putBMPInZheng(bmpdata,stride,msg.biHeight);
	}
	

	//现在可以开始分析了
	//现在的数据是正立的，BGR数据
/*	scanBMP(bmpdata,msg.biWidth,msg.biHeight);*/




///////////////////////
	unsigned char * newda=ChangeRGBToRGBA(bmpdata,msg.biWidth,msg.biHeight,&newW,0);

///	画之前需要先将原先的内容清楚
	if(image)
		free(image);
	currentCair = gdk_cairo_create (gtk_widget_get_window(darea));
/*	cairo_t *ccr =currentCair;*/

/*	g_return_if_fail(ccr != NULL);*/
	double cccc=0x00;
/*	currentCair=*SavecurrentCair;*/
	cairo_set_source_rgb(currentCair,cccc,cccc,cccc);
	cairo_paint (currentCair);

////
	image = cairo_image_surface_create_for_data ((unsigned char *) newda,CAIRO_FORMAT_RGB24,newW,msg.biHeight,newW*4);
/*	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));*/
/*	g_return_if_fail(image != NULL);*/
	currentCair = gdk_cairo_create (gtk_widget_get_window(darea));
/*	g_return_if_fail(ccr != NULL);*/
	cairo_set_source_surface(currentCair, image, 40, 20);
	cairo_paint (currentCair);
		cairo_destroy (currentCair);
	recoverPic=1;

}
void *thread_for_paint_Pic(void *arg)
{
	
	int x=0;
	int timeToWaitCandy=0;
	char *fileName=NULL;
/*		struct { long type; char text[100]; } mymsg;*/
	while(x == 0)
	{
/*		sleep(10);*/
/*		printf("wait for msg......\n");*/
	        key_t ipckey;
		int mq_id;

		int received;

		/* Generate the ipc key */
		ipckey = ftok(picShowSendToMain, 43);
		printf("My key is %d\n", ipckey);

		/* Set up the message queue */
		mq_id = msgget(ipckey,0);// IPC_CREAT
		printf("pictureDisplay Message identifier is %d\n", mq_id);

		received = msgrcv(mq_id, &mymsg, sizeof(mymsg), 0, 0);

		printf("%s (%d)\n", mymsg.text, received);		
/*		if()*/
			
		fileName=mymsg.text;

		if(fileName && 0 < received)
			{
			printf("get Message from show pic is %s\n", fileName);
			SendMsgToCandy(fileName);
			while(x == 0)
			{
				sleep(1);
				timeToWaitCandy++;
				int   newFlag=(*(pMapWithCandy)).flag;
				if(newFlag == 1)
				{
					printf("newFlag updata=%d \n",newFlag);
/*					SubP(NULL);*/
					printPos();
					(*(pMapWithCandy)).flag=0;
					break;
				}
				else   printf("wrong newFlag =%d \n",newFlag);
				if(timeToWaitCandy >5)
				{
					timeToWaitCandy=0;
					(*(pMapWithCandy)).flag=0;
					break;
				}
				
			}
/*			SubP(fileName);*/
			}
/*		if(fileName)*/
/*			free(fileName);*/
		fileName=NULL;
	}
	printf("end to wait msg................................\n");
}
void printPos()
{

/*	memcpy( (&((*(pMapWithCandy)).data)+sizeof(CvPoint)*coloutCount),&pt,sizeof(CvPoint));*/
	int coloutCount=pMapWithCandy->nSize;  
	printf("coloutCount  =%d \n",coloutCount);
	if(coloutCount>0)
	{	int i;
		POS * p;
		for (i = 0; i < coloutCount; i += 1)
		{
			//&(  (*pMapWithCandy).data);
			p= ( ( &(pMapWithCandy->data) ) +sizeof(POS)*i  );
/*			printf("	p.x=%d,p.y=%d\n",p->x,p->y);  */
		}
	}

	//创建manAllLine
/*	DrawPictureWithPoints(darea,coloutCount,(POS *) ( &(pMapWithCandy->data) )  );*/
	LineMan *manAllLine=malloc(sizeof(LineMan));
	InitialLineManNode(manAllLine);
	int res=CreateLineManFromPoses(coloutCount,(POS *) ( &(pMapWithCandy->data) ), manAllLine);
	//尝试打印manAllLine中的点
	int i;
	if(res >0)
	{
/*		 printAllLineIInPos(manAllLine);*/
	}
	else printf("no line	\n");


/*	DrawPictureWithLineMan(darea,manAllLine);*/

	//将图像加入网络中
	////////////////////////////
	char * fileName="NeuronData/NeuronData1";
	NeuronMan  nm;
/*	 InitiaNeuronMan( fileName, & nm);*/
	ReadDataNeuronMan(fileName,&nm);
	res=addNeoByLineMan(&nm,manAllLine);///////////
	if(res >0)
	{
/*		printNeuronManData(& nm);*/
	}
	else  printf("addNeoByLineMan fail  res=%d	\n",res);
	
	//现在可以从网络中读取一个图片的数据了--方式是给点一个这个图片节点的序号	
	manAllLineFromNeo=malloc(sizeof(LineMan));
	

	int index[13];
	index[0]=0;
	res= findImageNode( &nm,index);
	for (i = 1; i <= index[0]; i += 1)
	{
		InitialLineManNode( manAllLineFromNeo);
		GetLineManByNeo(index[i],&nm,manAllLineFromNeo);
		DrawPictureWithLineManInVector(darea,manAllLineFromNeo);
		sleep(3);
	}

/*	printAllLineIInPos(manAllLineFromNeo);*/
	
	munmapNeuronMan(&nm);
}
void hello( GtkWidget *widget, gpointer data )
{

	char * sevean=getLineInFile("Data/config",1);
	 SubP(sevean);
	int res;
	pthread_t a_thread;

	res = pthread_create(&a_thread, NULL,thread_for_paint_Pic, NULL);

}
void drow1( GtkWidget *widget, gpointer data )
{
	static int showId=0;
	static int showLineCount=0;
	showId=1;
	showLineCount++;
	char * fileName="NeuronData/NeuronData1";
	NeuronMan  nm;
/*	 InitiaNeuronMan( fileName, & nm);*/
	ReadDataNeuronMan(fileName,&nm);
	int res,i;
	//现在可以从网络中读取一个图片的数据了--方式是给点一个这个图片节点的序号	
	manAllLineFromNeo=malloc(sizeof(LineMan));
	

	int index[13];
	index[0]=0;
	res= findImageNode( &nm,index);

	
	for (i = 1; i <= index[0]; i += 1)
	{
		if(showId ==i )
		{

		InitialLineManNode( manAllLineFromNeo);
		GetLineManByNeo(index[i],&nm,manAllLineFromNeo);
		if(showLineCount >(manAllLineFromNeo->count))
		{
			showLineCount=1;
		}
		printf("showId=%d,showLineCount=%d,count=%d\n", showId,showLineCount,(manAllLineFromNeo->count));
/*		DrawPictureWithLineManInVector(darea,manAllLineFromNeo);*/
		DrawPictureWithLineManInVectorN(darea,manAllLineFromNeo,showLineCount);
/*		sleep(1);			*/
		}


	}
	munmapNeuronMan(&nm);

}

void SendMsgToCandy(char * fileName)
{
	printf("\n");
	static i=0;
	if(fileName)
	{
		/* Send a message */
		memset(msgForCandy.text, 0, 100);  //Clear out the space 
		strcpy(msgForCandy.text,fileName );//newfilename就是发送的字符串
		msgForCandy.type = 1;
		printf("msgForCandy.text is :%s\n", msgForCandy.text);
		int res=msgsnd(mqIdForCandy, &msgForCandy, sizeof(msgForCandy), 0);
		printf("msgsnd to candy res is :%d\n", res);


	//share space with candy
		int   newFlag=1;
/*		memcpy(  (*(pMapWithCandy)).data,xxx,(strlen(xxx)+1));*/
		(*(pMapWithCandy)).flag=0;

	}
}
void drow2( GtkWidget *widget, gpointer data )
{
	char * fileName="NeuronData/NeuronData1";

	NeuronMan  nm;
	 InitiaNeuronMan( fileName, & nm);
	ReadDataNeuronMan(fileName,&nm);

/*	 printNeuronManData(& nm);*/
	 munmapNeuronMan(&nm);
}
gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{

		if(recoverPic ==4)
			{
	DrawPictureWithLineManInVector(darea,manAllLineFromNeo);
			}

	return FALSE;
}
void ProInitialization()
{	
/*	currentCair = gdk_cairo_create (gtk_widget_get_window(darea));*/
/*	SavecurrentCair=*currentCair;*/
	



	//send msg  to candy
		ipckeyForCandy = ftok(SendMsgWithCandy, 43);
		printf(" ipckeyForCandy is %d\n",ipckeyForCandy);

		/* Set up the message queue */
		mqIdForCandy = msgget(ipckeyForCandy, IPC_CREAT | 0666);
		printf("mqIdForCandy is %d\n", mqIdForCandy);




//share space with candy
	keyWithCandy = ftok(nameWithCandy,0);
	if(keyWithCandy==-1)
		perror("ftok error");
	shmIdWithCandy=shmget(keyWithCandy,tryShm,IPC_CREAT | 0666);
        printf("My keyWithCandy is %d\n", keyWithCandy);
        printf("My shmIdWithCandy is %d\n",shmIdWithCandy);	
	if(shmIdWithCandy==-1)
	{
		perror("shmget error");
		return;
	}
	pMapWithCandy=(shareData*)shmat(shmIdWithCandy,NULL,0);
	printf("pMapWithCandy  is %x\n", pMapWithCandy);



}
int c=0;
/*void mouseEnter( GtkWidget *widget, gpointer data )*/
/*{*/
gint mouseEnter( GtkWidget *widget, GdkEvent *event, gpointer data )
{

/*	GdkEventMotion *e=(GdkEventMotion *)event;*/
/*	gint x=e->x;*/
/*	gint y=e->y;*/


/*	GdkDevice * dev=gdk_event_get_source_device(event);*/
/*	GdkWindow *   window=gdk_device_get_window_at_position(dev,&xx,&yy);*/
/*	if(window )*/
/*	  gdk_window_get_device_position(GTK_WINDOW(window),dev,&x,&y,NULL);*/
/*	else*/
/*		printf("is null\n");*/
/*	printf("sss x=%d ,y=%d\n",x,y);*/

}
void frame_callback(GtkWindow *window, GdkEvent *event, gpointer data)
{

		if(recoverPic ==1)
			{
/*cairo_t  * currentCair=gdk_cairo_create (gtk_widget_get_window(darea));*/
/*	cairo_set_source_surface(currentCair, image, 40, 20);*/
/*	cairo_paint (currentCair);*/
			}
		else
		if(recoverPic ==4)
			{
/*	DrawPictureWithLineManInVector(darea,manAllLineFromNeo);*/
			}
/*		recoverPic=0;*/
}
void myMainWindow(GtkWidget *window)
{
	recoverPic=0;


	g_signal_connect (window,"delete-event",G_CALLBACK (delete_eventInMainWin),NULL);
	g_signal_connect(G_OBJECT(window), "configure-event",G_CALLBACK(frame_callback), NULL);
/*	g_signal_connect(window,"motion-notify-event",G_CALLBACK (delete_event),NULL)*/
	gtk_widget_add_events (window, GDK_POINTER_MOTION_MASK);
	g_signal_connect (window, "motion-notify-event",G_CALLBACK(mouseEnter), NULL);		

	//gint i=1;
	int rows=2;
	int columns=1;
	//int buttomNum=4;
	int buttomID=0;
	gint xpadding=10;
	gint ypadding=10;

	table=gtk_table_new(rows,columns,FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table),xpadding);
	gtk_table_set_col_spacings(GTK_TABLE(table),ypadding);
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
	buttoms[buttomID]=gtk_button_new_with_label("clear neo");
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(drow2), NULL);
	//gtk_table_attach_defaults(GTK_TABLE(table),buttoms[0],0,1,0,1);
	//gtk_widget_set_usize(buttoms[buttomID],40,40);
	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 160, 0);
	gtk_widget_show (buttoms[buttomID]);	

	 darea = gtk_drawing_area_new();
         gtk_fixed_put (GTK_FIXED (fixed),darea, 0, 30);
	g_signal_connect (G_OBJECT (darea), "draw",G_CALLBACK (draw_callback), NULL);
	gtk_window_get_size(GTK_WINDOW(window),&x,&y);
	 gtk_widget_set_size_request (darea, x*2/3, y);
	
	int dareaForComperID=0;
	dareaForComper[dareaForComperID]= gtk_drawing_area_new();;
         gtk_fixed_put (GTK_FIXED (fixed),dareaForComper[dareaForComperID], x*2/3, 30);
/*	g_signal_connect (G_OBJECT (dareaForComper[dareaForComperID]), "draw",G_CALLBACK (draw_callback), NULL);*/
	gtk_window_get_size(GTK_WINDOW(window),&x,&y);
	 gtk_widget_set_size_request (dareaForComper[dareaForComperID], x*2/3, y);
	gtk_widget_show (dareaForComper[dareaForComperID]);	


	gtk_widget_show (darea);
	gtk_widget_show (fixed);
	gtk_widget_show_all(window);
	//工程信息初始化
	ProInitialization();
}
void printDAta(unsigned char * bmpdata,int w,int h)
{
	
	int i=0;
	int pos=0;
printf("w is%d \n",w);
	while(pos < w*h)
	{
		
		printf("%2x ",bmpdata[pos++]);
		i++;
		if(i == w)
		{
			printf("\n");
			i=0;
		}
	}



}
void paintInSurfaceRGB24(cairo_surface_t *image,int weight,int height,unsigned char * bmpdata)
{
/*	g_return_if_fail(bmpdata != NULL);*/
	//_return_if_fail(image != NULL);

/*        int i=0;*/
/*        int hangInData=0;*/
        //首先找到让msg.biWidth * 3 +i能被4整除的数
/*        i=0;*/
/*        while((weight * 3 +i)%4 != 0)*/
/*                i++;*/
/*        hangInData=i+weight * 3;*/
/*        int rubbish=i;*/
	//图像转置
		int stride=weight*4;
	putBMPInZheng(bmpdata,stride,height);


	
	image=cairo_image_surface_create_for_data (bmpdata,CAIRO_FORMAT_RGB24,stride/4,height,stride);

/*	g_return_if_fail(image != NULL);*/
	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));
/*	g_return_if_fail(cr != NULL);*/
	cairo_set_source_surface(cr, image, 40, 20);
	cairo_paint (cr);
	cairo_destroy (cr);

}
void paintInRGB24(char * filename)//数据和像素大小
{
	unsigned int  dataCount;
	bmpHead head;
	bmpMsg msg;
	unsigned char * bmpdata=readBmpData(filename,&dataCount,&head,&msg);
	printBmpMsg(&head,&msg);
	g_return_if_fail(msg.biBitCount == 24);

	//扩展为w可以被4整除
	int i=0;
	int w=msg.biWidth;
	int l=msg.biHeight;
        while((w * 3 +i)%4 != 0)
                i++;
	unsigned char * newbmpdata2=addLieToBMP( bmpdata, w * 3 +i,l,i*2);
	w+=i;
		//尝试转化为RGBA
/*	int newstride=w*3;*/
	unsigned char * newbmpdata =putBMPRGBtoRGBA( newbmpdata2, w,l);







	paintInSurfaceRGB24(image,w,l, newbmpdata);

	if(bmpdata)
		free(bmpdata);
	if(newbmpdata2)
		free(newbmpdata2);
	if(newbmpdata)
		free(newbmpdata);


}

gint delete_eventInMainWin( GtkWidget *widget, GdkEvent *event, gpointer data )
{

	if(shmdt(pMapWithCandy)==-1)
		perror(" detach error ");
	else 
		g_print(" detach ok \n");


		
	g_print ("delete event occurred in main\n");
	gtk_main_quit();

/* 将TRUE改为FALSE,主窗口就会用一个 " d e l e t e _ e v e n t "信号,然后退出 */
	return(TRUE);
}



