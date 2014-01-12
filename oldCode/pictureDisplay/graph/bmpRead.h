


#include <stdint.h>
/*
（1）文件头信息块
0000-0001：文件标识，为字母ASCII码“BM”，42 4D。亦或者与19778相比较。
0002-0005：整个文件大小，单位字节。
0006-0009：保留，每字节以“00”填写。
000A-000D：记录图像数据区的起始位置。从文件开始到位图数据(bitmap data)之间的偏移量。 
（2）图像描述信息块

000E-0011：图像描述信息块的大小，常为28H。
0012-0015：图像宽度。以像素为单位。
0016-0019：图像高度。以像素为单位。
001A-001B：图像的plane总数（恒为1）。
001C-001D：记录像素的位数，很重要的数值，图像的颜色数由该值决定。1 - Monochrome bitmap，4 - 16 color bitmap，8 - 256 color bitmap，F - 16位位图，18 - 24bit (true color) bitmap，20 - 32位位图。

001E-0021：数据压缩方式（数值位0：不压缩；1：8位压缩；2：4位压缩；3：Bitfields压缩）。
0022-0025：图像区数据的大小。单位字节，该数必须是4的倍数。
0026-0029：水平每米有多少像素，在设备无关位图（.DIB）中，每字节以00H填写。
002A-002D：垂直每米有多少像素，在设备无关位图（.DIB）中，每字节以00H填写。
002E-0031：此图像所用的颜色数。

0032-0035：指定重要的颜色数。当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要。
     如上，整个位图的信息头共54字节，每个位置有特定含义
*/
/*

图像数据区，在此部分记录着每点像素对应的颜色索引号，其记录方式也随颜色模式而定，
既2色图像每点占1位（8位为1字节）；
16色图像每点占4位（半字节）；
256色图像每点占8位（1字节）；
真彩色图像每点占24位（3字节）。
所以，整个数据区的大小也会随之变化。究其规律而言，可的出如下计算公式：
图像数据信息大小=（图像宽度*图像高度*记录像素的位数）/8。
				扫描行是由底向上存储的，这就是说，阵列中的第一个字节表示位图左下角的像素，
而最后一个字节表示位图右上角的像素。
*/
//位图头文件结构：共14字节
typedef struct tagBITMAPFILEHEADER{
     short bfType;
     int bfSize;//整个文件大小，单位字节。
     short bfReserved1;
     short bfReserved2;
     int bfOffBits;// 位图数据的起始位置，以相对于位图
}bmpHead ,*PbmpHead;


//位图信息结构：共36-14=22字节
typedef struct tagBITMAPINFOHEADER{
      int biSize;
      int biWidth;//x像素
      int biHeight;
      short biPlanes;
      short biBitCount;//每个像素所需的位数，
      int biCompression;
      int biSizeImage;//图像区数据的大小。单位字节，该数必须是4的倍数
      int biXPelsPerMeter;
      int biYPelsPerMeter;
      int biClrUsed;
      int biClrImportant;
}bmpMsg,*PbmpMsg;


//get bmp msg
void getBmpMsg(char * FileName,PbmpHead head,PbmpMsg msg);
void printBmpMsg(PbmpHead head,PbmpMsg msg);
unsigned char * readBmpData(char * FileName,unsigned int * dataCount,bmpHead * head,bmpMsg * msg);//读取bmp文件图形数据，返回的数据大，请及时释放
//将bmp数据反转，即使把下面的数据放到上面
void putBMPInZheng(unsigned char * newbmpdata,int w,int h);
//将bmp数据从rgb扩展为rgba
unsigned char * putBMPRGBtoRGBA(unsigned char * newbmpdata,int w,int h);//传入像素的大小

unsigned char * addLieToBMP(unsigned char * newbmpdata,int w,int h,int lie);//传入w是一行的字节数

//传入rgb，返回rgba的数据指针，同时，newW是新数据的行像素个数，同时会转正
unsigned char * ChangeRGBToRGBA(unsigned char * bmpdata,int w,int h,int * newW,int ifPutInZheng);//传入w是一行的字节数

int getstrideOfBMP(int w);








