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

#include <malloc.h>
#include "bmpRead.h"
#include "../common/fileOperating.h"

#ifndef  LIST_INCLUDE
#include "../common/list.h"
#endif

#include "searchLine.h"
extern int getstrideOfBMP(int w);
extern   void INIT_LIST_HEAD(struct list_head *list);
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
	g_return_if_fail(color1 != NULL);
	g_return_if_fail(color2 != NULL);
	int r=(int )(color1->r)-(int )(color2->r);
	int g=(int )(color1->g)-(int )(color2->g);
	int b=(int )(color1->b)-(int )(color2->b);

	//相似度的定义是需要仔细考虑的

	r= (r<0)? (-1)*r:r;
	g= (g<0)? (-1)*g:g;
	b= (b<0)? (-1)*b:b;	

	return (r+g+b);
}
inline int getXFromY(int pos,int y,int LengthOfLine)
{

	return (pos-y*LengthOfLine);

}
inline int getYFromX(int pos,int x,int LengthOfLine)
{

	return (pos-x)/LengthOfLine;

}
inline int getPosFromXY(int y,int x,int LengthOfLine)
{

	return (x+y*LengthOfLine);

}

void scanBMP(unsigned char * bmpdata,int w,int l)
{
	g_return_if_fail(bmpdata != NULL);
/*	printf("ssssssssssss");*/
	
/*	int stride=getstrideOfBMP(w);//指的是长度，一定大于0*/


/*	int zz=getXFromY(3,1,stride);*/
	
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
/*	printf("p=%x,next=%x,prev=%x ,&cnt.p.prev=%x \n",&(cnt.p),cnt.p.next,cnt.p.prev,&cnt.p.prev);*/
/*	InitialColorCnt(cnt);*/
/*	INIT_LIST_HEAD(&(cnt.p));*/


	POS  SubMatrix[5];//o不用：这个用来子矩阵在databmp中虚拟的坐标，可以用POS来计算除该点在bmpdata实际的位置(对应值在posInBMP中)
				//注意它存储的是虚拟坐标，如SubMatrix[1]的xy都为1时表示的是从上到下，从左到右数，像素值序数为1的点


	int index=1;
	for(;index>0;index++)
	{
		if(index <25)
		printf("index=%d\n",index);
		int res=getSubMatrix(index, bmpdata, w, l, posInBMP,SubMatrix);

		if(res ==1)
		{
			//计算该区域的像素平均值
/*			INIT_LIST_HEAD( &list);*/
			//list->next=cnt.p;

			//每次进入此函数要保证cnt是个空列表
			
			freeListCnt(&(cnt.p));
/*			if(index ==1)*/
			
			Color averageCol= CalculateAverageOfColor(bmpdata,posInBMP,&cnt,w,SubMatrix);
					
			//打印链表中的数据
/*			printListCnt(&(cnt.p));*/
			if(index <25)
			printf("	averageCol is:%x,%x,%x \n",averageCol.r,averageCol.g,averageCol.b);


			//是时候寻找突出点了：就是与周围颜色值有较大差异的点
/*			if(index ==8)*/
				findSpecialRGB(&averageCol);
		}
		else  break;

	}



	if(scanMark)
		free(scanMark);
/*	malloc(SubJuZheng);*/


}
void  findSpecialRGB(/*Color *SubMatrixOfCol,*/Color *averageCol)
{
/*Color SubMatrixOfCol[21][21];//使用时可以不用0，0位置*/
/*POS SubMatrixOfPos[21][21];//使用时可以不用0，0位置*/
	g_return_if_fail(averageCol != NULL );
	

	int gap=0;
	int hang=1;
	int lie=1;
	//遍历一个长宽都是SubJuZheng的矩阵----int 整数的取值范围是<2147483647，可以表示的最大bmp图片大小是1.5万个像素大小的图片
	for(;hang<=SubJuZheng;hang++)
	{	

/*printf("\nhang is %d\n",hang);*/
		
		for(lie=1;lie<=SubJuZheng;lie++)
		{
			gap=compRGB2(&(SubMatrixOfCol[hang][lie]),averageCol);
			g_return_if_fail(gap >=0 );

			if(gap >=150)
			{
if(hang==1 || lie==1)
{
printf("gap is %d\n",gap);
/*printf("averageCol is:%x,%x,%x \n",averageCol->r,averageCol->g,averageCol->b);*/
printf("aulCol is:%x,%x,%x \n",SubMatrixOfCol[hang][lie].r,SubMatrixOfCol[hang][lie].g,SubMatrixOfCol[hang][lie].b);	
}
			



				PointTobmpdata[SubMatrixOfPos[hang][lie]+2]= 0xff; //r
				PointTobmpdata[SubMatrixOfPos[hang][lie]+1]= 0x0; //g
				PointTobmpdata[SubMatrixOfPos[hang][lie]]= 0x0; //b
			}

		}

	}
		
}
void  freeListCnt(struct list_head  *cnt)
{
	g_return_if_fail(cnt != NULL );

	if(cnt->next == cnt || cnt->prev == cnt || cnt->next == NULL || cnt->prev == NULL)
	{
		INIT_LIST_HEAD(cnt);//重新初始化
		return;
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
			if(hang==0 || lie==0 || hang == (SubJuZheng))
			{
			bmpdata[indexInBMP+2]=0;
			bmpdata[indexInBMP+1]=0;
			bmpdata[indexInBMP]=0;

			}
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

void  printListCnt(struct list_head  *cnt)
{

	g_return_if_fail(cnt != NULL && cnt->next !=NULL);

	struct list_head  *pos=cnt->next;
	ColorCnt * pcnt=NULL;
/*	int ifEqual=0;*/
	if(pos==cnt)
	{
	printf("now list is empty \n");
	return;
	}
	printf("now list all node \n");
	while(pos !=cnt && pos != NULL)
	{
/*		printf("cnt=%x,pos=%x \n",cnt,pos);*/
		pcnt=(ColorCnt *)pos;
		printColorCNT(pcnt);
		pos=pos->next;

	}
}



void CountSomeColourInSubMatrix(ColorCnt * cnt,Color *averageCol)
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


}
void insertColorCnt(struct list_head  *cnt,Color *Col)
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















