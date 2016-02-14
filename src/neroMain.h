
//   
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define widgetsNum  40
#define buttomNum  40

#define textNum  4
void initNeroNetWork( );
// void GetDataForNeroSys( GtkWidget *widget, gpointer data );
// void CreateNeroNetWork( GtkWidget *widget, gpointer data );
// void createMsgSearchTab(GtkWidget *fixedInside);
void createToolsTab();

void createNeroTreeTab(GtkWidget *fixedInside);

// void createCreateNeroTab(GtkWidget *fixedInside);
// void createTab1_InMainWindow(GtkWidget * window,gint count,GtkWidget *notebook);
// void myMainWindow(GtkWidget *window);
void printPos();
void DrawPictureWithPoints();

// void hello( GtkWidget *widget, gpointer data );
// void drow1( GtkWidget *widget, gpointer data );
// void drow2( GtkWidget *widget, gpointer data );

//进行一些初始化
void ProInitialization();
// gint delete_eventInMainWin( GtkWidget *widget, GdkEvent *event, gpointer data );
// void myMainWindow(GtkWidget *window);
void paintInSurfaceRGB24(cairo_surface_t *image,int LineWeight,int Line,unsigned char * bmpdata);//传入图片的像素大小
void printDAta(unsigned char * bmpdata,int w,int h);

void SendMsgToCandy(char * fileName);
void app_set_theme(const gchar *theme_path);
void tab_SetSearchNeroMsgViewText(gchar *  str);
#ifndef ShmmaxCheck
#define ShmmaxCheck  33554432    //查看得知系统共享内存最大的就是32M
#define tryShm		(ShmmaxCheck)
#endif


#endif  //MAINWINDOW_H




