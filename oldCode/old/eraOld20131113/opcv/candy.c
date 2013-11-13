#ifdef _CH_
#pragma package <opencv>
#endif
 
#ifndef _EiC

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#endif

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

 #include "candy.h"
#include "../common/fileOperating.h"

char wndname[] = "Edge";
char tbarname[] = "Threshold";
char wndname1[] = "Edge1";
char wndname2[] = "Edge2";
char wndname3[] = "Edge3";
char wndname4[] = "Edge4";

extern int addLineToFile(char * FileName,char * str);
char * getLineInFile(char * FileName,int num);//返回获取的字符串

IplImage *image = 0, *cedge = 0, *gray = 0, *edge = 0;
void printPIcMsg(IplImage *image)
{

	printf("nSize =%d\n",image->nSize);
	printf("ID =%d\n",image->ID);
	printf("nChannels =%d\n",image->nChannels);
	printf("depth =%d\n",image->depth);
	printf("dataOrder =%d\n",image->dataOrder);
	printf("width =%d\n",image->width);
	printf("height =%d\n",image->height);
	printf("imageSize =%d\n",image->imageSize);//图像数据大小
	printf("widthStep =%d\n",image->widthStep);


}

 void candyTest(char * filename,int h,shareData *pMapWithCandy)
{

	printf("candyTest filename %s,size =%d\n", filename,sizeof(filename));
	char newFile[50]="../";
	strcat((newFile+3),filename);
/*	printf("candyTest newFile %s,size =%d\n", newFile,sizeof(newFile));*/


    int edge_thresh = 1; 	

    if( (image = cvLoadImage( newFile, 1)) == 0 )
	{
	printf("image error \n");
        return ;	
	}
	    img=cvLoadImage(newFile,0);  
    // Create the output image
    cedge = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 3);
 
    // Convert to grayscale
    gray = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);//1意思是说:虽然有RGB,但3个的										//值一样
										//他们同时表示一个通道
    edge = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
    cvCvtColor(image, gray, CV_BGR2GRAY);

    // Create a window
/*    cvNamedWindow(wndname, 1);*/
/*    cvNamedWindow(wndname1, 1);*/
    // create a toolbar 
    cvCreateTrackbar(tbarname, wndname, &edge_thresh, 100, on_trackbar);
 
    // Show the image
	    on_trackbar(70);

	//打印图像参数，并复制数据
/*	printf("image msg \n");*/
/*	printPIcMsg(image);*/


/*	pMapWithCandy->width=image->width;*/
/*	pMapWithCandy->height=image->height;*/
/*	pMapWithCandy->imageSize=image->imageSize;*/
/*	pMapWithCandy->widthStep=image->widthStep;*/

/*	memcpy( &((*(pMapWithCandy)).data),cedge->imageData,cedge->imageSize);*/
	int len=cedge->imageSize;
	int i=0;

/*	for (i = 0; i < len; i += 1)*/
/*	{*/

/*		*((cedge->imageData)+i)=0xa7;*/
/*		*/
/*	}*/
	
/*	memcpy( &((*(pMapWithCandy)).data),edge->imageData,edge->imageSize);*/


    // Wait for a key stroke; the same function arranges events processing
/*    cvWaitKey(0);*/
    cvReleaseImage(&image);
    cvReleaseImage(&gray);
    cvReleaseImage(&edge);
/*    cvDestroyWindow(wndname);*/
/*    cvDestroyWindow(wndname1);*/
}

// define a trackbar callback
void on_trackbar(int h)
{
printf( "on_trackbar\n");
	//各种方法的图像平滑
	     cvSmooth( gray, edge, CV_BLUR, 3, 3, 0, 0 );
	//会将src中的每一个元素的每一位取反，然后把结果赋给dst,gray是src
	//因为gray是灰度图片，所以显示是黑色的地方是图像。白色是背景
	//这样edge中白色部分是图像，黑色是背景（值为0）
	    cvNot( gray, edge );
//
/*	    cvShowImage(wndname1, edge);*/

	    // Run the edge detector on grayscale
/*
image 输入图像，这个必须是单通道的，即灰度图
edges 输出的边缘图像 ，也是单通道的，但是是黑白的
threshold1 第一个阈值
threshold2 第二个阈值
aperture_size Sobel 算子内核大小
*/
		//edge保存了边缘的点，就是将非边缘点置为黑色，现在edge中的白色部分就是边缘点
		//===edge已经完成了取边缘点
	    cvCanny(gray, edge, (float)h, (float)h*3, 3);
//
/*	    cvShowImage(wndname2, edge);*/

	 //将cedge清0
	    cvZero( cedge );
	    // copy edge points
/*
src
输入数组。
dst
输出数组。
mask
操作掩码是8比特单通道的数组，它指定了输出数组中被改变的元素。
函数cvCopy从输入数组中复制选定的成分到输出数组：
如果mask(I)!=0,则dst(I)=src(I)。
如果输入输出数组中的一个是IplImage类型的话，其ROI和COI将被使用。输入输出数组必须是同样的类型、维数和大小。函数也可以用来复制散列数组（这种情况下不支持mask）。
*/
	//就是将edge中不为0的点（白色部分，即是边缘点），对应的位置将image赋值给cedge
	    cvCopy( image, cedge, edge );

/*	    cvShowImage(wndname, cedge);*/

////////////////////cvFindContours使用//////////////////////////////
  //创建窗口

	int i=0;  

     int contours_num=0;//图像中提取轮廓的数目  
    storage=cvCreateMemStorage(0);//设置轮廓时需要的存储容器  
    CvSeq *contours =0;//设置存储提取的序列指针  

/*    cvCvtColor(cedge, gray, CV_BGR2GRAY);*/
  
/*    cvThreshold(gray,gray,128,255,CV_THRESH_BINARY);//二值化  ,因为img读取的时候就是灰度图片*/
    IplImage *pContourImg = cvCreateImage(cvGetSize(image),  IPL_DEPTH_8U,3); 
	    cvZero( pContourImg );
    IplImage *pContourImg1 = cvCreateImage(cvGetSize(image),  IPL_DEPTH_8U,3); 
	    cvZero( pContourImg );
	    cvZero( pContourImg1 );
/*    cvCvtColor(cedge, pContourImg, CV_GRAY2BGR);*/
/*  */
/*    cvNamedWindow("windows",1);  */
/*    cvShowImage("windows",img);//显示二值图  */
    /*cvFindContours查找物体轮廓*/  
/*    mode=CV_RETR_LIST;*/

/*
检索模式，可取值如下：
CV_RETR_EXTERNAL：只检索最外面的轮廓；
CV_RETR_LIST：检索所有的轮廓，并将其放入list中；
CV_RETR_CCOMP：检索所有的轮廓，并将他们组织为两层：顶层是各部分的外部边界，第二层是空洞的边界；
CV_RETR_TREE：检索所有的轮廓，并重构嵌套轮廓的整个层次。
边缘近似方法（除了CV_RETR_RUNS使用内置的近似，其他模式均使用此设定的近似算法）。可取值如下：
CV_CHAIN_CODE：以Freeman链码的方式输出轮廓，所有其他方法输出多边形（顶点的序列）。
CV_CHAIN_APPROX_NONE：将所有的连码点，转换成点。
CV_CHAIN_APPROX_SIMPLE：压缩水平的、垂直的和斜的部分，也就是，函数只保留他们的终点部分。
CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS：使用the flavors of Teh-Chin chain近似算法
的一种。
CV_LINK_RUNS：通过连接水平段的1，使用完全不同的边缘提取算法。使用CV_RETR_LIST检索模式能使用此方法。
*/
        mode=CV_RETR_TREE;//提取轮廓的模式  CV_RETR_TREE
        contours_num=cvFindContours(edge,storage,&contours,sizeof 		(CvContour),mode,CV_CHAIN_APPROX_NONE,cvPoint(0,0));  
 
        //可能是拟合，有这一句找出的轮廓线更直。---数据更少了
/*         contours = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 );   */
  
	printf( "contours_num=%d\n",contours_num);
	if(contours ==NULL)
	{
		printf( "cvFindContours fails\n");
		return;
	}

/*       //显示图像*/
/*        cvDrawContours(pContourImg, contours, */
/*		 CV_RGB(0,0,255), CV_RGB(255, 0, 0), */
/*		 1, 1, 8, cvPoint(0,0));*/
/*       cvShowImage(wndname, pContourImg );*/

         CvTreeNodeIterator iterator;   
         cvInitTreeNodeIterator(&iterator,contours,3);   
         //把所有轮廓的点收集起来   
         CvSeq* allpointsSeq = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour),   
                                    sizeof(CvPoint), storage);   
	int coloutCount=0;
         while( 0 != (contours = (CvSeq*)cvNextTreeNode(&iterator)) )
	{    
             //找到一个轮廓就可以用for循环提取里面的点了   
             //这里遍历CvSeq里面的元素的方法很怪异   
             onetourlength = contours->total;   
             //给点数组分配空间，记得释放   
             CvPoint *points = (CvPoint *)malloc(sizeof(CvPoint) * onetourlength);    
             //printf("seqlength:%dn",seqlength);   
             CvSeqReader reader;   
             CvPoint pt = cvPoint(0,0);   
             CvPoint ptZero = cvPoint(-1,-1);
             cvStartReadSeq(contours,&reader,0);   
             //开始提取   
		int i;
/*		printf("new  line:\n");*/
             for( i = 0 ;i < onetourlength; i++)
	     {   
                 CV_READ_SEQ_ELEM(pt,reader);   
                 points[i] = pt;   
  
                 cvSeqPush(allpointsSeq,&pt);   
		//把数据复制到共享内存里面去
		if(i==0)
		{
			
			memcpy( (&((*(pMapWithCandy)).data)+sizeof(CvPoint)*coloutCount),&ptZero,sizeof(CvPoint));	
			coloutCount++;
		}
		printf("	pt.x=%d,pt.y=%d--coloutCount=%d\n",pt.x,pt.y,coloutCount);
		memcpy( (&((*(pMapWithCandy)).data)+sizeof(CvPoint)*coloutCount),&pt,sizeof(CvPoint));
		coloutCount+=1;  
             }     
		printf("\n");

             //把这个轮廓点找出后，就可以用这些点画个封闭线
/*	      cvPolyLine(pContourImg1,&points,&onetourlength,1,0,CV_RGB(0,255,0),1,8,0); */
             if(points)
             {
             	free(points);
             }
	   }
	      (*(pMapWithCandy)).nSize=coloutCount;//point的个数
		printf("total :coloutCount =%d:\n",coloutCount);
/*              cvShowImage(wndname1, pContourImg1 );*/
    
	
printf( "end on_trackbar\n");
}
 
int main( int argc, char** argv )
{
	image=NULL;
	gray=NULL;
	edge=NULL;

	//与主程序内存共享代码
	char* filename=getLineInFile("../Data/config",1);
/*	candyTest(filename,0,pMapWithCandy);*/
/*	return 0;*/


	keyWithCandy = ftok(nameWithCandy,0);
	if(keyWithCandy == -1)
		perror("ftok error");
	shmIdWithCandy = shmget(keyWithCandy,tryShm,IPC_CREAT | 0666);	
	if(shmIdWithCandy == -1)
	{
		perror("shmget error");
		return;
	}
	pMapWithCandy = (shareData*)shmat(shmIdWithCandy,NULL,0);


/*	(*(p_map+i)).age=0;*/
/*	printf( "name:%s\n",(*(pMapWithCandy+i)).name );*/
/*	printf( "age %d\n",(*(pMapWithCandy+i)).age );*/

	int xx=0;
	static xxx=0;
///接受消息
printf( "msg ssss、\n");
	while(xx == 0)
	{
		printf( "\n");

		//从主程序收取消息，如果共享内存区域的flag为0说明可以进行下一步操作
		int received;
		/* Generate the ipc key */
		ipckeyForCandy = ftok(SendMsgWithCandy, 43);
		printf("My key is %d\n", ipckeyForCandy);

		/* Set up the message queue */
		mqIdForCandy = msgget(ipckeyForCandy,0);// IPC_CREAT
		printf("Message identifier is %d\n", mqIdForCandy);

		received = msgrcv(mqIdForCandy, &msgForCandy, sizeof(msgForCandy), 0,0);//IPC_NOWAIT

		printf("gei filename  = %s \n", msgForCandy.text);	



		if(received != -1)
		{
/*			printf( "name=%s\n",(*(pMapWithCandy)).data );*/
/*			printf( "flag=%d\n",(*(pMapWithCandy)).flag );*/
			candyTest(msgForCandy.text,0,pMapWithCandy);

			//更新flag：
			(*(pMapWithCandy)).flag=1;	
			printf( "set flag=%d\n",(*(pMapWithCandy)).flag);
		}	

	}

printf( "end 、\n");


/*	if(shmdt(p_map) == -1)*/
/*		perror(" detach error ");*/
    return 0;
}
 
/*#ifdef _EiC*/
/*main(1,"edge.c");*/
/*#endif*/
