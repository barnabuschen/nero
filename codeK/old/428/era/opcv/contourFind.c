#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
 
int main( int argc, char** argv )
{
  //声明IplImage指针
  IplImage* pImg = NULL; 
  IplImage* pContourImg = NULL;
 
 /*
 cvCreateMemStorage()用来创建一个内存存储器，来统一管理各种动态对象的内存。
函数返回一个新创建的内存存储器指针。
参数block_size对应内存器中每个内存块的大小，为0时内存块默认大小为64k。
 
 */
  CvMemStorage * storage = cvCreateMemStorage(0);
/*

对于CvSeq这一结构体，又称为可动态增长元素序列,
*/
  CvSeq * contour = 0;
  int mode = CV_RETR_EXTERNAL;
 
  if( argc == 3)
      if(strcmp(argv[2], "all") == 0)
	mode = CV_RETR_CCOMP; //内外轮廓都检测
 
 
  //创建俩个窗口
  cvNamedWindow("src", 1);
  cvNamedWindow("contour",1);
 
 
  //载入图像，强制转化为Gray
  if( argc >= 2 && (pImg = cvLoadImage( argv[1], 0)) != 0 )
    {
 	//显示初始图片----这里显示的好像是灰度图片
      cvShowImage( "src", pImg );
 
      //为轮廓显示图像申请空间

      //3通道图像，以便用彩色显示
      pContourImg = cvCreateImage(cvGetSize(pImg),
					  IPL_DEPTH_8U,
					  3);

      //copy source image and convert it to BGR image----灰度转化为BGR
      cvCvtColor(pImg, pContourImg, CV_GRAY2BGR);
 
 
      //查找contour---得到轮廓点集合---zai在灰度图中找
      cvFindContours( pImg, storage, &contour, sizeof(CvContour), 
		  mode, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 
    }
  else
    {
      //销毁窗口
      cvDestroyWindow( "src" );
      cvDestroyWindow( "contour" );
      cvReleaseMemStorage(&storage);
 
      return -1;
    }
  //将轮廓画出    ---在彩色图中话轮廓（其实图片本身就是灰度的，只是轮廓是彩色的）
  cvDrawContours(pContourImg, contour, 
		 CV_RGB(0,0,255), CV_RGB(255, 0, 0), 
		 2, 2, 8, cvPoint(0,0));
  //显示图像
  cvShowImage( "contour", pContourImg );
 
  cvWaitKey(0);
 
 
  //销毁窗口
  cvDestroyWindow( "src" );
  cvDestroyWindow( "contour" );
  //释放图像
  cvReleaseImage( &pImg ); 
  cvReleaseImage( &pContourImg ); 
 
  cvReleaseMemStorage(&storage);
 
  return 0;
}
