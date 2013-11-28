


//typedef struct{
//	char name[40];
//	int age;
//} people;
//int shm_id;
//key_t key;
//people *p_map;
//char* name = "/tmp/myshm2";



key_t ipckey;
int mq_id;
//struct { long type; char text[100]; } mymsg;
char *configFilen="../era/Data/config";


//向cangy发送消息变量
key_t ipckeyForCandy;
int mqIdForCandy;
struct { long type; char text[100]; } msgForCandy;
char* SendMsgWithCandy = "/home/jty/shareSpace/SendMsgWithCandy";



//share space with candy
typedef struct ShareDataWithMain{
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
char data;		/* 指向排列的图像数据 */
} shareData;
#ifndef ShmmaxCheck
#define ShmmaxCheck  33554432    //查看得知系统共享内存最大的就是32M
#define tryShm		(ShmmaxCheck)
#endif


int shmIdWithCandy;
key_t keyWithCandy;
shareData *pMapWithCandy;
char* nameWithCandy = "/home/jty/shareSpace/shareWithCandy8";


//尝试使用cvFindContours
IplImage *img;
IplImage* g_image = NULL;  
IplImage* g_gray = NULL;  
int g_thresh = 100;  //阈值  
CvMemStorage* g_storage = NULL; 
int mode = CV_RETR_EXTERNAL;
 CvMemStorage* storage = NULL; 

int onetourlength;


void on_trackbar(int h);
void candyTest(char * filename,int h,shareData *pMapWithCandy);







