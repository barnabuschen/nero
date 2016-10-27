
#ifndef MYTYPE_H
#define MYTYPE_H

// #include <glib.h>
#include "list.h"
#include "NeroError.h"
#include "NeroMsgId.h"

/*一些常量*/
#define  ChoseBaseObj   1
#define  ChoseDerivativeObj   2	

#define  ChoseOutputAddress  1
#define  ChoseOutputType   2	
#define  ChoseOutputData  3


	
#define NeroForgetCycle    10  /*10秒 超过这个时间不被更新，下次被识别时就不更新神经纤维的链接强度
				对于一个无符号int来说，如果1表示1秒，那么一个int可以表示136年

				*/
#define NeroActivateMaxTimes   200
#define NeroMaxLastTimeINForecastList    20  /*对象在预测列表中最长等待时间，超时则被去除列表*/
#define DataFlowProcessObjNum_MAX (7)
#define FILEPATH_MAX (180)
#define StringLen_MAX (500)			
/*8192kb  =1032KB*/
#define ChineseCharNum  8000
/*2的32次=	     4294967295   42亿*/
//#define MaxNeroNum  1000000/*百万*/
//#define MaxNeroNum  89000000/*千万  ,意味着总共需要  89000 000   *  32个字节(ActivationNeuron)= 2.7G*/
#define MaxNeroNum 100000000/*1亿*/


#define StagingAreaNeroNum 33554432/*用于StagingAreaNeroPool，一个G大约能包含33554432个ActivationNeuron对象*/
/*		     123456789	*/



/*俩个神经元相互联系的关系，父亲概念的定义指的是上层概念，孩子是下层概念*/
#define Relationship_FatherToChild  1  /*单向联系：父亲概念连向孩子*/
#define Relationship_ChildToFather 2 /*单向联系：孩子概念连向父亲*/
#define Relationship_bothTother  3/*双向联系：孩子概念连向父亲，同时，父亲概念连向孩子*/


#define Process_TemporaryNUM   7500    //just used  in  fuc  Process_StrengthenLink
/*传导纤维种类*/
#define NerveFiber_Input 1 
#define NerveFiber_Output  2


/*一些提示信息*/
#define NeroOK   1
#define NeroError   -1
#define NeroErrorMsg   printf("something wrong!\n")
#define NerOkMsg   	printf("exit right way!\n")
#define NerReportMsgError(id)   	printf("readUTF8FileData:something wrong with ID:%d!\n",id)

//~ #define Conf_Modify_Info    1
#ifdef   Conf_Modify_Info
				#define printConfChangMsg(a)    printf("conf has changed:%d  \n",a)
#else
				#define printConfChangMsg(a)    printf("  \n")
#endif

/*变量类型重定义*/
typedef int  nero_s32int;
//typedef int  nero_s32int;
typedef unsigned int  nero_us32int;
typedef unsigned char  nero_us8int;
typedef signed char  nero_s8int;
typedef  char  nero_8int;
// typedef  int  gint;
// typedef  char  gchar;
/*调试开关*/
//#define  Nero_ProcessERROR_Msg
#define  Nero_DeBuging1_cacel
#define Nero_DeBuging0
#define Nero_DeBuging1 
#define Nero_DeBuging2 
#define Nero_TestCount     30
//#define DataFlowProcess_error_Msg
//#define Nero_DeBuging17/11/13
#define Nero_DeBuging18_11_13 
#define Nero_DeBuging22_11_13 
#define Nero_DeBuging03_12_13 
#define Nero_DeBuging20_12_13 
#define Nero_DeBuging21_12_13 
#define Nero_DeBuging04_01_14
#define Nero_DeBuging08_01_14
#define Nero_DeBuging09_01_14
#define Nero_DeBuging10_01_14

#define Nero_DeBuging14_01_14
#define Nero_DeBuging24_01_14
#define Nero_DeBuging25_01_14
#define Nero_DeBuging27_01_14
#define Nero_DeBuging06_02_14
#define Nero_DeBuging04_25_16
#define Nero_DeBuging10_16_16
#define Nero_DeBuging10_26_16


#define NowIdDeBug 
#define Nero_DeBugInOperating_Pic
#define NoIncludeG_return_if_fail  


typedef struct ChineseUtf8Code
{
nero_us8int first;
nero_us8int second;
nero_us8int third;
nero_us8int fourth;//下面是高位

}ChUTF8;


typedef struct ChineseUtf8Code_
{
nero_us8int first;
nero_us8int second;
nero_us8int third;
}ChUTF8_;


typedef struct ChineseUnicode16Code   //16位的Unicode编码
{
//	union{
	nero_us8int first;
	nero_us8int second;
//	}
}Unicode16;

typedef struct ChineseUnicode16DigitalCode//五位拼起来就是Unicode16码
{
nero_us8int first;
nero_us8int second;
nero_us8int third;
nero_us8int fourth;//下面是高位
nero_us8int fifth;

}DigitalUnicode16;


typedef struct POSITIOM
{
int x;
int y;

}POS;

//这个结构用来生成一个以坐标表示的像素的集合，显然在函数getLineFromBMP
//中，生成similarityAroundOnePoint矩阵中的链表和，根据S点，可以将这个
//链表最终生成BMP结构的实际位置的链表
typedef struct ListPos
{
struct list_head p;
POS pos;
}PosList;


typedef struct COLOUR
{
unsigned char r;
unsigned char g;
unsigned char b;
}Color;


typedef struct COLOURCount
{
struct list_head p;
unsigned char r;
unsigned char g;
unsigned char b;
int count;
}ColorCnt;

typedef struct  lineManagement
{
struct list_head p;//用来维持所有线条表头的结构--就是维持LineMan本身的链表
ColorCnt * cntHead;//指向用COLOURCount表示的线条链表的表头的指针
PosList * posHead;//指向用PosList表示的线条链表的表头的指针
int count;//如果是表头：记录多少条线
		//如果是节点就记录该条线上有多少个点
}LineMan;




#define printColorCNT(Col)  (printf("	col is %x %x %x ,count is %d\n",(Col->r),(Col->g),(Col->b),(Col->count)))

#define printColor(Col)  (printf("col is %x %x %x \n",(Col->r),(Col->g),(Col->b)))

#define equalCOlVsColorCnt(col,cnt)  ((col->r == cnt->r)  && (col->g == cnt->g) &&(col->b == cnt->b))


#define InitialPOS(c)  ((c.x)=(c.y)=0)
#define InitialCOLOUR(c)  (c.r=c.g=c.b=0)

//#define InitialCOLOURWithCNT(col,cnt)  ((col->r = cnt->r)  ; (col->g == cnt->g) ;(col->b = cnt->b);)

//#define InitialColorCnt(c)  (c.r=0;c.g=0;c.b=0;c.count;c.prev=NULL;c.p.next=NULL;)

#define putNumTwoINOne(high,low,type) ( (((type)high) << (sizeof(high)*8)) +low )

#define putNumFourINOne(high2,high1,low2,low1,type) ( (((type)(unsigned char)high2) << (sizeof(high1)*24)) + \
							(((type)(unsigned char)high1) << (sizeof(high1)*16))+\
							(((type)(unsigned char)low2) << (sizeof(high1)*8))+\
										(unsigned char)low1 )	\

//#define BYTEOS32 11
//#ifdef BYTEOS32
//#endif


//#ifdef BYTEOS64
//#endif
//struct { long type; char text[100]; } mymsg;
//nero_8int  file_path_getcwd[FILEPATH_MAX];/*保存当前目录*/
nero_8int * Operating_ipckey; 

nero_8int * IO_ipckey ;
nero_8int * Log_ipckey ;
nero_8int * Sys_ipckey ;

nero_s32int Operating_mq_id;
nero_s32int IO_mq_id;
nero_s32int Log_mq_id;
nero_s32int Sys_mq_id;















#endif
