#ifndef   FILENAME
#define   FILENAME
#include "../common/type.h"
#endif

#include <stdio.h>
#include  <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include "bmpRead.h"
#include "../common/fileOperating.h"

#ifndef  LIST_INCLUDE
#include "../common/list.h"
#endif

#include "searchLine.h"
extern int getstrideOfBMP(int w);
extern int addLineToFile(char * FileName,char * str);
extern   void INIT_LIST_HEAD(struct list_head *list);
extern void emptyFile(char * fileName);
inline int compRGB(unsigned char *color1,unsigned char *color2)//比较俩种颜色的相似度，color1和color2是数组而不是字符串，所以请自己保证数组没有越界
{
	g_return_if_fail(color1 != NULL);
	g_return_if_fail(color2 != NULL);
	int r=(int )color1[0]-(int )color2[0];
	int g=(int )color1[1]-(int )color2[1];	
	int b=(int )color1[2]-(int )color2[2];

	//相似度的定义是需要仔细考虑的

	r= (r<0)? (-1)*r:r;
	g= (g<0)? (-1)*g:g;
	b= (b<0)? (-1)*b:b;	
	g_return_if_fail(r >=0);
	g_return_if_fail(g >=0);
	g_return_if_fail(b >=0);
	return (r+g+b);
}
inline int compRGB2(Color *color1,Color *color2)//比较俩种颜色的相似度，color1和color2是数组而不是字符串，所以请自己保证数组没有越界
{
	//这里引用空间俩点的距离定义来计算差距值
	g_return_if_fail(color1 != NULL);
	g_return_if_fail(color2 != NULL);
	int r=(int )(color1->r)-(int )(color2->r);
	int g=(int )(color1->g)-(int )(color2->g);
	int b=(int )(color1->b)-(int )(color2->b);

	//相似度的定义是需要仔细考虑的

	r= r*r;
	g= g*g;
	b= b*b;	

	return sqrt(r+g+b);
}
inline int compRGBWithCnt(ColorCnt *color1,ColorCnt *color2)//比较俩种颜色的相似度，color1和color2是数组而不是字符串，所以请自己保证数组没有越界
{
	//这里引用空间俩点的距离定义来计算差距值
	g_return_if_fail(color1 != NULL);
	g_return_if_fail(color2 != NULL);
	int r=(int )(color1->r)-(int )(color2->r);
	int g=(int )(color1->g)-(int )(color2->g);
	int b=(int )(color1->b)-(int )(color2->b);

	//相似度的定义是需要仔细考虑的

	r= r*r;
	g= g*g;
	b= b*b;	

	return sqrt(r+g+b);
}
inline int getXFromY(int pos,int y,int LengthOfLine)
{

	return (pos-y*LengthOfLine);

}
inline int getYFromX(int pos,int LengthOfLine)
{

	return (pos/LengthOfLine);//这个y可以直接用  pos/LengthOfLine就行了

}
inline int getPosFromXY(int y,int x,int LengthOfLine)//这里的行列都是指该点在bmp矩阵中的行列，不是以像素为单位的矩阵的行列
{

	return (x+y*LengthOfLine);

}
//初始化一条线的表头
int InitialInitialPOSHead(PosList *headOfPosList)
{
	if(headOfPosList == NULL)
	{
		printf("InitialInitialPOSHead error \n");
		return 0;
	}
	//首先创建第一条线的链表
/*	PosList *headOfPosList=malloc(sizeof(PosList));*/
	(headOfPosList->p).next=&(headOfPosList->p);
	(headOfPosList->p).prev=&(headOfPosList->p);
/*	*Node=headOfPosList;*/
}
//初始化一条线条集合管理表头，里面将动态分配一个内存
int InitialLineManNode(LineMan *headOfPosList)
{

	if(headOfPosList == NULL)
	{printf("InitialLineManNode error \n");
		return 0;
	}
	//首先创建第一条线的链表
/*	LineMan *headOfPosList=malloc(sizeof(LineMan));*/
	(headOfPosList->p).next=&(headOfPosList->p);
	(headOfPosList->p).prev=&(headOfPosList->p);
	(headOfPosList->cntHead)=NULL;
	(headOfPosList->posHead)=NULL;	
	(headOfPosList->count)=0;
/*	*Node=headOfPosList;*/
/*printf("headOfPosList =%x \n",headOfPosList);*/
}
int createLineFromPos(int PosCount,POS * p,PosList *head)
{
	if( p == NULL || PosCount<=0)
	{
		printf("createLineFromPos error \n");
		return 0;
	}
	int i=0;
/*	head=malloc(sizeof(PosList));*/
	InitialInitialPOSHead(head);
	PosList lin;
	for (i = 0; i < PosCount; i += 1)
	{
/*		printPosList(&(head->p));*/
/*		printf("insertPosList =%d,%d \n",(p+i)->x,(p+i)->y);*/
		lin.pos.x=(p+i)->x;
		lin.pos.y=(p+i)->y;
			
		insertPosList(&(head->p),&lin);
		
	}	
/*	printPosList(&(head->p));*/
	return 1;
}
int CreateLineManFromPoses(int PosCount,POS * p,LineMan *manAllLine)
{
	if(PosCount < 0 || manAllLine == NULL || manAllLine->count!=0 || p ==NULL)
	{
		printf("CreateLineManFromPoses error \n");
		printf("PosCount =%d \n",PosCount);
		printf("p =%x \n",p);
		printf("manAllLine =%x \n",manAllLine);
		printf("manAllLine->count =%d \n",manAllLine->count);
		return 0;
	}

	if(PosCount>0)
	{	int i=0;
		POS * curr;
		POS * next;
		POS * start;//一条线的第一个点
		POS * end;//一条线的最后一个点
		int countOfPosINLine=0;//统计线的点数
		int countOfLine=0;//统计线tiao数,其实是间隔点的数量
		int *pForJianGe=malloc(sizeof(int)*(PosCount));//将每个间隔点的位置保存在此
		//先统计有多少条线条、
		for (i = 0; i < (PosCount); i += 1)
		{
			curr=( p +i  );
			if(curr->x <0  || curr->y < 0 )
			{
				pForJianGe[countOfLine++]=i;
			}
				
		}
/*		printf("countOfLine =%x \n",countOfLine);*/
		//开始生成线条
		PosList *head;//
		LineMan *MaNode;
		int res=0;
		for (i = 0; i < (countOfLine); i += 1)
		{

			curr=( p +pForJianGe[i]+1);
			if(i != (countOfLine-1))
			{
/*				next=( p +pForJianGe[i+1] -1);	*/
				countOfPosINLine=pForJianGe[i+1]  -  pForJianGe[i] -1;		
			}
			else 
				countOfPosINLine=PosCount-pForJianGe[i]-1;

/*			printf("curr =%d,%d \n",curr->x,curr->y);*/
/*			printf("end =%d,%d \n",(curr+countOfPosINLine-1)->x,(curr+countOfPosINLine-1)->y);*/
/*			printf("countOfPosINLine =%d \n",countOfPosINLine);*/
			if(countOfPosINLine>0)
			{
				
				head=malloc(sizeof(PosList));
				InitialInitialPOSHead(head);
				res=createLineFromPos(countOfPosINLine,curr,head);
				if(res >0)
				{
					//插入线条
					MaNode=malloc(sizeof(LineMan));
					InitialLineManNode(MaNode);
					MaNode->posHead=head;
					insertLineManNode2(&(manAllLine->p),MaNode);
				}
				
			}	
		}
	}

/*printAllLineIInPos(manAllLine);*/
	return 1;
}



int scanBMP(unsigned char * bmpdata,int w,int l)
{
	g_return_if_fail(bmpdata != NULL);
	int numOfSubMatrixINWeight=w/(int)SubJuZheng;
	int numOfSubMatrixINHeight=l/(int)SubJuZheng;
	printf("可以容耐的小矩阵a*b=%d * %d \n",numOfSubMatrixINWeight,numOfSubMatrixINHeight);


	//这个用来管理最终生成的cntHead表示的线条的链表集合，
	LineMan  manAllLine;
	manAllLine.p.prev=&(manAllLine.p);
	manAllLine.p.next=&(manAllLine.p);
	manAllLine.cntHead=NULL;
	manAllLine.posHead=NULL;
	manAllLine.count=0;
	char *scanMark=calloc((w*l),sizeof(char));
	
	PointTobmpdata=bmpdata;

	//依次给出20*20像素的一个子矩阵，从左到右，从上到下，左右上下的坐标直接是bmpdata中的四个点
	//（zhi注意：一个点实际上是有3个数rgb表示的）的开始位置（左l，右r，上u，下d）
	//
	int posInBMP[5];


	//这个变量是一个链表，保存了每次计算区域的像素平均值是各个像素出现的值和次数
	//考虑到list_head 是个双向链表，如果把cnt作为一个表头，把cnt->prev/next初始化为它自己，当有插入节点时就将prex指向末尾是非常
	//有帮助的
	ColorCnt cnt;
	cnt.r=0;
	cnt.g=0;
	cnt.b=0;
	cnt.p.prev=&(cnt.p);	
	cnt.p.next=&(cnt.p);


	//这个链表存储了每次在小矩阵中找到的findSpecialRGB
	ColorCnt spectialCol;
	spectialCol.r=0;
	spectialCol.g=0;
	spectialCol.b=0;
	spectialCol.p.prev=&(spectialCol.p);	
	spectialCol.p.next=&(spectialCol.p);

	POS  SubMatrix[5];//o不用：这个用来子矩阵在databmp中虚拟的坐标，可以用POS来计算除该点在bmpdata实际的位置(对应值在posInBMP中)
				//注意它存储的是虚拟坐标，如SubMatrix[1]的xy都为1时表示的是从上到下，从左到右数，像素值序数为1的点

	emptyFile( "Data/output");
	int index=1;
	for(;index>0;index++)
	{
/*		if(index <25)*/
/*			printf("index=%d\n",index);*/
		int res=getSubMatrix(index, bmpdata, w, l, posInBMP,SubMatrix);

		if(res ==1)
		{
			//计算该区域的像素平均值
			//每次进入此函数要保证cnt是个空列表		
			freeListCnt(&(cnt.p));
			freeListCnt(&(spectialCol.p));
	
			Color averageCol= CalculateAverageOfColor(bmpdata,posInBMP,&cnt,w,SubMatrix);


			//是时候寻找突出点了：就是与周围颜色值有较大差异的点
			//你会发现如果averageGap<=1的话，基本上可以认为这里面没有特别特殊的点了
			int averageGap=findSpecialRGB( &spectialCol,&averageCol);
			#ifdef NowIdDeBug
			DeBugindex=index;
			DeBugaverageCol=averageCol;
			DeBugaverageGap=averageGap;
			DeBugspectialCol=&spectialCol;
			DeBugW=w;
			DeBugL=l;
			#endif
			//现在可以开始搜索线条了
			//因为在findSpecialRGB获取的会有很多个spectialCol，可能有多条可以搜索到
			//的线条
			//你可以借鉴slab管理的方式，获取的每一条线条都用一个单独的链表表示
			//然后用一个单独的链表来统一管理每条线条的head
			int findLineCnt=0;
			if(list_ISempty(&(spectialCol.p)) ==0)
				{
					findLineCnt=searchLineBySpectialColPoint(&spectialCol,bmpdata,& manAllLine,w,l);					
				}
		}
		else  break;

	}
	if(scanMark)
		free(scanMark);
/*	malloc(SubJuZheng);*/


}
int searchLineBySpectialColPoint(ColorCnt * spectialCol,unsigned char * bmpdata,LineMan *manAllLine,int w,int l)
{
	g_return_if_fail(spectialCol != NULL );
	g_return_if_fail(bmpdata != NULL );
	g_return_if_fail(manAllLine != NULL );
	g_return_if_fail((spectialCol->p).next != &(spectialCol->p) );
	g_return_if_fail((spectialCol->p).prev != &(spectialCol->p) );
	g_return_if_fail((manAllLine->p).next == &(manAllLine->p) );
	g_return_if_fail((manAllLine->p).prev == &(manAllLine->p) );



	int findLineCnt=0;
	
	static int here=0;


	struct list_head  *pos=(spectialCol->p).next;//获取第一个col
	ColorCnt * cnt=NULL;
	//第一步变量spectialCol依次获取每一个col
	while(pos !=&(spectialCol->p) && pos != NULL)
	{
		
		cnt=(ColorCnt *)pos;//获取第一个col

		//首先计算该点与该点周围3个圈的相似度，即一个7*7的矩阵
		int countOFSimilar=CalculateSimilarMatrix(cnt,w,l,bmpdata);

		//将周围至少有3个相似点的非相似点设为相似点




		//终于到这个模块的最后一步了，也是最要命的一步
		switch(countOFSimilar)
		{
		case 0:break;
		case 1:break;
		case 2:break;
/*		case 3:break;*/
		case numOfSimilarMatrix:break;

		default:
			{

			//输入当前点，bmp，以及找到线后的管理链表manAllLine，
			int res=getLineFromBMP(cnt,bmpdata,manAllLine, w, l);
			break;
			}
		}
		pos=pos->next;
	}





	return findLineCnt;
}
int getLineFromBMP(ColorCnt * currentCol ,unsigned char * bmpdata,LineMan *manAllLine,int w,int l)
{
	g_return_if_fail(currentCol != NULL );
	g_return_if_fail(bmpdata != NULL );
	g_return_if_fail(manAllLine != NULL );
	g_return_if_fail(w > 0 );
	g_return_if_fail(l > 0 );

	int res=0;
	int stride=getstrideOfBMP(w);//指的是长度，一定大于0
	int currentColHang=getYFromX(currentCol->count,stride);//我们称矩阵similarityAroundOnePoint中心点
						//即currentCol点为S点
	int currentColLie=getXFromY(currentCol->count,currentColHang,stride);//从0开始
	//第一次搜索similarityAroundOnePoint最内圈的点

	//从最内圈开始，依次往外层数，依次为第1圈，第2圈，第3圈
	//因为S点在矩阵中的位置为hang=4，lie=4(设为Xs,Ys)
	//第N圈左上角的位置为hang=Ys-N，lie=Xs-N,这个小矩阵的长宽为N*2+1
	int ifHasSimilar=0;//记录有没有相似点在这层上,da大于0说明有相似点

	int Xs=4;
	int Ys=4;
	int quan=1;
	int i=0;
	int startX=Xs-quan;
	int startY=Ys-quan;
	int lenOfMatrix=quan*2+1;
	int j=1;//控制每一条边上读取的个数
	for (i = 0; i < 4; i += 1)//这里的i表示遍历矩阵的4条边，开始从左上角向下遍历
	{
		for (j = 1; j < lenOfMatrix; j += 1)
		{
			if(similarityAroundOnePoint[startY][startX] ==1)
				ifHasSimilar++;

			//控制x,y的变化
			if(i == 0)
			{startY++;}
			if(i == 1)
			{startX++;}
			if(i == 2)
			{startY--;}
			if(i == 3)
			{startX--;}

		}
	}
		static  int here=0;


	if(ifHasSimilar <=0)
		return -1;
	//现在可以遍历最外圈了
	ifHasSimilar=0;//记录有没有相似点在这层上,da大于0说明有相似点
	quan=3;
	i=0;
	startX=Xs-quan;
	startY=Ys-quan;
	lenOfMatrix=quan*2+1;
	j=1;//控制每一条边上读取的个数
	for (i = 0; i < 4; i += 1)//这里的i表示遍历矩阵的4条边，开始从左上角向下遍历
	{
		for (j = 1; j < lenOfMatrix; j += 1)
		{
			if(similarityAroundOnePoint[startY][startX] ==1)
				ifHasSimilar++;

			//控制x,y的变化
			if(i == 0)
			{startY++;}
			if(i == 1)
			{startX++;}
			if(i == 2)
			{startY--;}
			if(i == 3)
			{startX--;}

		}
	}


	//manAllLine用来管理最终生成的cntHead表示的线条的链表集合，
	//但是我们首先得到的是一个posHead表示的链表集合,它需要在此函数结尾转为manAllLine
	LineMan  manAllLineIInPos;
	manAllLineIInPos.p.prev=&(manAllLineIInPos.p);
	manAllLineIInPos.p.next=&(manAllLineIInPos.p);
	manAllLineIInPos.cntHead=NULL;
	manAllLineIInPos.posHead=NULL;
	manAllLineIInPos.count=0;
	if(ifHasSimilar <= 0)//最外圈没有相似点的话，就直接保存数据，类似记录了图片上的一个小点
	{
		//待完成


		return -2;
	}
	ifHasSimilar = 0;
	//运行到这里说明最外圈和最内圈都有相似点了
	//首先创建这些点的一个链表
	//这里的矩阵遍历顺序与前面的需要有变化
	
	//这里首先会生成一个PosList的链表，用它来处理similarityAroundOnePoint矩阵中的相似点
	
	//成功生成则返回1，否则返回0
	int hasGetListOfPos=getsimilarPosFormMatr(&manAllLineIInPos);


	#ifdef NowIdDeBug
	DeBugmanAllLineIInPos=&manAllLineIInPos;
	#endif
	//接下来对headOfPosList，分离出在bmp中实际的线条
	return hasGetListOfPos;
}

//这个函数的最终目的是找到该矩阵内的最外围点,返回找到边缘节点的个数
int getsimilarPosFormMatr(/*PosList *headOfPosList*/LineMan * manAllLineIInPos)
{
	g_return_if_fail(manAllLineIInPos != NULL );
/*	g_return_if_fail((cnt->count) >=0 );	*/
	int res=0;
	
	//一开始，链表拥有的第一个节点就是中心点---错错错，不一定

	//首先创建第一条线的链表
	PosList *headOfPosList=malloc(sizeof(PosList));
	(headOfPosList->p).next=&(headOfPosList->p);
	(headOfPosList->p).prev=&(headOfPosList->p);

	//将链表加入manAllLineIInPos
	LineMan *newLineMan=(LineMan *)malloc(sizeof(LineMan));
	newLineMan->cntHead=NULL;
	newLineMan->posHead=headOfPosList;
	newLineMan->count=1;

	insertLineManNode(&(manAllLineIInPos->p),newLineMan);
	(manAllLineIInPos->count)=1;
	
	//一开始链表为空，首先判断第一个边缘点，方法如下
	//：从S点出发向上找到第一个边缘点（进入此函数的条件已经能够保证一定能找到这样的点，可能是S点本身）
	//把这个点加入链表，同时把经过的所以相似点但不是边缘点做标记

	//现在开始遍历了，显然遍历的顺序是依次按链表节点进行
	//DAN当遍历到某一个节点（设为X点）时，依次搜索该点附近的点（以一定的搜索次序：逆时针）
	//1：抛弃所以非相似点，如果是相似点，首先判断某点（设为y点）是否是我们已经判断为是相似点区域内部的点了
		//（你需要一个7*7的矩阵来记录所有已被判断为相似点的点，判断过的非相似点也可以记录）
	//2：如果该点不属于已经判断过的相似点，
		//则加入链表，因为你每次搜索的次序是一定的这保证了你搜索到边缘点的方向一定是逆时针的

	//先找到第一个边缘点
	PosList newPos;
	(newPos.pos.x)=4;
	(newPos.pos.y)=4;

	int isEdge=0;
	int Xs=4;
	int Ys=4;

	int i=0;//从S点出发向上找到第一个边缘点
	for (i = 0; i <= ((SimilarMatrix-2)/2); i += 1)
	{
		(newPos.pos.x)=Xs;
		(newPos.pos.y)=Ys-i;
		isEdge=ifIsEdge(&(newPos.pos));
		if(isEdge ==1)
		{	res++;
			break;
		}
				
	}
	//插入第一个节点
	insertPosList(&(headOfPosList->p),&newPos);

	//开始遍历链表
	//因为你的遍历次序为逆时针，所以在确定某个节点下一个边缘节点时候可能会漏掉其他边缘节点
	//所以你需要反复遍历链表，直到再也找不到边缘节点为止---这样就可能设计到节点插入的问题


	//每次开始之前需要保证similarityAroundOneMark上值都为0，除了第一个被插入的节点之外
	int j=1;
	i=1;
	for (i = 1; i < SimilarMatrix; i += 1)
	{
		for (j = 1; j < SimilarMatrix; j += 1)
		{
			if(i ==(newPos.pos.y) && j==(newPos.pos.x) )
				similarityAroundOneMark[i][j]=1;
			else
				similarityAroundOneMark[i][j]=0;
		}		
			
	}


	//遍历一遍链表，找寻是否有剩余的边缘点,判断是否要重新遍历
	//如果有这样的点存在就需要重新遍历
	//---是相似点，
	//---它能manAllLineIInPos中某条线能有接触
	//--它没有被标记
	//它不是区域内部的点--就是四周都被包围的点
	
	static here=0;
	int serchOverTime=0;
	int ifHaveTosearchPosList=1;



	while(ifHaveTosearchPosList == 1)
	{
/*printf("6666666666666\n");*/
		res=searchPosListForNewNode( manAllLineIInPos)+res;
		ifHaveTosearchPosList= hasPosToAddToLine(manAllLineIInPos);
		serchOverTime++;


		#ifdef NowIdDeBug
/*		if(DeBugindex == 50)*/
		{	
				PrintSimilarMatrix(NULL,DeBugW, NULL);	
/*				printf("serchOver=%d ,currentLineCount=%d\n",serchOverTime,manAllLineIInPos->count);*/
/*				printAllLineIInPos(manAllLineIInPos);*/
		}
		if(serchOverTime > 2)
		{
			break;
		}

		#endif
	}


	return res;
}


//遍历一遍链表，找寻是否有剩余的边缘点
int searchPosListForNewNode(LineMan * manAllLineIInPos)
{
	g_return_if_fail(manAllLineIInPos != NULL);
	g_return_if_fail((manAllLineIInPos->count) >0);
	int res=0;
	static here=0;
	//如果每次都是从头遍历，并且跳过已经被加入链表的点的话，很明显，在多次遍历以后就能遍历到所以的
	//相似点，问题是，节点的顺序，尤其是新加入节点不在末尾的时候
	//首先算法应该保证整个链表是表示的一条边缘线，而不因该有分支
	//如果重新遍历的时候，在遍历到点X的时候发现确实有相似点不适合放入原先的线上，就需要考虑新建一条线的链表
	//新建一条线的链表的条件就是，X点不是该链表的第一个节点，建立新链表后重新搜索X点
	//而不过是第一个节点，就是可以插入的时候，插入X点的前面，作为新的第一个节点，并从新节点开始重新遍历

	//-----这样处理的结果就是3叉型的线条将被分割为俩条线
		//十字型的将被分割为俩条线。。。。。。。
	//整个算法如下：
	//会对每一条线都遍历一遍
	PosList *currentPos=NULL;//当前正在搜索其周围获取下一个边缘点的位置
	LineMan *currentLineOfMan=NULL;//当前正在搜索的线条
	PosList * currentLineOfPos=NULL;
	PosList * newPosList=NULL;
	int x=0;
	int y=0;
	int i=1;
/*	int currentPosOfNUM=1;//当前点是链表中的第几个点，从1开始*/
	int j=0;
	for (i = 1; i <= (manAllLineIInPos->count); i += 1)
	{
		//获取第i条线
		currentLineOfMan=(LineMan *)list_GetNUMn( &(manAllLineIInPos->p),i);
/*		currentPosOfNUM=1;*/
		//获取第i条线的表头
		currentLineOfPos=(currentLineOfMan->posHead);

		//现在开始遍历该线条链表

		g_return_if_fail((currentLineOfMan->count) >=1);

		for(j=1;j<=(currentLineOfMan->count);j++)
		{
			//获取第i条线的第j个节点
			g_return_if_fail(currentLineOfPos != NULL);
			
			currentPos=(PosList *)list_GetNUMn( &(currentLineOfPos->p),j);

			g_return_if_fail(currentPos != NULL);
			g_return_if_fail(currentPos != currentLineOfPos);
			//获取该点的附近的一个相似的边缘点
			newPosList=getNextEdgePosList2( currentPos, currentLineOfPos);
			//newPosList不为空则需要判断是否需要插入一点

			if(newPosList)
			{
					x=(newPosList->pos).x;
					y=(newPosList->pos).y;

					g_return_if_fail(x >=1);
					g_return_if_fail(y >=1);
					g_return_if_fail(x <=(SimilarMatrix-1));
					g_return_if_fail(x <=(SimilarMatrix-1));
				//如果 currentPos是末尾，就插入末尾，如果是第一个
				//节点，就插入currentPos前面（j保持不变），否则新建链表
					similarityAroundOneMark[y][x]=1;
				//
				if(j == (currentLineOfMan->count))
				{
					list_add(&(newPosList->p), (currentLineOfPos->p).prev);
					(currentLineOfMan->count)++;

					continue;
				}
				if(j == 1)
				{
					list_add(&(newPosList->p),&(currentLineOfPos->p));
					(currentLineOfMan->count)++;
/*					similarityAroundOneMark[y][x]=1;*/
					j--;
					continue;
				}
				//新建链表
				//首先创建第一条线的链表
				PosList *headOfPosList=malloc(sizeof(PosList));
				(headOfPosList->p).next=&(headOfPosList->p);
				(headOfPosList->p).prev=&(headOfPosList->p);
				//插入第一个节点
/*				insertPosList(&(headOfPosList->p),newPosList);*/
				list_add(&(newPosList->p), &(headOfPosList->p));

				//将链表加入manAllLineIInPos
				LineMan *newLineMan=(LineMan *)malloc(sizeof(LineMan));
				newLineMan->cntHead=NULL;
				newLineMan->posHead=headOfPosList;
				newLineMan->count=1;

				list_add(&(newLineMan->p), (manAllLineIInPos->p).prev);
				(manAllLineIInPos->count)++;
/*				insertLineManNode(&(manAllLineIInPos->p),LineMan *Node)*/
			}
		}
	}


}
PosList * getNextEdgePosList2(PosList *currentPos,PosList * currentLineOfPos)
{

	g_return_if_fail(currentPos != NULL);
	g_return_if_fail(currentLineOfPos != NULL);
	//对于一个点能否作为下一个边缘点，除了getNextEdgePosList中的条件之外
	//还需要有一点---一个边缘点的加入是必须对原有的边缘点集合中的点所能接触到的
	//非相似点是有加成的，换句话说，如果新的边缘点所接触到的非相似点都是已有边缘点
	//所能接触到的，那么这个点不能作为新的边缘点
	PosList * newPosList=NULL;
	int x=(currentPos->pos).x;
	int y=(currentPos->pos).y;

	if(x <0 || y <0  || x >(SimilarMatrix-1) || y >(SimilarMatrix-1))
	{
		return newPosList;
	}
	int posX[9]={0,0,-1,-1,-1,0,1,1,1};//
	int posY[9]={0,-1,-1,0,1,1,1,0,-1};//
	int i=0;
	int nearX=0;//附近的点
	int nearY=0;
	for (i = 1; i < 9; i += 1)
	{
		//表示currentPos周围的一个点
		nearX=posX[i]+x;
		nearY=posY[i]+y;
		//判断是不是相似点
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//最后判断（nearX，nearY）该点能不能添加新的能接触到的非相似点
				//检查周围的非相似点，能否与线条中的点接触
				//只要有一个接触不到，就说明此点能作为新的边缘点


#ifdef NowIdDeBug
	DeBugnearX=nearX;
	DeBugnearY=nearY;	
	DeBugcurrentPosX=x;
	DeBugcurrentPosY=y;
	if(DeBugindex == 50 && nearX==3 && nearY == 5 && x ==3 && y==6)
	{
		int   posCount=countNode(&(currentLineOfPos->p));

		printf("DeBugindex=%d  lineCount=%d\n",DeBugindex,(DeBugmanAllLineIInPos->count));
		printf("posCount=%d\n",(posCount));

		PrintSimilarMatrix(NULL,DeBugW, NULL);
		printAllLineIInPos(DeBugmanAllLineIInPos);
	}

#endif
				//返回1表明有
				int ifc=ifCouldMeetNewNonSimilar(nearX,nearY,currentLineOfPos);
/*printf("ifc=%d,nearX=%d,nearY=%d \n",ifc,nearX,nearY);*/

#ifdef NowIdDeBug
	if(DeBugindex == 50 && x ==3 && y==6)
	{
		printf("ifc=%d,nearX=%d,nearY=%d \n",ifc,nearX,nearY);
	}
#endif

				if(ifc ==1)
				{

						newPosList=(PosList *)malloc(sizeof(PosList));
						(newPosList->pos).x=nearX;
						(newPosList->pos).y=nearY;
						return newPosList;			
				}
				
			}
		}	
			
	}

/*int ContactWithLine(int x,int y,LineMan * manAllLineIInPos)*/
	return newPosList;
}
//判断x,y作为一个相似点，其周围的非相似点有否与currentLineOfPos中的点接触
//如果存在一个（x,y）周围的非相似点没有与currentLineOfPos线条接触，则返回1
int ifCouldMeetNewNonSimilar(int x,int y ,PosList * currentLineOfPos)
{

//算法如下，该点能接触到的共有8点，依次判断该点（非类似点）是否有与链表中的点接触即可

	//构建一个LineMan，它里面就一条线条---currentLineOfPos
	LineMan  manAllLineIInPos2;
	manAllLineIInPos2.p.prev=&(manAllLineIInPos2.p);
	manAllLineIInPos2.p.next=&(manAllLineIInPos2.p);
	manAllLineIInPos2.cntHead=NULL;
	manAllLineIInPos2.posHead=NULL;
/*	manAllLineIInPos2.count=1;*/
	
	LineMan newLineMan;
	newLineMan.cntHead=NULL;
	newLineMan.posHead=currentLineOfPos;
	newLineMan.count=countNode(&(currentLineOfPos->p));

	insertLineManNode(&(manAllLineIInPos2.p),&newLineMan);
	(manAllLineIInPos2.count)=1;

	int posX[9]={0,0,-1,-1,-1,0,1,1,1};//
	int posY[9]={0,-1,-1,0,1,1,1,0,-1};//
	int i=0;
	int nearX=0;//附近的点
	int nearY=0;
	int hasContact=0;
	for (i = 1; i < 9; i += 1)
	{
		//表示currentPos周围的一个点
		nearX=posX[i]+x;
		nearY=posY[i]+y;
		//判断是不是相似点.//非相似点才会被比较
		if(ifIssimilarPosInMatrix(nearX,nearY) != 1)
		{
			//所有的（nearX，nearY）都接触不到，才说明此点能作为新的边缘点
			//为0表明不接触，为1表明（nearX，nearY）与线条有接触
			hasContact=ContactWithLine( nearX, nearY,  &manAllLineIInPos2);

#ifdef NowIdDeBug
	if(DeBugindex == 50 && DeBugnearX==3 && DeBugnearY == 5 && DeBugcurrentPosX ==3 && DeBugcurrentPosY==6)
	{
		printf("i=%d,hasContact=%d  \n",i,hasContact);

	}

#endif
			if(hasContact == 0)
			{
				return 1;
			}
			
		}

	}

	return 0;
}
PosList * getNextEdgePosList(PosList *currentPos)
{
	g_return_if_fail(currentPos != NULL);
	PosList * next=NULL;
	//获取一个点附近的下一个边缘点

	int x=(currentPos->pos).x;
	int y=(currentPos->pos).y;
	int nearX=0;//附近的点
	int nearY=0;
	PosList * newPosList=NULL;

	if(x <0 || y <0  || x >(SimilarMatrix-1) || y >(SimilarMatrix-1))
	{
		return newPosList;
	}

	//依次判断currentPos周围的点是否为边缘点，不用管是不是超除边界，因为ifIssimilarPosInMatrix会处理

	newPosList=(PosList *)malloc(sizeof(PosList));
		//上
		nearX=x;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}
		//左上
		nearX=x-1;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}
		//左
		nearX=x-1;
		nearY=y;		
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}
		//左下
		nearX=x-1;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}
		//下
		nearX=x;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}
		//右下
		nearX=x+1;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}
		//右
		nearX=x+1;
		nearY=y;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}
		//右上
		nearX=x+1;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 1)
		{
			//判断是否已经被标记
			if(similarityAroundOneMark[nearY][nearX] != 1)
			{
				//现在得到结果了
				(newPosList->pos).x=nearX;
				(newPosList->pos).y=nearY;
				return newPosList;
			}
		}

	if(newPosList)
		free(newPosList);
	return NULL;

}
int insertLineManNode2(struct list_head  *head,LineMan *Node)
{

	g_return_if_fail(head != NULL);
	g_return_if_fail(Node != NULL);
	LineMan * newNode=(LineMan *)malloc(sizeof(LineMan));
	if(newNode)
	{
		(newNode->cntHead)=(Node->cntHead);
		(newNode->posHead)=(Node->posHead);
		(newNode->count)=(Node->count);

		//就是在链表的tail和head之间插入一个节点，这样head->prev永远指向tail
		list_add_tail(&(newNode->p),head);
	}
	newNode=(LineMan *)head;
	if((newNode->count)>=0)
		(newNode->count)=(newNode->count)+1;
	else 
		(newNode->count)=1;
	return 0;
}
int insertLineManNode(struct list_head  *head,LineMan *Node)
{

	g_return_if_fail(head != NULL);
	g_return_if_fail(Node != NULL);
	LineMan * newNode=(LineMan *)malloc(sizeof(LineMan));
	if(newNode)
	{
		(newNode->cntHead)=(Node->cntHead);
		(newNode->posHead)=(Node->posHead);
		(newNode->count)=(Node->count);

		//就是在链表的tail和head之间插入一个节点，这样head->prev永远指向tail
		list_add_tail(&(newNode->p),head);
	}
	return 0;
}
int ifIsEdge(POS * pos)//只有四周都是形似点的时候才不是边缘点
{
	g_return_if_fail(pos != NULL);
	int res=0;
	int x=pos->x;
	int y=pos->y;
	int nearX=0;//附近的点
	int nearY=0;

	if(pos->x <0 || pos->y <0  || pos->x >(SimilarMatrix-1) || pos->y >(SimilarMatrix-1))
	{
		return 0;
	}
	//考虑一种特殊情况，就是该点恰好在矩阵边缘的时候
	//很明显这样的点就是边缘点
	if(x == 0 || y == 0 || y == (SimilarMatrix-1) || x == (SimilarMatrix-1))
	{
		return 1;
	}
	else
	{
		//现在判断该点的前后左右包括左上角，右下角。。。是否有相似点
	
		//该点周围一共是8个点，分成上中下3行

		//上
		nearX=x;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
		nearX=x-1;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
		nearX=x+1;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
		//中
		nearX=x+1;
		nearY=y;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
		nearX=x-1;
		nearY=y;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
		//下
		nearX=x-1;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
		nearX=x;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
		nearX=x+1;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 1;
	}

	return res;
}
//相似点返回1，不是为0，如果坐标不合法，返回2
int ifIssimilarPosInMatrix(int xx,int yy)
{
	int x=xx;
	int y=yy;
	int res=0;
	if(x <0 || y <0  || x >(SimilarMatrix-1) || y >(SimilarMatrix-1))
	{
		return 2;
	}

	if(similarityAroundOnePoint[y][x] ==0)
	{
		return 0;
	}
	else
		return 1;
	return res;
	
}
int insertPosList(struct list_head  *head,PosList *Node)
{

	g_return_if_fail(head != NULL);
	g_return_if_fail(Node != NULL);
	PosList * newNode=(PosList *)malloc(sizeof(PosList));
/*	ColorCnt * tail=NULL;*/
	if(newNode)
	{
		(newNode->pos).x=(Node->pos).x;
		(newNode->pos).y=(Node->pos).y;

		//就是在链表的tail和head之间插入一个节点，这样head->prev永远指向tail
		list_add_tail(&(newNode->p),head);
/*		printf("insertColorCnt is:");*/
/*		printColor(Col) ;*/
/*		printf(" \n");*/
	}
	return 0;
}
void PrintSimilarMatrix(ColorCnt * cnt,int w,unsigned char * bmpdata)
{
	int hang=1;
	int lie=1;
	int stride=getstrideOfBMP(w);//指的是长度，一定大于0
/*	int cnthang=getYFromX(cnt->count,stride);*/
/*	int cntlie=getXFromY(cnt->count,cnthang,stride);//从0开始*/
/*printf("stride is%d 中心点的位置(%d,%d)\n",stride,cntlie,cnthang);*/
/*printf("pos is %dcolor is (%d,%d,%d)\n",cnt->count,cnt->r,cnt->g,cnt->b);*/
	for (; hang < SimilarMatrix; hang += 1)
	{
		for (lie = 1; lie < SimilarMatrix; lie += 1)
		{
			if(lie ==1)
			{
				printf("\n");
/*				bmpdata[cnt->count]=0x00;*/
/*				bmpdata[cnt->count+1]=0x00;*/
/*				bmpdata[cnt->count+2]=0xff;*/
/*				bmpdata[cnt->count-3]=0x00;*/
/*				bmpdata[cnt->count+1-3]=0x00;*/
/*				bmpdata[cnt->count+2-3]=0xff;*/

				
			}		
			printf(" %d",similarityAroundOnePoint[hang][lie]);


		}
	}

printf("\n");
				


}
int CalculateSimilarMatrix(ColorCnt * cnt,int w,int l,unsigned char * bmpdata)
{
	g_return_if_fail(cnt != NULL );
	g_return_if_fail((cnt->count) >=0 );
	int countOFSimilar=0;
	//遍历一个矩阵
	int hang=1;
	int lie=1;
	//实际的hang和列
/*	int aclhang=0;*/
/*	int acllie=0;*/
	//	cnt点的行，列

	int stride=getstrideOfBMP(w);//指的是长度，一定大于0
	int cnthang=getYFromX(cnt->count,stride);
	int cntlie=getXFromY(cnt->count,cnthang,stride);//从0开始
	int bmpposX=0;
	int bmpposY=0;//这是附近点的坐标变量
	int PixelPosX=0;
	int PixelPosY=0;
	int posOfSimilarInBMP=0;//附近点在bmp文件中的实际位置
	ColorCnt SimilarCnt;
/*	int SimilarWith=0;    //与cnt比较的相似度*/
/*	int ifSimilar=0;//gai该点到底是不是相似点，为1相似，为0不相似*/
	//那么7*7矩阵左上角的坐标如下
	int PixelPosXOfZ=cntlie/3 -3;
	int PixelPosYOfZ=cnthang-3;

	for (; hang < SimilarMatrix; hang += 1)
	{
		for (lie = 1; lie < SimilarMatrix; lie += 1)
		{
			//先定义俩个名词
			//1：像素矩阵，--每个元素是一个像素----对应像素坐标（x1,y1）
			//2：bmp矩阵，--每个元素是一个r或g或b值---对应bmp坐标(x2,y2)
			//所以cnthang，cntlie是bmp坐标,而且有关系如下：y1=y2,x1=x2/3

			PixelPosX=PixelPosXOfZ+lie-1;
			PixelPosY=PixelPosYOfZ+hang-1;
			bmpposX=PixelPosX*3;
			bmpposY=PixelPosY;
			posOfSimilarInBMP=getPosFromXY(bmpposY,bmpposX,stride);
			SimilarCnt.r=bmpdata[posOfSimilarInBMP+2];
			SimilarCnt.g=bmpdata[posOfSimilarInBMP+1];
			SimilarCnt.b=bmpdata[posOfSimilarInBMP];
			//现在计算该点与中心点的相似度
			similarityAroundOnePoint[hang][lie]=CalculateSimilarTwoCol(cnt,&SimilarCnt);
			if(similarityAroundOnePoint[hang][lie]  ==1 )
			{
				countOFSimilar++;
			}
			
		}
	}

	if(countOFSimilar <2)
		countOFSimilar=0;
	else countOFSimilar=countOFSimilar-1;

	return countOFSimilar;
}
int CalculateSimilarTwoCol(ColorCnt * cnt1,ColorCnt * cnt2)
{
	//比较cnt2与cnt1的相识度，这里以cnt1为基准，就是cnt1是参照对象
	g_return_if_fail(cnt1 != NULL );
	g_return_if_fail(cnt2 != NULL );
	//定义相似度为0到100的一个值，为100说明俩个值完全相同，为0表示俩个值完全不相似
	int Similar=0;
	int ifSimilar=0;
	//计算俩个的距离
	int gap=compRGBWithCnt(cnt1,cnt2);
	//这里需要给出相似度
	//显然，从gap来说，gap是从0到一个255*sqrt（3）的一个变化范围
	//但是当gap超过100时已经完全可以认为是没有一点相似度的
	if(gap >100)
		gap=100;
	Similar=100-gap;
			//但是我们最终要知道该点到底是不是相似的
			if(Similar >= similarityOfTwoCol)
			{
				ifSimilar=1;
			}
			else
			{
				ifSimilar=0;
			}
	return ifSimilar;
}
//////////////////////////////////
/*s
findSpecialRGB的算法很重要，
:平均gap---p
：当前gap--c
因为gap的定义是空间俩点的距离，所以是越大则，差异越大

那么一个点与整个矩阵的平均值的差异大小如下定义：
chayi=|c-p|/p =|1-c/p|=c/p -1,chayi越大说明差异越大，（显然c大于p是才是符合要求的）
现在取chayi>0.3
-----------------------注意这个函数的作用不是找出所有的线条，只要能找到线条上的某些点就
			达到目的了，因为你是在一个个小矩阵里面找的，所以你找到的点是受到这个】		
			矩阵影响而不是该点周围的一个小范围的影响，当然你可以尝试这样去做
			就是遍历整个矩阵，对每一个点来进行一个判断，它与它周围是不是有较大差异
*/
///////////////////////////////////
int  findSpecialRGB(ColorCnt * spectialCol,Color *averageCol)
{
/*Color SubMatrixOfCol[21][21];//使用时可以不用0，0位置*/
/*POS SubMatrixOfPos[21][21];//使用时可以不用0，0位置*/
	g_return_if_fail(averageCol != NULL );
	g_return_if_fail(spectialCol != NULL );
/*	static int jfhere=1;*/
/*printf("jhere is %d\n",jfhere++);*/
	int gap=0;
	int averageGap=0;
	int hang=1;
	int lie=1;
	//遍历一个长宽都是SubJuZheng的矩阵----int 整数的取值范围是<2147483647，可以表示的最大bmp图片大小是1.5万个像素大小的图片
	//第一遍计算所有的gap值
	for(;hang<=SubJuZheng;hang++)
	{	

/*printf("\nhang is %d\n",hang);*/	
		for(lie=1;lie<=SubJuZheng;lie++)
		{
			gap=compRGB2(&(SubMatrixOfCol[hang][lie]),averageCol);
			g_return_if_fail(gap >=0 );
			SubMatrixOfGap[hang][lie]=gap;
			averageGap+=gap;
		}

	}
	
	averageGap=averageGap/((SubJuZheng*SubJuZheng));
	if(averageGap <= MinAverageGap)
	{
		return averageGap;
	}

/*	printf("averageGap is:%d\n",averageGap);*/

	double chayi=RGBDifferentX;
	//那么认为差距较大点的取值范围为(p即使平均gap)：      （1+chayi）*p<c  或者c<(1-chayi)*p
	int maxMargin=(1+chayi)*(double)averageGap;
/*	int minMargin=(1-chayi)*(double)averageGap;*/
	Color findCol;
	for(hang=1;hang<=SubJuZheng;hang++)
	{			
		for(lie=1;lie<=SubJuZheng;lie++)
		{
			gap=SubMatrixOfGap[hang][lie];
			if(gap >maxMargin )
			{
/*				if(hang==1 && lie ==8)*/
/*				{*/
/*				printf("gap is %d\n",gap);*/
				/*printf("averageCol is:%x,%x,%x \n",averageCol->r,averageCol->g,averageCol->b);*/
/*				printf("aulCol is:%x,%x,%x avecal is:%x,%x,%x\n",SubMatrixOfCol[hang][lie].r,SubMatrixOfCol[hang][lie].g,SubMatrixOfCol[hang][lie].b,averageCol->r,averageCol->g,averageCol->b);	*/
/*				printf("SubMatrixOfPos[hang][lie] is %d,hang %d,%d\n",SubMatrixOfPos[hang][lie],hang,lie);*/
/*				}*/
				

				//n你需要将这些点的位置保存下来,保存在一个链表中，
				//这里用ColorCnt.count来保存位置，
				#ifdef IfShowSpecial
				PointTobmpdata[SubMatrixOfPos[hang][lie]+2]= 0xff; //r
				PointTobmpdata[SubMatrixOfPos[hang][lie]+1]= 0x0; //g
				PointTobmpdata[SubMatrixOfPos[hang][lie]]= 0x0; //b
				#else
				struct list_head  *HeadPoint=NULL;
				if(spectialCol == NULL)
					return;
				HeadPoint=&spectialCol->p;
				findCol.r=PointTobmpdata[SubMatrixOfPos[hang][lie]+2];
				findCol.g=PointTobmpdata[SubMatrixOfPos[hang][lie]+1];
				findCol.b=PointTobmpdata[SubMatrixOfPos[hang][lie]];
						
				insertColorCnt(HeadPoint,&findCol);	
				//保存pos
				g_return_if_fail(HeadPoint->prev != HeadPoint );

				ColorCnt *lin=(ColorCnt *)(HeadPoint->prev);
				lin->count=SubMatrixOfPos[hang][lie];

				#endif
			}

		}

	}
	return 	averageGap;
}
int  freeListCnt(struct list_head  *cnt)
{
	g_return_if_fail(cnt != NULL );

	if(cnt->next == cnt || cnt->prev == cnt || cnt->next == NULL || cnt->prev == NULL)
	{
		INIT_LIST_HEAD(cnt);//重新初始化
		return 0;
	}

	struct list_head  *pos=cnt->next;
	struct list_head  *next=pos->next;
	ColorCnt * cntToFree=NULL;
/*	int ifEqual=0;*/

	
	while(pos->next !=cnt && pos->next !=NULL)
	{
/*		printf("cnt=%x,pos=%x \n",cnt,pos);*/
		cntToFree=(ColorCnt *)pos;
		free(cntToFree);
		pos=next;
		next=pos->next;
	}
	INIT_LIST_HEAD(cnt);//重新初始化
	return 0;
}
Color CalculateAverageOfColor(unsigned char * bmpdata,int *SubMatrixposInBMP,ColorCnt * cnt,int w,POS  *SubMatrix)
{
	
	g_return_if_fail(bmpdata != NULL);
	g_return_if_fail(SubMatrixposInBMP != NULL);
	g_return_if_fail(SubMatrix != NULL);	
	g_return_if_fail(cnt != NULL );
/*	printf("cnt=%x,p=%x,next=%x,prev=%x \n",cnt,&(cnt->p),((struct list_head *)(cnt))->next,(cnt->p).prev);*/
	g_return_if_fail(  ((cnt->p).next) == &(cnt->p)    );
	g_return_if_fail(  ((cnt->p).prev) == &(cnt->p));


	g_return_if_fail(w >=0);

/*	if(((cnt->p).next) != &(cnt->p))*/


	Color averageCol;//存储平均值
	InitialCOLOUR(averageCol);

	int stride=getstrideOfBMP(w);//指的是长度，一定大于0

	//其实只要知道左上角的值，就可以计算除4个角的虚拟坐标了
	int hang=0;
	int lie=0;
	POS actualPos;
	int indexInBMP=0;
	InitialPOS(actualPos);
	
	unsigned int r=0;
	unsigned int g=0;
	unsigned int b=0;
	//遍历一个长宽都是SubJuZheng的矩阵----int 整数的取值范围是<2147483647，可以表示的最大bmp图片大小是1.5万个像素大小的图片
	for(;hang<SubJuZheng;hang++)
	{	

/*printf("\nhang is %d\n",hang);*/
		
		for(lie=0;lie<SubJuZheng;lie++)
		{
/*printf("\nhang is %d,lie is %d\n",hang,lie);*/
		//首先计算该点的虚拟坐标，POS[0]正好可以利用
			actualPos.x=3*(SubMatrix[1].x+lie);
			actualPos.y=SubMatrix[1].y+hang;
			//现在计算该actualPos点在bmp中的下标
			 indexInBMP=GetPosFromXY(actualPos.x,actualPos.y,stride);
/*			if(hang==0 && lie==0)*/
/*				printf("SubMatrix[1].x is %d  SubMatrix[1].y is %d \n",SubMatrix[1].x,SubMatrix[1].y);*/
/*			printf("w is %d,actualPos.x is %d  actualPos.y is %d stride is %d\n",w,actualPos.x,actualPos.y,stride);*/
/*			if(hang==0 && lie==0)			*/
/*			printf("indexInBMP is %d\n",indexInBMP);*/
			 g_return_if_fail(stride*(SubMatrix[4].y+1) >= indexInBMP);
			//现在bmpdata中的数据还是BGR的
			r+=bmpdata[indexInBMP+2];
			g+=bmpdata[indexInBMP+1];
			b+=bmpdata[indexInBMP];
			averageCol.r=bmpdata[indexInBMP+2];
			averageCol.g=bmpdata[indexInBMP+1];
			averageCol.b=bmpdata[indexInBMP];
			//将此矩阵保存下来
			SubMatrixOfCol[hang+1][lie+1].r=averageCol.r;
			SubMatrixOfCol[hang+1][lie+1].g=averageCol.g;
			SubMatrixOfCol[hang+1][lie+1].b=averageCol.b;
			SubMatrixOfPos[hang+1][lie+1]=indexInBMP;
			//因为数据在SubMatrixOfCol保存，所以不会影响
			#ifdef IfPaintLie
			if(hang==0 || lie==0 || hang == (SubJuZheng))
			{
			bmpdata[indexInBMP+2]=0;
			bmpdata[indexInBMP+1]=0;
			bmpdata[indexInBMP]=0;

			}
			#endif
			//统计该像素出现的频率
			
/*			if(hang==0 && lie==0)*/
				CountSomeColourInSubMatrix(cnt,&averageCol);
		}




	} 
	averageCol.r=(int)r/(SubJuZheng*SubJuZheng);
	averageCol.g=(int)g/(SubJuZheng*SubJuZheng);
	averageCol.b=(int)b/(SubJuZheng*SubJuZheng);
/*printf("averageCol is:%x,%x,%d \n",averageCol.r,averageCol.g,averageCol.b);*/

	return averageCol;//???????????////
}
int  OutPutListCnt(struct list_head  *cnt,char * file,int mark)
{

	g_return_if_fail(cnt != NULL && cnt->next !=NULL);
	g_return_if_fail(file != NULL);
//先将文件清空


	struct list_head  *pos=cnt->next;
	ColorCnt * pcnt=NULL;

	if(pos==cnt)
	{

	return;
	}
	
	char str[40]="";
/*	printf("index=%d \n",index);*/
	sprintf(str,"index= %d \n",mark);
	addLineToFile(file,str);
	while(pos !=cnt && pos != NULL)
	{
		

		pcnt=(ColorCnt *)pos;
		sprintf(str,"	r=%x,g=%x,b=%x,count=%d \n",pcnt->r,pcnt->g,pcnt->b,pcnt->count);
		addLineToFile(file,str);

		pos=pos->next;

	}




	return 0;
}
int   countNode(struct list_head  *cnt)
{
	g_return_if_fail(cnt != NULL);

	struct list_head  *pos=cnt->next;
	ColorCnt * pcnt=NULL;
	int count=0;
	if(pos==cnt)
	{
/*	printf("now list is empty \n");*/
	return count;
	}
/*	printf("now list all spectialCol  \n");*/
	while(pos !=cnt && pos != NULL)
	{
		count++;
		pos=pos->next;

	}

		return count;


}
int  printListCnt(struct list_head  *cnt)
{

	g_return_if_fail(cnt != NULL && cnt->next !=NULL);

	struct list_head  *pos=cnt->next;
	ColorCnt * pcnt=NULL;
/*	int ifEqual=0;*/
	if(pos==cnt)
	{
/*	printf("now list is empty \n");*/
	return;
	}
	int  count= countNode(cnt);
	printf("now list all spectialCol  and all count is%d\n",count);
	while(pos !=cnt && pos != NULL)
	{
/*		printf("cnt=%x,pos=%x \n",cnt,pos);*/
		pcnt=(ColorCnt *)pos;
		printColorCNT(pcnt);
		pos=pos->next;

	}
}



int CountSomeColourInSubMatrix(ColorCnt * cnt,Color *averageCol)
{
//对于cnt链表中的数据要及时释放啊
/*	g_return_if_fail(cnt != NULL);*/
	g_return_if_fail(averageCol != NULL);

	struct list_head  *HeadPoint=NULL;
	ColorCnt * cntP=NULL;
	if(cnt == NULL)
		return;
	HeadPoint=&cnt->p;
	//查找averageCol是否在cnt中出现过
	
	struct list_head  * equalPos=searchListForColour(HeadPoint,averageCol);//HeadPoint是一个表头
	//找不到说明此颜色为出现过，需要添加
	if(equalPos == NULL)
	{
		//插入一个节点
/*		printf("插入一个节点 \n");*/
		 
		insertColorCnt(HeadPoint,averageCol);
		 
	}
	else
	{
		cntP=(ColorCnt *)equalPos;
		cntP->count=cntP->count+1;
/*		printf("find is:count is %d \n",cntP->count);*/
/*		printColor(averageCol) ;*/
/*		printf(" \n");		*/
	}

	return 0;
}
int insertColorCnt(struct list_head  *cnt,Color *Col)
{

	g_return_if_fail(cnt != NULL);
	g_return_if_fail(Col != NULL);
	ColorCnt * newcnt=(ColorCnt *)malloc(sizeof(ColorCnt));
/*	ColorCnt * tail=NULL;*/
	if(newcnt)
	{
		newcnt->r=Col->r;
		newcnt->g=Col->g;
		newcnt->b=Col->b;
		newcnt->count=1;
		//就是在链表的tail和head之间插入一个节点，这样head->prev永远指向tail
		list_add_tail(&(newcnt->p),cnt);
/*		printf("insertColorCnt is:");*/
/*		printColor(Col) ;*/
/*		printf(" \n");*/
	}
	return 0;
}
struct list_head  * searchListForColour(struct list_head  *point,Color *Col)//point是表头
{
/*	printListCnt(point);*/

	g_return_if_fail(point != NULL);
	g_return_if_fail(Col != NULL);

	if(point->next == point  ||  point->next == NULL)
		return NULL;
	struct list_head  *pos=point->next;
	ColorCnt * cnt=NULL;
	int ifEqual=0;
	while(pos !=point && pos != NULL)
	{
		
		cnt=(ColorCnt *)pos;
		ifEqual=equalCOlVsColorCnt(Col,cnt);
		if(ifEqual == 1)
		{
/*			printColor(Col);*/
			return pos;
		}
		pos=pos->next;
	}	

	return NULL;
}
int getSubMatrix(int index,unsigned char * bmpdata,int w,int l,int * posInBMP,POS  *SubMatrix)
{
	g_return_if_fail(SubMatrix != NULL);
	g_return_if_fail(bmpdata != NULL);
	g_return_if_fail(index >0);
	int stride=getstrideOfBMP(w);//指的是长度，一定大于0

	int numOfSubMatrixINWeight=w/(int)SubJuZheng;
	int numOfSubMatrixINHeight=l/(int)SubJuZheng;
/*	printf("index is%d ,可以容耐的小矩阵a*b=%d * %d \n",index,numOfSubMatrixINWeight,numOfSubMatrixINHeight);*/
	if(index > (numOfSubMatrixINWeight*numOfSubMatrixINHeight))
		return  0;
	//将index转化为对应的在bmpdata中的行列位置
	int hangOfSubMatrix=(index-1)/numOfSubMatrixINWeight +1;
	int lieOfSubMatrix=(index-1)%numOfSubMatrixINWeight+1;
/*printf("lieOfSubMatrix is %d,hangOfSubMatrix is%d\n",lieOfSubMatrix,hangOfSubMatrix);*/

	int hang=1;
	int lie=1;
	
	
	int pOfMatrix=1;
	for(;hang<=numOfSubMatrixINHeight;hang++)
	{	

/*printf("\nhang is %d\n",hang);*/
		
		for(lie=1;lie<=numOfSubMatrixINWeight;lie++)
		{
			pOfMatrix=1;
/*printf("lie is %d\n",hang);*/
			//注意这里的（x，y）指的是第几个像素点，但是在bmp中一个像素使用3个值保存的
			if(hang == hangOfSubMatrix  && lie== lieOfSubMatrix)
			{
			SubMatrix[pOfMatrix].x=(lie-1)*(SubJuZheng);//矩阵左上角坐标,x从0开始
			SubMatrix[pOfMatrix].y=(hang-1)*(SubJuZheng);//		y从0开始
			posInBMP[pOfMatrix]=GetPosFromXY(SubMatrix[pOfMatrix].x *3,SubMatrix[pOfMatrix].y,stride);

/*			posOfBMP[BMPLength][BMPLength];*/

			pOfMatrix++;

			SubMatrix[pOfMatrix].x=SubMatrix[pOfMatrix-1].x+SubJuZheng-1;//矩阵右上角坐标,x从0开始
			SubMatrix[pOfMatrix].y=SubMatrix[pOfMatrix-1].y;//		y从0开始
			posInBMP[pOfMatrix]=GetPosFromXY(SubMatrix[pOfMatrix].x *3,SubMatrix[pOfMatrix].y,stride);
			pOfMatrix++;		

			SubMatrix[pOfMatrix].x=SubMatrix[pOfMatrix-2].x;//矩阵z左下角坐标,x从0开始
			SubMatrix[pOfMatrix].y=SubMatrix[pOfMatrix-2].y+SubJuZheng-1;//		y从0开始
			posInBMP[pOfMatrix]=GetPosFromXY(SubMatrix[pOfMatrix].x *3,SubMatrix[pOfMatrix].y,stride);
			pOfMatrix++;	

			SubMatrix[pOfMatrix].x=SubMatrix[pOfMatrix-2].x;//矩阵z右下角坐标,x从0开始
			SubMatrix[pOfMatrix].y=SubMatrix[pOfMatrix-1].y;//		y从0开始
			posInBMP[pOfMatrix]=GetPosFromXY(SubMatrix[pOfMatrix].x *3,SubMatrix[pOfMatrix].y,stride);
			pOfMatrix++;

/*printf("第%d * %d 个矩阵坐标是：（%d，%d），（%d，%d）（%d，%d）（%d，%d）\n",lie,hang*/
/*,SubMatrix[1].x,SubMatrix[1].y,SubMatrix[2].x,SubMatrix[2].y,SubMatrix[3].x,SubMatrix[3].y,SubMatrix[4].x,SubMatrix[4].y);*/

			return  1;
			}
			
		}
	}


	return 0;


}


void printAllLineIInPos(LineMan *manAllLineIInPos)
{

	int countOfline=manAllLineIInPos->count;
	int i;
	printf("\n");
	printf("allline =%d\n",countOfline);
	for (i = 1; i <= countOfline; i += 1)
	{
		LineMan *  l=(LineMan *)list_GetNUMn(&(manAllLineIInPos->p),i);
		printf("line i=%d\n",i);
		printPosList( &((l->posHead)->p));
	}
	printf("over\n");
}
int  printPosList(struct list_head *headOfPosList)
{

	g_return_if_fail(headOfPosList != NULL && headOfPosList->next !=NULL);

	struct list_head  *pos=headOfPosList->next;
	PosList * Node=NULL;

	if(pos==headOfPosList)
	{
	printf("now headOfPosList is empty \n");
	return 0;
	}
	int  count= countNode(headOfPosList);
/*	printf("now list all PosList  and all count is%d\n",count);*/
	while(pos !=headOfPosList && pos != NULL)
	{
/*		printf("cnt=%x,pos=%x \n",cnt,pos);*/
		Node=(PosList *)pos;
		printf("x=%d,and y= %d\n",(Node->pos).x,(Node->pos).y);
		pos=pos->next;

	}
	return 0;
}

//是内部点时返回1
int hideInArea(int x,int y)
{

	int nearX=0;//附近的点
	int nearY=0;

	if(x <0 || y <0  || x >(SimilarMatrix-1) ||y >(SimilarMatrix-1))
	{
		return 0;
	}

	if(x == 0 || y == 0 || y == (SimilarMatrix-1) || x == (SimilarMatrix-1))
	{
		return 0;
	}
	//现在判断该点的前后左右包括左上角，右下角。。。是否有相似点，只要发现一个不是，就说明该点
	//不是内部点
	
		//该点周围一共是8个点，分成上中下3行

		//上
		nearX=x;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;
		nearX=x-1;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;
		nearX=x+1;
		nearY=y-1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;
		//中
		nearX=x+1;
		nearY=y;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;
		nearX=x-1;
		nearY=y;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;
		//下
		nearX=x-1;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;
		nearX=x;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;
		nearX=x+1;
		nearY=y+1;
		if(ifIssimilarPosInMatrix(nearX,nearY) == 0)
			return 0;

	return 1;
}
//判断x,y会不会与manAllLineIInPos中的点接触,hui会接触返回1
int ContactWithLine(int x,int y,LineMan * manAllLineIInPos)
{
	PosList *currentPos=NULL;//当前正在搜索其周围获取下一个边缘点的位置
	LineMan *currentLineOfMan=NULL;//当前正在搜索的线条
	PosList * currentLineOfPos=NULL;

	if(x<=0 || y<=0 || x>=(SimilarMatrix) || y>=(SimilarMatrix))
		return 0;


	int i=1;
	int j=0;
	int cha1=0;
	int cha2=0;
	for (i = 1; i <= (manAllLineIInPos->count); i += 1)
	{
		//获取第i条线
		currentLineOfMan=(LineMan *)list_GetNUMn( &(manAllLineIInPos->p),i);
/*		currentPosOfNUM=1;*/
		//获取第i条线的表头
		currentLineOfPos=(currentLineOfMan->posHead);

		//现在开始遍历该线条链表
		g_return_if_fail((currentLineOfMan->count) >=1);

		for(j=1;j<=(currentLineOfMan->count);j++)
		{
			//获取第i条线的第j个节点
			g_return_if_fail(currentLineOfPos != NULL);
			
			currentPos=(PosList *)list_GetNUMn( &(currentLineOfPos->p),j);


			//判断currentPos与x,y能否接触
			//如果俩个点的坐标值差俩个单位就不会接触
			cha1=x-((currentPos->pos).x);
			cha2=y-((currentPos->pos).y);
			cha1=abs(cha1);
			cha2=abs(cha2);			
/*#ifdef NowIdDeBug*/
/*	if(DeBugindex == 50 && DeBugnearX==3 && DeBugnearY == 5 && DeBugcurrentPosX ==3 && DeBugcurrentPosY==6)*/
/*	{*/
/*		printf("lineCount=%d ,j=%d,poscount=%d\n",i,manAllLineIInPos->count,j,(currentLineOfMan->count));*/

/*		printf("x=%d,y=%d \n",x,y);*/
/*		printf("(cx=%d,cy=%d \n",(currentPos->pos).x,(currentPos->pos).y);*/
/*		printf("cha1=%d,cha2=%d \n",cha1,cha2);*/

/*	}*/

/*#endif*/
			if(cha1 <=1 && cha2 <=1)		
				return 1;
		}
	}

	return 0;

}

int hasPosToAddToLine(LineMan * manAllLineIInPos)
{
	int haveTo=0;
	int i=0;
	int j=0;
	int ishideInArea=0;
	int iscontactWithLine=0;
	static here=0;
	for(j=1;j<=(SimilarMatrix-1);j++)
	{
		for(i=1;i<=(SimilarMatrix-1);i++)
		{
			if(similarityAroundOnePoint[j][i] == 1 && similarityAroundOneMark[j][i] == 0)
				{
					//判断是不是内部点
					
					ishideInArea=hideInArea(i,j);
					//判断能不能与链表中的点接触
					iscontactWithLine=ContactWithLine(i,j,manAllLineIInPos);
					//B不是内部点，且能与链表线接触时需要再次遍历

#ifdef NowIdDeBug
/*if(DeBugindex == 50)*/
/*{*/
/*printf("j=%d,i=%d \n",(j),i);*/
/*printf("ishideInArea=%d,iscontactWithLine=%d \n",(ishideInArea),iscontactWithLine);*/
/*	if(DeBugindex >0)*/
/*	{*/

/*		PrintSimilarMatrix(NULL,DeBugW, NULL);*/
/*		printAllLineIInPos(manAllLineIInPos);*/
/*	}*/
/*}*/
#endif


					if(ishideInArea ==0 && iscontactWithLine ==1)
						return 1;
				}

		}

	}
	return 0;
}










