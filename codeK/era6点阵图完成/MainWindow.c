#ifndef   FILENAME
#define   FILENAME
#include "common/type.h"
#endif
//#define GTK_ENABLE_BROKEN
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
	g_return_if_fail(bmpdata != NULL);
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
	//////////////////////////////////////////do something//////////////////////////////////
	//
	int i=0;
	int j=0;
	bmpHead head;
	bmpMsg msg;
	unsigned int  dataCount;
	unsigned char * bmpdata11=readBmpData("Data/17.bmp",&dataCount,&head,&msg);
	int endpos=100;
	for(;i<msg.biHeight;i++)
	{

		for(j=4;j<16 && i <endpos;j+=4)
			{

				bmpdata[i*stride+j]=0x00+i;//b
				bmpdata[i*stride+j+1]=0xff;//g
				bmpdata[i*stride+j+2]=0xff;//r
			}
		for(j=32;j<44 && i <endpos;j+=4)
			{

				bmpdata[i*stride+j]=0x00;//b
				bmpdata[i*stride+j+1]=0xff-i;//g
				bmpdata[i*stride+j+2]=0xff;//r
			}
		for(j=48;j<60 && i <endpos;j+=4)
			{

				bmpdata[i*stride+j]=0x00;//b
				bmpdata[i*stride+j+1]=0xff;//g
				bmpdata[i*stride+j+2]=0xff-i;//r
				if(j==50)
				printf("%2x %2x %2x \n",bmpdata[i*stride+j],bmpdata[i*stride+j+1],bmpdata[i*stride+j+2]);
			}
		for(j=4;j<12 && i >=endpos;j+=4)
			{
/*				bmpdata[i*stride+j]=0xff;*/
/*				bmpdata[i*stride+j+1]=0xff;*/
/*				bmpdata[i*stride+j+2]=0x00;*/
				bmpdata[i*stride+j]=0x00;//b
				bmpdata[i*stride+j+1]=0x00;//g
				bmpdata[i*stride+j+2]=0x00;//r
			}
		

	}

////////////////////////////////////////

	
	image=cairo_image_surface_create_for_data (bmpdata,CAIRO_FORMAT_RGB24,stride/4,height,stride);

	g_return_if_fail(image != NULL);
	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));
	g_return_if_fail(cr != NULL);
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
void hello( GtkWidget *widget, gpointer data )
{
	char * sevean=getLineInFile("Data/config",1);






	 paintInRGB24(sevean);



}
void drow1( GtkWidget *widget, gpointer data )
{
	char * sevean="Data/7.bmp";	

	image=cairo_image_surface_create_from_png(sevean);
/*	unsigned char * pixbuf=cairo_image_surface_get_data (image);*/

	

	g_return_if_fail(image != NULL);
	cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(darea));
	cairo_set_source_surface(cr, image, 10, 40);
/*	cairo_set_source_rgb(cr,0,1,0);//设置画笔颜色，也就是红，绿，蓝，这里设置成绿色。*/
/*	cairo_rectangle(cr,0,0,x,y);//画一个方块，位置从坐标(10,10)开始，宽200，高200*/
/*	cairo_fill(cr);//填充，使用的颜色当然是上面设置的颜色。*/
	//gdk_cairo_set_source_pixbuf (cr, pixbuf, x, y);
	cairo_paint (cr);
	cairo_destroy (cr);

}
#define MAX_PATH 1024
void drow2( GtkWidget *widget, gpointer data )
{
	char name[MAX_PATH];
	struct dirent *dp;
	DIR *dfd;
	char *dir="Data";
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
			printf("%s\n",dp->d_name);
			
			bmpHead head;
			bmpMsg msg;
			name[0]='\0';
			strcat(name,"Data/");
			strcat(name+5,dp->d_name);
			getBmpMsg(name,& head,& msg);
			printf("每个像素所需的位数 : %d \n",msg.biBitCount);
			//printBmpMsg(& head,& msg);
/*			(*fcn)(name);*/
		 }
	}
	closedir(dfd);


}
gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
return FALSE;
	char * sevean=getLineInFile("Data/config",1);
	printf("sevean%s \n",sevean);

	unsigned int  dataCount;
	bmpHead head;
	bmpMsg msg;
/*	unsigned char * bmpdata=*/
	readBmpData(sevean,&dataCount,&head,&msg);
	printBmpMsg((PbmpHead)&head,(PbmpMsg)&msg);
	//printf("数据区大小%d ,应该是%d \n",msg.biSizeImage,msg.biWidth*msg.biHeight*3);



	GtkWidget *   bmp=gtk_image_new_from_file(sevean);
	GdkPixbuf * Pixbuf=(GdkPixbuf * )gtk_image_get_pixbuf(( GtkImage *)bmp);
	guchar *    bmpdata=gdk_pixbuf_get_pixels (Pixbuf);
	g_return_if_fail(bmpdata != NULL);

	gsize   allB=gdk_pixbuf_get_byte_length  (Pixbuf);///就是图像区数据的大小
	printf("allB is %d \n",allB);
	int w= gdk_pixbuf_get_width(Pixbuf);
	int l=gdk_pixbuf_get_height (Pixbuf);
	int stride=gdk_pixbuf_get_rowstride(Pixbuf);


	
	int i=0;
        while((w * 3 +i)%4 != 0)
                i++;
/*        hangInData=i+w * 3;*/
	

	unsigned char * newbmpdata2=addLieToBMP( bmpdata, w * 3 +i,l,i*2);
	w+=i;

	//尝试转化为RGBA
	unsigned char * newbmpdata =putBMPRGBtoRGBA( newbmpdata2, w,l);


	int stride2 =  cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, 1);
	int stride3 =  cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, w);
	stride=w*4;
	printf("w is %d \n",w);
	printf("l is %d \n",l);
	printf("stride is %d ,%d  ,%d  \n",stride,stride2,stride3);

/*	putBMPRGBtoBGR(bmpdata,w,l);*/
	image = cairo_image_surface_create_for_data ((unsigned char *) newbmpdata,CAIRO_FORMAT_RGB24,w, l,stride);
	 printDAta( newbmpdata2, stride, 1);

	printf("w is %d \n",cairo_image_surface_get_width (image));
	printf("l is %d \n",cairo_image_surface_get_height (image));
	printf("stride is %d\n", cairo_image_surface_get_stride(image));


	g_return_if_fail(image != NULL);
	cairo_t *ccr = gdk_cairo_create (gtk_widget_get_window(darea));
	g_return_if_fail(cr != NULL);
	cairo_set_source_surface(ccr, image, 40, 20);
	cairo_paint (ccr);
	cairo_destroy (ccr);
	if(bmpdata)
		free(bmpdata);
	if(newbmpdata)
		free(newbmpdata);
	if(newbmpdata2)
		free(newbmpdata2);
	return FALSE;
}
void ProInitialization()
{	




}

void myMainWindow(GtkWidget *window)
{

		

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
	buttoms[buttomID]=gtk_button_new_with_label("drow1");
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(drow1), NULL);
	//gtk_table_attach_defaults(GTK_TABLE(table),buttoms[0],0,1,0,1);
	//gtk_widget_set_usize(buttoms[buttomID],40,40);
	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 60, 0);
	gtk_widget_show (buttoms[buttomID]);


	buttomID++;
	buttoms[buttomID]=gtk_button_new_with_label("drow2");
	g_signal_connect (buttoms[buttomID], "clicked",G_CALLBACK(drow2), NULL);
	//gtk_table_attach_defaults(GTK_TABLE(table),buttoms[0],0,1,0,1);
	//gtk_widget_set_usize(buttoms[buttomID],40,40);
	gtk_fixed_put (GTK_FIXED (fixed), buttoms[buttomID], 120, 0);
	gtk_widget_show (buttoms[buttomID]);	

	 darea = gtk_drawing_area_new();
         gtk_fixed_put (GTK_FIXED (fixed),darea, 0, 30);
	g_signal_connect (G_OBJECT (darea), "draw",G_CALLBACK (draw_callback), NULL);
	gtk_window_get_size(GTK_WINDOW(window),&x,&y);
	 gtk_widget_set_size_request (darea, x, y);
	



	gtk_widget_show (darea);
	gtk_widget_show (fixed);
	gtk_widget_show_all(window);
	//工程信息初始化
	//ProInitialization();
}






