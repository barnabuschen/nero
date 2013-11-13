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

void scanBMP(unsigned char * bmpdata,int w,int l)
{
	g_return_if_fail(bmpdata != NULL);
	int numOfSubMatrixINWeight=w/(int)SubJuZheng;
	int numOfSubMatrixINHeight=l/(int)SubJuZheng;
	printf("可以容耐的小矩阵a*b=%d * %d \n",numOfSubMatrixINWeight,numOfSubMatrixINHeight);

	LineMan  manAllLine;
	manAllLine.p.prev=&(manAllLine.p);
	manAllLine.p.next=&(manAllLine.p);
	manAllLine.cntHead=NULL;

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
/*			if(index ==1)*/		
			Color averageCol= CalculateAverageOfColor(bmpdata,posInBMP,&cnt,w,SubMatrix);
					
			//打印链表中的数据
/*			printListCnt(&(cnt.p));*/
			if(index <55)
			{
			printf("index=%d,averageCol is:%x,%x,%x \n",index,averageCol.r,averageCol.g,averageCol.b);
			}

			//是时候寻找突出点了：就是与周围颜色值有较大差异的点
			//你会发现如果averageGap<=1的话，基本上可以认为这里面没有特别特殊的点了
			int averageGap=findSpecialRGB( &spectialCol,&averageCol);
			if(index <55)
			{
				printf("averageGap=%d\n",averageGap);
				printListCnt(&(spectialCol.p));	
				OutPutListCnt(&(cnt.p),"Data/output",index);

			}
			//现在可以开始搜索线条了
			//因为在findSpecialRGB获取的会有很多个spectialCol，可能有多条可以搜索到
			//的线条
			//你可以借鉴slab管理的方式，获取的每一条线条都用一个单独的链表表示
			//然后用一个单独的链表来统一管理每条线条的head
			int findLineCnt=0;
			if(list_ISempty(&(spectialCol.p)) ==0)
				{
					findLineCnt=searchLineBySpectialColPoint(&spectialCol,bmpdata,& manAllLine,w,l);
/*					printf("ssssssss");*/
					
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
		CalculateSimilarMatrix(cnt,w,l,bmpdata);
	
		here++;
		if(here ==1)
		{
			printf("here is %d\n",here);
			PrintSimilarMatrix(cnt,w, bmpdata);
		}


		pos=pos->next;
	}





	return findLineCnt;
}
void PrintSimilarMatrix(ColorCnt * cnt,int w,unsigned char * bmpdata)
{
	int hang=1;
	int lie=1;
	int stride=getstrideOfBMP(w);//指的是长度，一定大于0
	int cnthang=getYFromX(cnt->count,stride);
	int cntlie=getXFromY(cnt->count,cnthang,stride);//从0开始
printf("stride is%d 中心点的位置(%d,%d)\n",stride,cntlie,cnthang);
printf("pos is %dcolor is (%d,%d,%d)\n",cnt->count,cnt->r,cnt->g,cnt->b);
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

			
		}
	}




	return 0;
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
	return Similar;
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
void  OutPutListCnt(struct list_head  *cnt,char * file,int mark)
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
void  printListCnt(struct list_head  *cnt)
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















