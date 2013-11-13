#include <gtk/gtk.h>


//char * titlebmp="Data/title.bmp";	

//认为俩个像素的rgb差值之后小于30认为颜色是相似的，可以近似认为颜色相同
#define ColourSimilar     40    

#define buttomNum  4
#define textNum  4
#define MaxShowPicture  50
int correntPicture=0;
char * filenameList[MaxShowPicture];//声明一直指针数组，
GtkWidget *  imageShowpic[MaxShowPicture];
GtkWidget * gridShowBMP;//GtkImage 接收事件
GtkWidget * eventBox[MaxShowPicture];

GtkWidget * grid;
GtkWidget * table;//主界面的table
//GtkWidget * texts[textNum];
GtkWidget * buttoms[buttomNum];
GtkWidget * srollWindows[textNum];
GdkWindow *paintwindow;
GtkWidget *darea;
GtkWidget *fixed;
cairo_surface_t *image;
void hello( GtkWidget *widget, gpointer data );
void drow1( GtkWidget *widget, gpointer data );
void drow2( GtkWidget *widget, gpointer data );
////进行一些初始化
//void ProInitialization();
//void myMainWindow(GtkWidget *window);
//void paintInSurfaceRGB24(cairo_surface_t *image,int LineWeight,int Line,unsigned char * bmpdata);//传入图片的像素大小
//void printDAta(unsigned char * bmpdata,int w,int h);

void showAllBMP();
gint mouseEnter( GtkWidget *widget, GdkEvent *event, gpointer data );












