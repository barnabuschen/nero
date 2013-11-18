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

#include <cairo.h>
#include "NeuralNetwork/NeuralNetwork.h"
/*#include "common/Neuron.h"*/
#include "MainWindow.h"
#include "common/fileOperating.h"
/*#include "graph/bmpRead.h"*/
#include "tools/readUTF8File.h"
#include "tools/createDot.h"



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


LineMan *manAllLineFromNeo;

void ProInitialization()
{


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

/*	char * sevean=getLineInFile("Data/config",1);*/
/*	 SubP(sevean);*/
/*	int res;*/
/*	pthread_t a_thread;*/

/*	res = pthread_create(&a_thread, NULL,thread_for_paint_Pic, NULL);*/

}
void drow1( GtkWidget *widget, gpointer data )
{
}
void drow2( GtkWidget *widget, gpointer data )
{
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



	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook),TRUE);
	gtk_fixed_put (GTK_FIXED (fixed),notebook, 0, 30);
/*	gtk_container_add(GTK_CONTAINER(window), notebook);*/
	gtk_window_get_size(GTK_WINDOW(window),&x,&y);
	gtk_widget_set_size_request (notebook, x, y);
	gint count;

	for (count = 1; count <= 3; count++)
	{
		
		createTab1_InMainWindow( (window), count, notebook);
	}

	gtk_widget_show_all(window);
    








	gtk_widget_show_all(window);
	//工程信息初始化
	ProInitialization();
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
	gtk_container_add(GTK_CONTAINER(vbox), buttoms[buttomID]);
	
	
	gtk_widget_set_size_request (vbox, 15, 15);
	gtk_fixed_put (GTK_FIXED (fixedInside), vbox, 0, 0);
	
	    
}
void CreateNeroNetWork( GtkWidget *widget, gpointer data )
{
	GtkWidget * dialog= dialog = gtk_message_dialog_new (Mainwindow,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_CLOSE,
                                 "done here");


	
	readUTF8FileData("data/ChUnicode");
	nero_s32int res=CreateActNeroNet();
	
	/*一下步就是将字符信息加入网络*/
	
	/*show  neroNet*/
/*	createNeroNetDotGraph(GodNero, "data/pic.dot");*/
	
	
	#ifdef  Nero_DeBuging11
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);	
	#endif


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


	#ifdef Nero_DeBuging1_cacel
	switch(counts)
	{
	case 3:;
	case 2:text = g_strdup_printf("Page %d", count);;break;
	case 1:text = g_strdup_printf("tools");createToolsTab(fixedInside);break;
	default:break;
	
	}
	#else
	switch(counts)
	{
	case 1:text = g_strdup_printf("createNero");createCreateNeroTab( fixedInside);break;
	case 2:text = g_strdup_printf("Page %d", count);break;
	case 3:text = g_strdup_printf("tools");createToolsTab(fixedInside);break;
	default:break;
	
	}
	#endif	
	GtkWidget *label = gtk_label_new(text);/*tab的名字*/


	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), fixedInside, label);


}






















