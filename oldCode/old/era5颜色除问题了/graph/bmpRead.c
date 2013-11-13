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
//#include <gtk/gtk.h>
#include <assert.h>

#include <malloc.h>
#include "bmpRead.h"
#include "../common/fileOperating.h"

extern char *  titlebmp;
extern char * getStrInFile(char * FileName,int len,int point);
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
	
	printf("biSizeImage is %d \n",msg->biSizeImage);
	unsigned char * data=(unsigned char *)getStrInFile(FileName,msg->biSizeImage,54);
	*dataCount=msg->biSizeImage;
	printf("dataCount is %d \n",*dataCount);
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
