#ifndef   FILENAME
#define   FILENAME
#include "../common/type.h"
#endif



//#define SUBJuZheng  20  //长宽都是
const int SubJuZheng =20;  //长宽都是
const int SubJuZhengadd1 =21;  //长宽都是

#define BMPLength   501

Color SubMatrixOfCol[21][21];//使用时可以不用0，0位置
int SubMatrixOfPos[21][21];//使用时可以不用0，0位置

unsigned char * PointTobmpdata;

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

void  findSpecialRGB(/*Color *SubMatrixOfCol,*/Color *averageCol);
















