#ifndef   FILENAME
#define   FILENAME
#include "../common/type.h"
#endif


#define IfPaintLie  1  //判断是否话格子
//#define IfShowSpecial   //此有定义时将在函数findSpecialRGB中标记红色

#define MinAverageGap  1    //一个矩阵里面的平均gap的最小值，如果小于等于这个数，则认为在部分没有特殊点
#define RGBDifferentX  (0.8500)  //差异系数，用来控制差异的值大于多少是认为是差异点
#define SUBJuZhengBASE  9  //长宽都是
const int SubJuZheng =SUBJuZhengBASE;  //长宽都是
//const int SubJuZhengadd1 =21;  //长宽都是

//#define BMPLength   501
#define SubMatrixLen   (SUBJuZhengBASE+1)

//int SpectalColPOSINBMP[];
Color SubMatrixOfCol[SubMatrixLen][SubMatrixLen];//使用时可以不用0，0位置
int SubMatrixOfPos[SubMatrixLen][SubMatrixLen];//使用时可以不用0，0位置
int SubMatrixOfGap[SubMatrixLen][SubMatrixLen];//使用时可以不用0，0位置
unsigned char * PointTobmpdata;
#define SimilarMatrix   8
#define MaxGap    40
#define similarityOfTwoCol   (100-MaxGap)	//当俩个col的相似度大于这个值时，认为是相似的
				//d当前的算法中similarityOfTwoCol=100-gap
				//就是当gap小于40时候认为是相似的
int similarityAroundOnePoint[SimilarMatrix][SimilarMatrix];//保存一个点及其周围点的相似度,7*7矩阵，0位置不用
//int posOfBMP[BMPLength][BMPLength];//posOfBMP[1][1]表示该个像素在bmp图片中的起始位置

void scanBMP(unsigned char * bmpdata,int w,int l);


//w l是这个矩阵的像素个数，w不是一行的宽度，将获取的结果保存在posInBMP中，如果index大于最大可以获取的矩阵数的化，函数返回0，否则
//返回1
int getSubMatrix(int index,unsigned char * bmpdata,int w,int l,int * posInBMP,POS  *SubMatrix);//获取矩阵bmpdata中的第index个矩阵


//计算在SubMatrixposInBMP中表示的bmpdata中的子矩阵的平均像素值，并返回这个值
//另外，在计算过程中将会统计区域中数量较大的像素值,作为一个链表返回，表头为cnt，w 为一行的像素个数
Color CalculateAverageOfColor(unsigned char * bmpdata,int *SubMatrixposInBMP,ColorCnt * cnt,int w,POS  *SubMatrix);


//将Color计入cnt中，来统计每一种颜色出现的次数
void CountSomeColourInSubMatrix(ColorCnt * cnt,Color *averageCol);

#define GetPosFromXY(x,y,LengthOfLine)  ((x)+(y)*(LengthOfLine))

//getPosInBMPByVirtualPos---通过虚拟坐标获取实际像素矩阵的pos,他们y一样，x为3倍关系,前者从0开始，后者从0开始
//#define getPosInBMPByVirtualPos(actualPos,VirtualPos)  ((actualPos->x)=(VirtualPos->x)*3;(actualPos->y)=(VirtualPos->y))

//搜索point指向的list，返回第一个与col相同的颜色的list指针
struct list_head  * searchListForColour(struct list_head  *point,Color *Col);


void insertColorCnt(struct list_head  *cnt,Color *Col);

void  freeListCnt(struct list_head  *cnt);

void  printListCnt(struct list_head  *cnt);
void  OutPutListCnt(struct list_head  *cnt,char * file,int mark);
int   countNode(struct list_head  *cnt);
int  findSpecialRGB(ColorCnt * spectialCol,Color *averageCol);

//借鉴slab管理的方式，获取的每一条线条都用一个单独的链表表示
//然后用一个单独的链表来统一管理每条线条的head,在这里即使manAllLine
//返回找到的线条数
int searchLineBySpectialColPoint(ColorCnt * spectialCol,unsigned char * bmpdata,LineMan *manAllLine,int w,int l);

int CalculateSimilarMatrix(ColorCnt * cnt,int w,int l,unsigned char * bmpdata);
//比较cnt2与cnt1的相识度，这里以cnt1为基准，就是cnt1是参照对象，返回1表示相似，返回0表示不相似
int CalculateSimilarTwoCol(ColorCnt * cnt1,ColorCnt * cnt2);
void PrintSimilarMatrix(ColorCnt * cnt,int w,unsigned char * bmpdata);











