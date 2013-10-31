#ifndef   FILENAME
#define   FILENAME
#include "../common/type.h"
#endif


//#define IfPaintLie  1  //判断是否话格子
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
#define numOfSimilarMatrix  48		//这个数要根据SimilarMatrix得出
#define MaxGap    40
#define similarityOfTwoCol   (100-MaxGap)	//当俩个col的相似度大于这个值时，认为是相似的
				//d当前的算法中similarityOfTwoCol=100-gap
				//就是当gap小于40时候认为是相似的
int similarityAroundOnePoint[SimilarMatrix][SimilarMatrix];//保存一个点及其周围点的相似度,7*7矩阵，0位置不用
int similarityAroundOneMark[SimilarMatrix][SimilarMatrix];//标记一个点其周围点已经被认定为边缘点，
							//且已经链入链表的相似点,7*7矩阵，0位置不用
							//当标记为1时说明已经加入链表，以后无需再判断
							//如果该点是非相似点，那么当被标记为2的时候
							//表明该点被


//int posOfBMP[BMPLength][BMPLength];//posOfBMP[1][1]表示该个像素在bmp图片中的起始位置

int scanBMP(unsigned char * bmpdata,int w,int l);


//w l是这个矩阵的像素个数，w不是一行的宽度，将获取的结果保存在posInBMP中，如果index大于最大可以获取的矩阵数的化，函数返回0，否则
//返回1
int getSubMatrix(int index,unsigned char * bmpdata,int w,int l,int * posInBMP,POS  *SubMatrix);//获取矩阵bmpdata中的第index个矩阵


//计算在SubMatrixposInBMP中表示的bmpdata中的子矩阵的平均像素值，并返回这个值
//另外，在计算过程中将会统计区域中数量较大的像素值,作为一个链表返回，表头为cnt，w 为一行的像素个数
Color CalculateAverageOfColor(unsigned char * bmpdata,int *SubMatrixposInBMP,ColorCnt * cnt,int w,POS  *SubMatrix);


//将Color计入cnt中，来统计每一种颜色出现的次数
int CountSomeColourInSubMatrix(ColorCnt * cnt,Color *averageCol);

#define GetPosFromXY(x,y,LengthOfLine)  ((x)+(y)*(LengthOfLine))

//getPosInBMPByVirtualPos---通过虚拟坐标获取实际像素矩阵的pos,他们y一样，x为3倍关系,前者从0开始，后者从0开始
//#define getPosInBMPByVirtualPos(actualPos,VirtualPos)  ((actualPos->x)=(VirtualPos->x)*3;(actualPos->y)=(VirtualPos->y))

//搜索point指向的list，返回第一个与col相同的颜色的list指针
struct list_head  * searchListForColour(struct list_head  *point,Color *Col);


int insertColorCnt(struct list_head  *cnt,Color *Col);

int  freeListCnt(struct list_head  *cnt);

int  printListCnt(struct list_head  *cnt);
int  OutPutListCnt(struct list_head  *cnt,char * file,int mark);
int   countNode(struct list_head  *cnt);
int  findSpecialRGB(ColorCnt * spectialCol,Color *averageCol);

//借鉴slab管理的方式，获取的每一条线条都用一个单独的链表表示
//然后用一个单独的链表来统一管理每条线条的head,在这里即使manAllLine
//返回找到的线条数
int searchLineBySpectialColPoint(ColorCnt * spectialCol,unsigned char * bmpdata,LineMan *manAllLine,int w,int l);

//返回计算到的相似点的个数
int CalculateSimilarMatrix(ColorCnt * cnt,int w,int l,unsigned char * bmpdata);
//比较cnt2与cnt1的相识度，这里以cnt1为基准，就是cnt1是参照对象，返回1表示相似，返回0表示不相似
int CalculateSimilarTwoCol(ColorCnt * cnt1,ColorCnt * cnt2);
void PrintSimilarMatrix(ColorCnt * cnt,int w,unsigned char * bmpdata);


//输入当前点，bmp，以及找到线后的管理链表manAllLine，---在这个函数中将搜索及确定与currentCol相似的边缘线条
//并将边缘线条上的点都组成一个链表，存与manAllLine之下
int getLineFromBMP(ColorCnt * currentCol ,unsigned char * bmpdata,LineMan *manAllLine,int w,int l);
//这里首先会生成一个PosList的链表，用它来处理similarityAroundOnePoint矩阵中的相似点
//在链表中的点是整个连在一起的相似点区域的最外围点的集合，显然，就是一个边界，当然这个边界
//还是在similarityAroundOnePoint矩阵范围内的
//成功生成则返回1，否则返回0
int getsimilarPosFormMatr(LineMan * manAllLineIInPos);
//判断一个pos是不是边缘点，是返回1
int ifIsEdge(POS * pos);
//只要该点合法就检查是不是相似点
int ifIssimilarPosInMatrix(int xx,int yy);

int  printPosList(struct list_head *headOfPosList);

int searchPosListForNewNode(LineMan * manAllLineIInPos);

//获取一个点附近的下一个边缘点
PosList * getNextEdgePosList(PosList *currentPos);
//PosList * getNextEdgePosList2(PosList *currentPos);
PosList * getNextEdgePosList2(PosList *currentPos,PosList * currentLineOfPos);
int insertPosList(struct list_head  *head,PosList *Node);


int insertLineManNode(struct list_head  *head,LineMan *Node);
//判断是否还需要遍历链表来获得边缘点
int hasPosToAddToLine(LineMan * manAllLineIInPos);

int hideInArea(int x,int y);
int ContactWithLine(int x,int y,LineMan * manAllLineIInPos);
//判断相似点currentPos能否给线条currentLineOfPos增加可接触到的非相似点
int ifCouldMeetNewNonSimilar(int x,int y ,PosList * currentLineOfPos);









#ifdef NowIdDeBug
//DeBug
int DeBugW;
int DeBugL;

int DeBugindex;
Color DeBugaverageCol;
int DeBugaverageGap;
ColorCnt *DeBugspectialCol;

LineMan  * DeBugmanAllLineIInPos;

int DeBugnearX;
int DeBugnearY;

int DeBugcurrentPosX;
int DeBugcurrentPosY;
#endif
















