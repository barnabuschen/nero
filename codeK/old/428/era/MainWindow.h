#include <gtk/gtk.h>


//char * titlebmp="Data/title.bmp";	

//认为俩个像素的rgb差值之后小于30认为颜色是相似的，可以近似认为颜色相同
#define ColourSimilar     40    

#define buttomNum  4
#define textNum  4
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
//进行一些初始化
void ProInitialization();
gint delete_eventInMainWin( GtkWidget *widget, GdkEvent *event, gpointer data );
void myMainWindow(GtkWidget *window);
void paintInSurfaceRGB24(cairo_surface_t *image,int LineWeight,int Line,unsigned char * bmpdata);//传入图片的像素大小
void printDAta(unsigned char * bmpdata,int w,int h);

void SendMsgToCandy(char * fileName);

typedef struct People{
	char name[40];
	int age;
} people;
struct { long type; char text[100]; } mymsg;
//tuxt图片显示程序向main发送消息
char * picShowSendToMain="/home/jty/shareSpace/picShowSendToMain";



//向cangy发送消息变量
key_t ipckeyForCandy;
int mqIdForCandy;
char* SendMsgWithCandy = "/home/jty/shareSpace/SendMsgWithCandy";

struct { long type; char text[100]; } msgForCandy;

//share space with candy
typedef struct{
	int flag;
int nSize;         /* IplImage大小 */
int ID;            /* 版本 (=0)*/
int nChannels;     /* 大多数OPENCV函数支持1,2,3 或 4 个通道 */
int alphaChannel; /* 被OpenCV忽略 */
int depth;         /* 像素的位深度，主要有以下支持格式： IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16U,IPL_DEPTH_16S, IPL_DEPTH_32S,
IPL_DEPTH_32F 和IPL_DEPTH_64F */

int dataOrder;     /* 0 - 交叉存取颜色通道, 1 - 分开的颜色通道.
只有cvCreateImage可以创建交叉存取图像 */

int width;        /* 图像宽像素数 */
int height;        /* 图像高像素数*/
int imageSize;     /* 图像数据大小(在交叉存取格式下ImageSize=image->height*image->widthStep），单位字节*/
char *imageData;    
int widthStep;     /* 排列的图像行大小，以字节为单位 */
char *data;		/* 指向排列的图像数据 */
} shareData;
	int shmIdWithCandy;
	key_t keyWithCandy;
	shareData *pMapWithCandy;
	char* nameWithCandy = "/home/jty/shareSpace/shareWithCandy5";

#ifndef ShmmaxCheck
#define ShmmaxCheck  33554432    //查看得知系统共享内存最大的就是32M
#define tryShm		(4096)
#endif







