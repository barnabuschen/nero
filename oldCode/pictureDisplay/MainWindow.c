/*#ifndef   FILENAME*/
/*#define   FILENAME*/
/*#include "common/type.h"*/
/*#endif*/
//#define GTK_ENABLE_BROKEN


#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>


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
/*#ifdef ComlourSimilar*/
#include "MainWindow.h"
/*#endif*/
#include "common/fileOperating.h"
#include "graph/bmpRead.h"
#include <cairo.h>
extern int addLineToFile(char * FileName,char * str);
extern int delLine(char * FileName,int line);
extern void getBmpMsg(char * FileName,PbmpHead head,PbmpMsg msg);
extern  int    putStrInFile(char * FileName,char * str,int len,int point);
extern int addLineToFile(char * FileName,char * str);
extern void emptyFile(char * fileName);
	gint x=50;
	gint y=50;

void hello( GtkWidget *widget, gpointer data )
{
///////////////////////////////////////
	char name[500];
	struct dirent *dp;
	DIR *dfd;
	char *dir="../era/Data";
	if ((dfd = opendir(dir)) == NULL) {
		fprintf(stderr, "dirwalk: can't open %s\n", dir);
		return;
	}	
	while ((dp = readdir(dfd)) != NULL)
	{
		if (strcmp(dp->d_name, ".") == 0
		    || strcmp(dp->d_name, "..") == 0)
			continue;    /* skip self and parent */
		if (strlen(dir)+strlen(dp->d_name)+2 > sizeof(name))
			fprintf(stderr, "dirwalk: name %s %s too long\n",dir, dp->d_name);
		else
		 {
/*			int len=sizeof(dp->d_name);*/
/*		if((dp->d_name)[len-1] !='p' || (dp->d_name)[len-2] !='m' || (dp->d_name)[len-1] !='b')*/
		if((dp->d_name)[0]== 'c')
		{
/*			printf("len=%d,1=%c\n",len,(dp->d_name)[1]);*/
			continue;			
		}
			filenameList[correntPicture++]=malloc(sizeof(char)*(24+sizeof(dp->d_name)));
			bmpHead head;
			bmpMsg msg;
			(filenameList[correntPicture-1])[0]='\0';
			strcat(filenameList[correntPicture-1],"../era/Data/");
			strcat(filenameList[correntPicture-1]+12,dp->d_name);

			getBmpMsg(filenameList[correntPicture-1],& head,& msg);
/*			if(msg.biBitCount != 24)*/
/*			{*/
/*				correntPicture--;*/
/*				continue;*/
/*			}*/
/*			printf("filenameList %s\n",filenameList[correntPicture-1]);*/
/*			printf("每个像素所需的位数 : %d \n",msg.biBitCount);*/
			//printBmpMsg(& head,& msg);
/*			(*fcn)(name);*/
		 }
	}
	closedir(dfd);
////////////////////////////////////////////////////////////

	int weightOfGrid=3;//一行显示3张图片
	int rowOfgrid=correntPicture/weightOfGrid +1;
	    gtk_grid_set_row_spacing(GTK_GRID(grid), rowOfgrid);   
	    gtk_grid_set_column_spacing(GTK_GRID(grid), weightOfGrid); 
	int i;
	int hang=0;
	int lie=0;
	for (i = 0; i < correntPicture && i<50 && hang<rowOfgrid; hang+=1)
	{

		for (lie = 0; i < correntPicture && i<50 &&lie < weightOfGrid; i += 1,lie += 1)
		{
			imageShowpic[i] = gtk_image_new_from_file (filenameList[i]);
			
			eventBox[i]=gtk_event_box_new ();

			gtk_widget_add_events (eventBox[i], GDK_2BUTTON_PRESS);
			g_signal_connect (eventBox[i], "button_release_event",G_CALLBACK(mouseEnter), filenameList[i]);	

			gtk_grid_attach(GTK_GRID(gridShowBMP),eventBox[i] ,lie,hang,1,1);
			gtk_container_add (GTK_CONTAINER (eventBox[i]), imageShowpic[i] );
			gtk_widget_show (eventBox[i]);
			gtk_widget_show (imageShowpic[i]);	
/*			printf("lie is x=%d ,y=%d\n",lie);*/
		}
	
	}


/*	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(srollWindows[0]),gridShowBMP);*/
	gtk_widget_show (gridShowBMP);
/*	gtk_widget_show (srollWindows[0]);*/


        /* Generate the ipc key */
/*        ipckey = ftok("/tmp/foome2", 43);*/
        ipckey = ftok(picShowSendToMain, 43);
        printf("My key is %d\n", ipckey);

        /* Set up the message queue */
        mq_id = msgget(ipckey, IPC_CREAT | 0666);
        printf("Message identifier is %d\n", mq_id);

//////////共享内存代码

/*	char* name2 = ".";*/
/*	key = ftok(name2,0);*/
/*	if(key==-1)*/
/*		perror("ftok error");*/
/*	shm_id=shmget(key,4096,IPC_CREAT | 0666);*/
/*        printf("My key is %d\n", key);*/
/*        printf("My shm_id is %d\n", shm_id);	*/
/*	if(shm_id==-1)*/
/*	{*/
/*		perror("shmget error");*/
/*		return;*/
/*	}*/
/*	p_map=(people*)shmat(shm_id,NULL,0);*/
/*	printf("map  is %x\n", p_map);*/

/*	memcpy(((p_map[0])).name,newfilename,linshic);//末尾的结束符号也写进去*/
/*	(*(p_map)).age=2000;*/
	//断开与共享内存的链接
/*	if(shmdt(p_map)==-1)*/
/*		perror(" detach error ");*/
}
void showAllBMP()
{

}
void drow1( GtkWidget *widget, gpointer data )
{

}
gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
return FALSE;


}
void ProInitialization()
{	

}

gint mouseEnter( GtkWidget *widget, GdkEvent *event, gpointer data )
{
	char *filename=NULL;
	if(data)
	{
		filename=(char *)data;
		printf("liefilename =%s\n",filename);
	}
	else
	{
		printf("no data\n");
	
	}
/*	GdkEventMotion *e=(GdkEventMotion *)event;*/
/*	gint x=e->x;*/
/*	gint y=e->y;*/
/*	printf("sss x=%d ,y=%d\n",x,y);*/

	int len=strlen(filename);
	char *newfilename=malloc(sizeof(char)*(len-7+1));//要除去../era/部分
	int i=0;
	int j=0;
	for (i = 7; filename[i] !='\0'; i += 1)
	{
		newfilename[j++]= filename[i];
	}
	newfilename[j]= '\0';
/*	printf("newfilename=%s\n",newfilename);*/
/*	char * inputStr=NULL;*/


	//修改config文件，并发送消息
/*	 delLine("../era/Data/config",0);*/
/*        putStrInFile("../era/Data/config",newfilename,strlen(newfilename),0);*/
/*	addLineToFile("../era/Data/config","\n\n");*/



        /* Send a message */
        memset(mymsg.text, 0, 100); /* Clear out the space */
        strcpy(mymsg.text, newfilename);//newfilename就是发送的字符串
        mymsg.type = 1;
        msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);

	
	emptyFile(linshi);
	addLineToFile(linshi,newfilename);
	addLineToFile(linshi,"\n");
	////////////更新共享内存区域
/*	printf("newfilename=%s，size=%d\n",newfilename,sizeof(newfilename)/(sizeof(char)));*/
/*	int linshic=strlen(newfilename);*/
/*	printf("newfilename=%s，size=%d,map=%x\n",newfilename,linshic,&((*(p_map)).age));*/
/*	memcpy(((p_map[0])).name,newfilename,(linshic+1));//末尾的结束符号也写进去*/
/*	(*(p_map)).age=linshic+1;*/

	if(newfilename)
		free(newfilename);

	return 0;

}
void myMainWindow(GtkWidget *window)
{

/*	gtk_widget_add_events (window, GDK_POINTER_MOTION_MASK);*/
/*	g_signal_connect (window, "motion-notify-event",G_CALLBACK(mouseEnter), NULL);		*/

	//gint i=1;
	int rows=6;
	int columns=9;
	//int buttomNum=4;
	int buttomID=0;
	gint xpadding=10;
	gint ypadding=10;


	    grid = gtk_grid_new();
	    gtk_grid_set_row_spacing(GTK_GRID(grid), rows);   
	    gtk_grid_set_column_spacing(GTK_GRID(grid), columns);    
/*	    gtk_container_set_border_width(GTK_CONTAINER(grid), 6);*/

	gtk_scrolled_window_add_with_viewport(window,grid);
/*	gtk_container_add (GTK_CONTAINER (window), grid);*/

	fixed = gtk_fixed_new();
/*	gtk_container_add(GTK_CONTAINER(window), fixed);*/
	gtk_grid_attach(GTK_GRID(grid),fixed,0,1,6,9);

	    gridShowBMP = gtk_grid_new();
/*	    gtk_grid_set_row_spacing(GTK_GRID(grid), rows);   */
/*	    gtk_grid_set_column_spacing(GTK_GRID(grid), columns);    */
/*	    gtk_container_set_border_width(GTK_CONTAINER(grid), 6);*/
/*	    gtk_fixed_put (GTK_FIXED (fixed),gridShowBMP, 0, 0);*/

/*		srollWindows[0]=gtk_scrolled_window_new(NULL,NULL);*/

/*                gtk_table_attach_defaults(GTK_TABLE(littleTable),srollWindows[textID],0,6,1,2);*/
		gtk_fixed_put (GTK_FIXED (fixed),gridShowBMP, 0, 0);

/*		gtk_widget_show (srollWindows[0]);*/

	buttoms[buttomID]=gtk_button_new_with_label("bt1");
	g_signal_connect (buttoms[0], "clicked",G_CALLBACK(hello), NULL);
	gtk_grid_attach(GTK_GRID(grid),buttoms[buttomID],0,0,1,1);
/*	gtk_widget_set_usize(buttoms[buttomID],40,40);*/
/*	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 0, 0);*/
	gtk_widget_show (buttoms[0]);
	
	
	buttomID++;
	buttoms[buttomID]=gtk_button_new_with_label("drow1");
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(drow1), NULL);
	gtk_grid_attach(GTK_GRID(grid),buttoms[buttomID],1,0,1,1);
/*	gtk_widget_set_usize(buttoms[buttomID],40,40);*/
/*	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 60, 0);*/
	gtk_widget_show (buttoms[buttomID]);


/*	buttomID++;*/
/*	buttoms[buttomID]=gtk_button_new_with_label("drow2");*/
/*/*	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(drow2), NULL);*/
/*	gtk_table_attach_defaults(GTK_TABLE(table),buttoms[buttomID],0,1,0,1);*/
/*	gtk_widget_set_usize(buttoms[buttomID],40,40);*/
/*	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 120, 0);*/
/*	gtk_widget_show (buttoms[buttomID]);	*/

/*	 darea = gtk_drawing_area_new();*/
/*         gtk_fixed_put (GTK_FIXED (fixed),darea, 0, 30);*/
/*	g_signal_connect (G_OBJECT (darea), "draw",G_CALLBACK (draw_callback), NULL);*/
/*	gtk_window_get_size(GTK_WINDOW(window),&x,&y);*/
/*	 gtk_widget_set_size_request (darea, x, y);*/
/*	*/



	gtk_widget_show (grid);
	gtk_widget_show (fixed);
	gtk_widget_show_all(window);
	//工程信息初始化
	//ProInitialization();
}






