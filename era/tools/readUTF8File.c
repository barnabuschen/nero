





#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */
#include <stdio.h>

#include "readUTF8File.h"



/*2013年11月15日 星期五 20时22分16秒 */


static int nero_error_Id=0;

struct charshuzhu6
{
nero_us8int tmp[6];

};

 struct adkfjao
{
ChUTF8 tmp;
nero_s32int end;

};


ChUTF8 chChar[ChineseCharNum];
nero_s32int charCounts;



nero_s32int readUTF8FileData(nero_8int * FileName)//
{
/*	struct adkfjao  testsadkfjao;*/
/*	struct charshuzhu6 trunshuzi;*/
/*	struct charshuzhu6 *trunshuziPoint;*/
/*	DigitalUnicode16 unicodeInDigtial;*/
/*	DigitalUnicode16 *unicodePont;*/
	nero_s32int fd;
	nero_s8int *mapped_mem, * p;
	nero_s32int flength = 1024;
	nero_us8int tmp;
	nero_s32int i;
	nero_s32int charLength=0;//该字节所占位数
	//存储utf编码，chChar中是以utf8编码的汉字，chCharUnicode16Code中则是对应的UnicodeCode
	charCounts=0;
	
/*	nero_us32int chCharUnicodeCode[ChineseCharNum];*/
/*	Unicode16 chCharUnicode16Code[ChineseCharNum];*/
	
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
	#ifdef Nero_DeBuging10
	printf("flength=%d.\n",flength);
	printf("FileName=%s.\n",FileName);
	printf("mapped_mem=%x.\n",mapped_mem);
	printf("start  \n");
	#endif
	p=mapped_mem;
	while(1)
	{
	
		tmp=*p;
	
		//这里需要判断tmp的最高位是0，还是110还是1110
		/*
		
		一个数与   1000 0000做与运算,等于0说明最高位为0
		一个数与   0010 0000做与运算,等于0说明第3位为0
		
		*/
		#ifdef Nero_DeBuging1
		if(charCounts >55500)
			break;
		#endif
		if( *p ==13  && *(p+1) ==10)
		{
/*			NerReportMsgError(nero_error_Id);*/
			break;
		}
		nero_error_Id++;
		if( (tmp & 0x80 ) ==0)
		{
/*			printf("xx.\n");*/
			charLength=1;
		}
		else 
		{	if((tmp & 0x20 ) ==0)
			{
				charLength=2;
			
			}
			else
			{
				charLength=3;
			}
		}
		//
		nero_error_Id++;
		#ifdef Nero_DeBuging20
		printf("charLength=%d.\n",charLength);
		printf("tmp=%d.\n",tmp);
		#endif		
		
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
		nero_error_Id++;
		if(*p != 0x3a)
		{
			NeroErrorMsg;
			return NeroError;
		
		}
		p++;		
		//现在开始 将Unicode编码存于unicodeInDigtial中
/*		unicodePont=(DigitalUnicode16 *)p;*/
/*		unicodeInDigtial=*unicodePont;	*/

/*		trunshuziPoint=(struct charshuzhu6 *)&unicodeInDigtial;		*/
/*		trunshuzi=*trunshuziPoint;*/
/*		trunshuzi.tmp[5]=0;*/
/*		nero_s32int res=atoi(trunshuzi.tmp);				*/
		
		//打印utf8编码数据：
		#ifdef Nero_DeBuging10
		if(charCounts >5000)
		{
			testsadkfjao.tmp=chChar[charCounts];
			testsadkfjao.end=0;
			printf("%s\n",(nero_s8int *)&testsadkfjao);
		}
		#endif

/* */
/*		printf("%d:%d \n",charCounts,res);*/

		p+=6;
		charCounts++;
	}
	
	
	
	
	close(fd);
	munmap(mapped_mem, flength);

	return NeroOK;

}










