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

#ifndef ShmmaxCheck
#define ShmmaxCheck  554432    //查看得知系统共享内存最大的就是32M
#define ShmMin   40960
#endif
//tuxt图片显示程序向main发送消息
char * picShowSendToMain="/home/jty/shareSpace/picShowSendToMain";


key_t ipckey;
int mq_id;
struct { long type; char text[100]; } mymsg;
char *linshi="../era/Data/config";
typedef struct{
	char name[40];
	int age;
} people;
#define MY_SHM_ID 67483
	int shm_id;
	key_t key;
	char temp;
	people *p_map;
	char* name2 = "/tmp/myshm2";//sendMsgToMain


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












