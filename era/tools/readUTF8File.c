





#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */
#include <stdio.h>

#include "readUTF8File.h"


#define ChineseCharNum  8000
/**/


static int nero_error_Id=0;

// b readUTF8FileData
//b 55
nero_s32int readUTF8FileData(nero_us8int * FileName)//
{
	DigitalUnicode16 unicodeInDigtial;
	DigitalUnicode16 *unicodePont;
	nero_s32int fd;
	nero_s8int *mapped_mem, * p;
	nero_s32int flength = 1024;
	nero_us8int tmp;
	nero_s32int i,j,k;
	nero_s32int charLength;//该字节所占位数
	//存储utf编码，chChar中是以utf8编码的汉字，chCharUnicode16Code中则是对应的UnicodeCode
	nero_s32int charCounts=0;
	ChUTF8 chChar[ChineseCharNum];
/*	nero_us32int chCharUnicodeCode[ChineseCharNum];*/
	Unicode16 chCharUnicode16Code[ChineseCharNum];
	
	void * start_addr = 0;
	fd = open(FileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	flength = lseek(fd, 1, SEEK_END);
//	write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
	lseek(fd, 0, SEEK_SET);
/*	write(fd, "7", 1);*/
	mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
	MAP_PRIVATE,       //不允许其它进程访问此内存区域
	fd, 0);
	
	
	
	/* 使用映射区域. 现在开始解析UTF8File*/	
	p=mapped_mem;
	while(1)
	{
	
		tmp=*p;
	
		//这里需要判断tmp的最高位是0，还是110还是1110
		/*
		
		一个数与   1000 0000做与运算,等于0说明最高位为0
		一个数与   0010 0000做与运算,等于0说明第3位为0
		
		*/
		#ifdef Nero_DeBuging
		nero_error_Id++;
		if(charCounts >200)
			break;
		#endif
		if( *p ==13  && *(p+1) ==10)
		{
			NerOkMsg;
			break;
		}
		
		if( (tmp & 0xa000 ) ==0)
			charLength=1;
		else 
		{	if((tmp & 0x0a00 ) ==0)
			{
				charLength=2;
			
			}
			else
			{
				charLength=3;
			}
		}
		//
		
		if(charLength !=3)
		{
			NerReportMsgError(nero_error_Id);
			return NeroError;
		
		}
		for(i=1;i<=charLength;i++)
		{
		
			switch(i)
			{
				case 1: chChar[charCounts].first=*p;p++;break;//低位
				case 2: chChar[charCounts].second=*p;p++;break;
				case 3: chChar[charCounts].third=*p;p++;break;
				case 4: chChar[charCounts].fourth=*p;p++;break;
				default:NerReportMsgError(nero_error_Id);return NeroError;
			}
		}
		//下一个字符应该是冒号
		if(*p != 0x3a)
		{
			NeroErrorMsg;
			return NeroError;
		
		}
		p++;		
/*		unicodeInDigtial.first=*p;p++;*/
/*		unicodeInDigtial.second=*p;p++;*/
/*		unicodeInDigtial.third=*p;p++;*/
/*		unicodeInDigtial.fourth=*p;p++;*/
/*		unicodeInDigtial.fifth=*p;p++;*/
/*		*/

/*		等价于*/
		unicodePont=(DigitalUnicode16 *)p;
		unicodeInDigtial=*unicodePont;
		//检测unicodeInDigtial合法性
		
		
		//打印数据：
		printf("%d::%d %d %d %d %d",charCounts,
			unicodeInDigtial.first,unicodeInDigtial.second,unicodeInDigtial.third,
			unicodeInDigtial.fourth,unicodeInDigtial.fifth);
		
		p+=6;
		charCounts++;
	}
	
	
	
	
	close(fd);
	munmap(mapped_mem, flength);

	return NeroOK;

}


nero_s32int readUTF8FileData_old(char * FileName)//
{
	int fd;
	char *mapped_mem, * p;
	int flength = 1024;
	void * start_addr = 0;
	fd = open(FileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	flength = lseek(fd, 1, SEEK_END);
//	write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
	lseek(fd, 0, SEEK_SET);
/*	write(fd, "7", 1);*/
	mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
	MAP_PRIVATE,       //不允许其它进程访问此内存区域
	fd, 0);
	/* 使用映射区域. */
	printf("%s\n", mapped_mem); /* 为了保证这里工作正常，参数传递的文件名最好是一个文本文件 */
	close(fd);
	munmap(mapped_mem, flength);

	return NeroOK;

}















