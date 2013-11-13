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

extern char *  titlebmp;
extern char * getStrInFile(char * FileName,int len,int point);

int getstrideOfBMP(int w)
{
	g_return_if_fail(w >0);
	int i=0;

        while((w * 3 +i)%4 != 0)//203*3=609---612
                i++;
 	return (w * 3 +i);



}
unsigned char * ChangeRGBToRGBA(unsigned char * bmpdata,int w,int h,int *newW,int ifPutInZheng)
{

	//扩展为w可以被4整除
	int i=0;
/*	int w=msg.biWidth;*/
	int l=h;
        while((w * 3 +i)%4 != 0)
                i++;
	unsigned char * newbmpdata2=addLieToBMP( bmpdata, w * 3 +i,l,i*2);
	w+=i;
	*newW=w;
		//尝试转化为RGBA
/*	int newstride=w*3;*/
	unsigned char * newbmpdata =putBMPRGBtoRGBA( newbmpdata2, w,l);


	int stride=w*4;
	if(ifPutInZheng ==1)
	{
	putBMPInZheng(newbmpdata,stride,l);
	}





/*	paintInSurfaceRGB24(image,w,l, newbmpdata);*/

/*	if(bmpdata)*/
/*		free(bmpdata);*/
	if(newbmpdata2)
		free(newbmpdata2);
/*	if(newbmpdata)*/
/*		free(newbmpdata);	*/


	return newbmpdata;

}
unsigned char * addLieToBMP(unsigned char * bmpdata,int w,int h,int lie)
{


	int newHangData=(w+lie);//转后一行的数据
	int j=0;
	int i=0;
/*	unsigned char tmp;*/
	int pos;
	int pos2=0;
	unsigned char * newbmpdata=malloc(sizeof(unsigned char)*(h*newHangData));
	for(pos=0;j<h;j++)
	{
		//printf("hangInData is %d,w is :%d,h is %d,pos is %d \n",hangInData,w,h,pos);
		for(i=0,pos=w*j;i<newHangData;i++,pos++)
		{
			if(i<w)
			{

				newbmpdata[pos2++]=bmpdata[pos];
			}
			else    newbmpdata[pos2++]=0x55;
		}

	}
	return newbmpdata;


}
 void putBMPRGBtoBGR(unsigned char * bmpdata,int w,int h)
{
	#ifndef NoIncludeG_return_if_fail 
	g_return_if_fail(bmpdata1 != NULL);
	#endif
/*	g_return_if_fail(bmpdata != NULL);*/
        int i=0;
        int hangInData=0;
        //首先找到让msg.biWidth * 3 +i能被4整除的数
        i=0;
        while((w * 3 +i)%4 != 0)
                i++;
        hangInData=i+w * 3;


	int j=0;
	i=0;
	unsigned char tmp;
	int pos;
/*	int pos2=0;*/

	for(pos=0;j<h;j++)
	{
		//printf("hangInData is %d,w is :%d,h is %d,pos is %d \n",hangInData,w,h,pos);
		for(i=0,pos=w*j*3;i<hangInData;i=i+3,pos+=3)
		{
			if(i<(w*3))
			{
				tmp=bmpdata[pos+2];
				bmpdata[pos+2]=bmpdata[pos];
				bmpdata[pos]=tmp;
			}
		}

	}
/*	return newbmpdata;*/
}
unsigned char * putBMPRGBtoRGBA(unsigned char * bmpdata,int w,int h)
{
	#ifndef NoIncludeG_return_if_fail 
	g_return_if_fail(bmpdata1 != NULL);
	#endif
/*	g_return_if_fail(bmpdata != NULL);*/
        int i=0;
        int hangInData=0;
        //首先找到让msg.biWidth * 3 +i能被4整除的数
        i=0;
        while((w * 3 +i)%4 != 0)
                i++;
        hangInData=i+w * 3;

	int newHangData=w*4;//转后一行的数据,转后一定能被4整除啊
	int j=0;
	i=0;
/*	unsigned char tmp;*/
	int pos;
	int pos2=0;
	unsigned char * newbmpdata=malloc(sizeof(unsigned char)*(h*newHangData));
	for(pos=0;j<h;j++)
	{
/*		printf("hangInData is %d,w is :%d,h is %d,pos is %d \n",hangInData,w,h,pos);*/
		for(i=0,pos=w*j*3;i<hangInData;i=i+3,pos+=3)
		{
			if(i<(w*3))
			{

				newbmpdata[pos2++]=bmpdata[pos];
				newbmpdata[pos2++]=bmpdata[pos+1];
				newbmpdata[pos2++]=bmpdata[pos+2];

				newbmpdata[pos2++]=0xff;
/*				newbmpdata[pos2++]=0x79;//b*/
/*				newbmpdata[pos2++]=0xa1;//g*/
/*				newbmpdata[pos2++]=0x33;//r*/
/*				newbmpdata[pos2++]=0xff;*/
			}
		}

	}
	return newbmpdata;
}
void putBMPInZheng(unsigned char * newbmpdata,int w,int h)
{

	int halfHang=(h)/2;
	int i=0,lie=0;
	int hang=0;
	int hang2=0;
	unsigned char linshicc=0;
	while(hang<halfHang)
	{

		hang=i/(w);
		lie=i%(w);
		hang2=(h-hang-1 );
		int linshixx=lie+hang2* (w);
/*		if(lie < 10)*/
/*		printf("i is %d hang is %d lie  is %d  hang2 is %d linshixx is %d \n",i,hang,lie,hang2,linshixx);*/

		linshicc=newbmpdata[linshixx];
		newbmpdata[linshixx]=newbmpdata[i];
		newbmpdata[i]=linshicc;
		i++;
	}




}
unsigned char * readBmpData(char * FileName,unsigned int * dataCount,bmpHead * head,bmpMsg * msg)
{

/*	bmpMsg head;*/
/*	bmpMsg msg;*/
	getBmpMsg(FileName,head,msg);
	
	//
	
/*	("biSizeImage is %d \n",msg->biSizeImage);*/
	unsigned char * data=(unsigned char *)getStrInFile(FileName,msg->biSizeImage,54);
	*dataCount=msg->biSizeImage;
/*	printf("dataCount is %d \n",*dataCount);*/
	return data;

}
void printBmpMsg(PbmpHead head,PbmpMsg msg)
{
	assert(head);
	assert(msg);

	printf("整个文件大小 : %d \n",head->bfSize);
	printf("记录图像数据区的起始位置 : %x \n",head->bfOffBits);
	printf("像素x : %d \n",msg->biWidth);
	printf("像素y : %d \n",msg->biHeight);
	printf("每个像素所需的位数 : %d \n",msg->biBitCount);
	printf("图像区数据的大小。单位字节 : %d \n",msg->biSizeImage);
	//printf("整个文件大小 : %d \n",head->bfSize);
	

}
void getBmpMsg(char * FileName,PbmpHead head,PbmpMsg msg)
{
	assert(FileName);
	assert(head);
	assert(msg);
	char * linshi=getStrInFile(FileName,0x36,0);
	printf("linshi is : %s \n",linshi);
	if(linshi)
	{

		//unsigned char * linshi[37];
		

		//整个文件大小，单位字节。	
		head->bfSize=putNumFourINOne(linshi[5],linshi[4],linshi[3],linshi[2],unsigned int);
		//记录图像数据区的起始位置。从文件开始到位图数据(bitmap data)之间的偏移量。 
		head->bfOffBits=putNumFourINOne(linshi[13],linshi[12],linshi[11],linshi[10],unsigned int);

		
		//x像素
		msg->biWidth=putNumFourINOne(linshi[21],linshi[20],linshi[19],linshi[18],unsigned int);
		msg->biHeight=putNumFourINOne(linshi[25],linshi[24],linshi[23],linshi[22],unsigned int);

		//每个像素所需的位数，必须是1(双色),4(16色)，8(256色)或24(真彩色)之一
		msg->biBitCount=putNumTwoINOne(linshi[29],linshi[28],short);

		//图像区数据的大小。单位字节，该数必须是4的倍数。
		msg->biSizeImage=putNumFourINOne(linshi[37],linshi[36],linshi[35],linshi[34],unsigned int);
		free(linshi);
	}
}

/*int main(int argc, char *argv[])*/
/*{*/
/*//int c=sizeof(int);*/

/*//printf("%d \n", ~(unsigned int)0 / 2);*/
/*printf("sssss \n");*/

/*bmpHead head;*/
/*bmpMsg info;*/
/*	char msg[37];*/
/*	msg[36]='\0';*/
/*	int len=36;*/
/*	int point=0;*/
/*	char * ss=titlebmp;*/
/*	printf("titlebmp is %s \n",titlebmp);*/
/*	getBmpMsg(titlebmp,& head,& msg);*/
/*	printBmpMsg(& head,& msg);*/
/*	*/




/*return 0;*/

/*}*/
