//#include <gtk/gtk.h>
//   
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define buttomNum  40
#define textNum  4



void createToolsTab();
void createCreateNeroTab(GtkWidget *fixedInside);
void createTab1_InMainWindow(GtkWidget * window,gint count,GtkWidget *notebook);
void myMainWindow(GtkWidget *window);
void printPos();
void DrawPictureWithPoints();
void DrawPictureWithLineMan(GtkWidget *darea,LineMan *manAllLine);
void DrawPictureWithLineManInVector(GtkWidget *darea,LineMan *manAllLine);

//显示前面n条线
void DrawPictureWithLineManInVectorN(GtkWidget *darea,LineMan *manAllLine,int showLine);

void hello( GtkWidget *widget, gpointer data );
void drow1( GtkWidget *widget, gpointer data );
void drow2( GtkWidget *widget, gpointer data );
//进行一些初始化
void ProInitialization();
gint delete_eventInMainWin( GtkWidget *widget, GdkEvent *event, gpointer data );
void myMainWindow(GtkWidget *window);
void paintInSurfaceRGB24(cairo_surface_t *image,int LineWeight,int Line,unsigned char * bmpdata);//传入图片的像素大小
void printDAta(unsigned char * bmpdata,int w,int h);

void SendMsgToCandy(char * fileName);



#ifndef ShmmaxCheck
#define ShmmaxCheck  33554432    //查看得知系统共享内存最大的就是32M
#define tryShm		(ShmmaxCheck)
#endif


#endif  //MAINWINDOW_H




