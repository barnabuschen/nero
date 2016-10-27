



#include <string.h>
#include <malloc.h>
#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */
#include <stdio.h>
#include "../NeuralNetwork/NeuralNetwork.h"
#include "readUTF8File.h"

#include "../common/fileOperating.h"

#include "../NeuralNetwork/NeuralOperating.h"

/*2013年11月15日 星期五 20时22分16秒 */


static int nero_error_Id=0;

struct charshuzhu6
{
nero_us8int tmp[6];

};



ChUTF8 chChar[ChineseCharNum];
nero_s32int charCounts;
static nero_8int  file_path_getcwd[FILEPATH_MAX]="/tmp";/*保存当前目录*/



// 解析文件iris.data，将文件中得数据分为3个数组进行保存

nero_s32int readIrisFileForData(nero_8int * FileName )
{

// /home/jty/nero/nero/src/data/iris/iris.data

    ChUTF8      * chCharInTask;
    nero_s32int  charCountsInTask;
    nero_s32int *dataKind;
    nero_us8int *linc;
    nero_s32int dataNum,k,countOfWord,m,lenOfpar;
    nero_s32int  numberTmp;
    void **DataFlow;
    nero_8int baseobjName[100]="阿拉伯数字";
    const nero_us32int  LenOfLinStrRecored=2000;
    nero_us8int linStrRecored[LenOfLinStrRecored];
    nero_us8int newlinStrRecored[LenOfLinStrRecored];
     nero_us8int lastlinStrRecored[LenOfLinStrRecored];
   
    struct DataFlowProcessArg arg2;     
    struct { long type; char text[100]; } mymsg;  
     nero_8int  fileName[FILEPATH_MAX];
      nero_8int  newDataFileForIris[FILEPATH_MAX];
    struct adkfjao  testsadkfjao;
    nero_s32int fd;
    nero_s8int *mapped_mem, * p,* end,* LineEnd,* LineStart;
    nero_s32int flength = 1024;
    nero_us8int tmp;
    nero_s32int i,objNUm,x,y,z;
    nero_s32int charLength=0;//该字节所占位数
    //存储utf编码，chChar中是以utf8编码的汉字，chCharUnicode16Code中则是对应的UnicodeCode
    charCounts=0;
    



    getcwd(file_path_getcwd,FILEPATH_MAX);
    // sprintf(fileName,"%s%s",file_path_getcwd,FileName);
    sprintf(fileName,"%s",FileName);

    // sprintf(newDataFileForIris,"%s%s",file_path_getcwd,FileName);
    sprintf(newDataFileForIris,"./data/iris/newDataFileForIris.sh",file_path_getcwd,FileName);
	createFile(newDataFileForIris);

    void * start_addr = 0;
    fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    flength = lseek(fd, 1, SEEK_END);
//  write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
    lseek(fd, 0, SEEK_SET);
/*  write(fd, "7", 1);*/
    mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
    MAP_PRIVATE,       //不允许其它进程访问此内存区域
    fd, 0);
    
    

    /* 使用映射区域. 现在开始解析UTF8File*/ 
    #ifdef Nero_DeBuging10_01_14
    printf("flength=%d.\n",flength);
    printf("FileName=%s.\n",fileName);
    printf("mapped_mem=%x.\n",mapped_mem);
    printf("start  \n");
    #endif
    p=mapped_mem;

    end=mapped_mem+flength-1;
    charCountsInTask=0;

     chCharInTask=chChar;
     objNUm=0;
    charCountsInTask=0;

    LineStart=p;
    LineEnd=p;
    countOfWord=0;
    // return  0;
    while( 1)
    {
        if( p  >= end)
        {
          // NerReportMsgError(nero_error_Id);
            break;
        }

        // printf("start  p=%x,end=%x\n",p,end);
        tmp=*p;
        charLength=0;
        //这里需要判断tmp的最高位是0，还是110还是1110
        
        // 一个数与   1000 0000做与运算,等于0说明最高位为0
        // 一个数与   0010 0000做与运算,等于0说明第3位为0
        

        // nero_error_Id++;
        // the  highest  len of uft8  code  for chinese is 3
        if( (tmp & 0x80 ) ==0)
        {
            charLength=1;
        }
        else 
        {   if((tmp & 0x20 ) ==0)
            {
                charLength=2;
            
            }
            else
            {
                charLength=3;
            }
        }
        if( *p == 10)
        {
            charLength=0;
        }

        //
        // nero_error_Id++;
        #ifdef Nero_DeBuging2_
        printf("charLength=%d.\n",charLength);
        printf("tmp=%d.\n",tmp);
        #endif      
        
        // if(charLength >3  ||  charLength< 1 )
        // {
        //     // printf("p=%x.\n",p);
        //     // NerReportMsgError(nero_error_Id);
        //     return NeroError;
        
        // }
        chCharInTask[objNUm].first=0;//低位
        chCharInTask[objNUm].second=0;
        chCharInTask[objNUm].third=0;
        for(i=1;i<=charLength;i++)
        {
           
            switch(i)
            {
                case 1: chCharInTask[objNUm].first=*p;p++;charCountsInTask++;break;//低位
                case 2: chCharInTask[objNUm].second=*p;p++;charCountsInTask++;break;
                case 3: chCharInTask[objNUm].third=*p;p++;charCountsInTask++;break;
                case 4: chCharInTask[objNUm].fourth=*p;p++;charCountsInTask++;break;
                default:
                        // printf("p=%x.\n",p);
                        // NerReportMsgError(nero_error_Id);
                        return NeroError;
            }
            
        }

        // if(*(p) ==10)
        //     p++;
        if(charLength >0 /* &&  *(p) ==10 */)
        {

            
             #ifdef Nero_DeBuging2_
            // charCountsInTask++; 
            x=chCharInTask[objNUm].first;
            y=chCharInTask[objNUm].second;
            z=chCharInTask[objNUm].third;
            if (x !=0 && y !=0 &&  z !=0)
            {
                printf("打印字符对象1《%c%c%c》charLength=%d\n",x,y,z,charLength); 
            }
            else if(x !=0   )
            {
                printf("打印字符对象2《%c》charLength=%d\n",x,charLength); 
            }
            else if(y !=0   )
            {
                printf("打印字符对象3《%c》charLength=%d\n",y,charLength); 
            }
            else if(z !=0   )
            {
                printf("打印字符对象4《%c》charLength=%d\n",z,charLength); 
            }

            #endif  

             objNUm=objNUm+1;//num  of char   In  one line
        }


        if(*p != 0x0a)
        {
            // printf("*p != 0x0a\n"); 
            continue;
        }
        else
        {
        	countOfWord++;
			LineEnd=p-1;
			k=LineEnd-LineStart+1;
			nero_us8int linStrRecored[LenOfLinStrRecored];
			if( k>=   LenOfLinStrRecored  ||  k <  5)
			{
				printf("\n");   
				NeroErrorMsg;
				return NeroError;
			}

			memset(linStrRecored,0,LenOfLinStrRecored);
			memset(newlinStrRecored,0,LenOfLinStrRecored);
			memset(lastlinStrRecored,0,LenOfLinStrRecored);
            memcpy(linStrRecored,LineStart,k);

            // printf("%s\n",linStrRecored); 
            // 先去掉点号
            // x--
            // y 
            for(y=0,LineStart=linStrRecored,LineEnd=linStrRecored;y< LenOfLinStrRecored;y++)
            {

            	if( linStrRecored[y] ==  '.')
            	{
            		x=y;
            		while(x < (LenOfLinStrRecored -1))
            		{
            			linStrRecored[x]=linStrRecored[x+1];
            			x++;           			
            		}
            	}


            }

		    LineStart=strtok(linStrRecored,",");
		    k=0;
		    numberTmp=0;
		    printf("\nline=%d:",countOfWord);  
		    // memset(linStrRecored,0,LenOfLinStrRecored);
 
		    while( LineStart != NULL )
		    {
		        /* While there are tokens in "string" */
		        // printf( "%s ", LineStart );
		        /* Get next token: */
		    	switch(k)
		    	{
			    	case 0:
			    	case 1:
			    	case 2:
			    			numberTmp=atoi(LineStart);
		    				// printf("LineStart=%s,numberTmp=%d\n",LineStart,numberTmp);   
							sprintf(newlinStrRecored,"%s%d ",newlinStrRecored,numberTmp);
			    			break;

			    	case 3:
			    			numberTmp=atoi(LineStart);
		    				// printf("LineStart=%s,numberTmp=%d\n",LineStart,numberTmp);   
							sprintf(newlinStrRecored,"%s%d",newlinStrRecored,numberTmp);
			    			break;			    	
			    	case 4:

							sprintf(lastlinStrRecored,"%s %s\n",LineStart+5,newlinStrRecored);
			    			break;
			    	default:
			    			break;
				}


		        LineStart = strtok( NULL, ",");
		        k++;
		    }
	        printf( "%s ", lastlinStrRecored );
			addLineToFile(newDataFileForIris,lastlinStrRecored);	

            #ifdef  Nero_DeBuging10_26_16_
            if(objNUm > 0)
            {
                DataFlow=(void **)malloc(sizeof(void *)*objNUm);
                dataKind=(nero_s32int *)malloc(sizeof(nero_s32int *) * objNUm);                            
                for (k=0;k<objNUm;k++)
                {
                            // printf("%d:%d \n",charCounts,res);

                    DataFlow[k]=(void *)malloc((sizeof( char)*3));
                    linc=(char *)DataFlow[k];
                    memset(linc,0,3);

                    // linc[0]=chCharInTask[objNUm]
                    //len Of char =1  or 3,so in fact  you do not need to  copy 3 char,
                    //but  when you want to  use this data to create a  NeuronNode_ForChCharacter
                    //  obj  ,you  need  the len of DataFlow[k] is three  
                    //                                  DataFlow[k][0]=   tff->data[k+1]    
                    //                                  DataFlow[k][1]=   tff->data[k+2]  =0    (if  the data is a num)
                    //                                  DataFlow[k][2]=   tff->data[k+3]    =0   (if  the data is a num)                   

                    memcpy(linc,&(chCharInTask[k]),3);
                    dataKind[k]=NeuronNode_ForChCharacter;
            		printf("%s",linc);

                }
                printf("\n");        
                // 现在开始准备发送消息了
                // arg2.dataNum=objNUm;
                // arg2.dataKind=dataKind;
                // arg2.conf=&neroConf;
                // arg2.DataFlow=DataFlow;

                // memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
                // mymsg.type =MsgId_Nero_DataFlowProcess ;
                // msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
            }
            #endif
        }
        
        //打印utf8编码数据：
        #ifdef Nero_DeBuging10_01_14_
        if(charCounts >6700)
        {
            testsadkfjao.tmp=chChar[charCounts];
            testsadkfjao.end=0;
            printf("%s\n",(nero_s8int *)&testsadkfjao);
        }
        #endif

        p++;     
        charCountsInTask++;  
        // printf("%d:%d \n",charCounts,res);
        objNUm=0;
		LineStart=p;
		LineEnd=p;

    }
    NerOkMsg;
    close(fd);
    munmap(mapped_mem, flength);

	return NeroOK;


}













//this file wang to  add  line  to  file  ,which line is  a  str  
nero_s32int CreatedWordsIntoFile(nero_8int * FileName)
{
    nero_8int  fileName[FILEPATH_MAX];
    nero_8int  writeStr[StringLen_MAX];
	nero_s32int i=0,j;
	// nero_8int *mapped_mem, * p,*end;
 //        nero_8int       *linStart,*linEnd;
 	getcwd(file_path_getcwd,FILEPATH_MAX);
    sprintf(fileName,"%s%s",file_path_getcwd,FileName);
	createFile(fileName);


	printf("%s\n",fileName);
	for(i=0;i<10;i++)
	{

		for(j=0;j<10;j++)
		{
			sprintf(writeStr,"%d+%d=%d\n",i,j,i+j);
			addLineToFile(fileName,writeStr);

		}

	}

}



/*将ChUTF8 chChar[ChineseCharNum];数据进行排序*/
nero_s32int sortChar()
{
	
	



	return NeroOK;
}

/*输出f8Word * wordsHead里面的词*/
nero_s32int printWords(Utf8Word * wordsHead)
{
	Utf8Word   *tmp,*last;
	struct adkfjao  testsadkfjao;	
	
	last=wordsHead->next;
	testsadkfjao.end=0;
	nero_s32int i;	
	printf("printWords\n");
	while(last)
	{
		
	
	
		
		for (i=0;i<last->num;i++)
		{	
	
			#ifdef Nero_DeBuging1

			testsadkfjao.tmp=last->words[i];
			printf("%s",(nero_s8int *)&testsadkfjao);
			
			#endif	
	
		}
		printf("\n");
		last=last->next;
	}




	return NeroOK;
}

/*文件格式为一行一个词*/
/*注意wordsHead是头指针，所以第一个数据就wordsHead-》next*/
nero_s32int readUTF8FileForWords(nero_8int * FileName ,Utf8Word * wordsHead)
{
	nero_s32int fd;
	nero_s8int *mapped_mem, * p,*q,*end;
	nero_s32int flength = 1024,wordsLen;
/*	nero_us8int tmp;*/
	nero_s32int i;
/*	nero_s32int charLength=0;//该字节所占位数	*/
	Utf8Word   *tmp,*last;
	ChUTF8    *words;
	struct adkfjao  testsadkfjao;
	if(wordsHead)
		wordsHead->next=NULL;
	else
		return NeroError;
	void * start_addr = 0;
	fd = open(FileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	flength = lseek(fd, 1, SEEK_END);
//	write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
	lseek(fd, 0, SEEK_SET);
/*	write(fd, "7", 1);*/
	mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
	MAP_PRIVATE,       //不允许其它进程访问此内存区域
	fd, 0);


	last=NULL;
	p=mapped_mem;
	end=p+flength-1;
	while(1)
	{
	
	
		if( p >= end /*|| *(p) ==0x0a*/)
		{
/*			NerReportMsgError(nero_error_Id);*/
			break;
		}		

		/*搜索下一个  0x0a 就是换行符的位置*/
		q=p+1;
		while(*(q) !=0x0a /*&& *(q) !=0x09*/)/*换行符或者tab*/
			q=q+1;
		/*判断有几个字符*/
		wordsLen=(q-p)/3;
		words=(ChUTF8 *)malloc(sizeof(ChUTF8 )*wordsLen);
		for (i=0;i<wordsLen;i++)
		{
			memcpy(&words[i],p, 3);/*malloc分配的是连续内存*/	
			
					//打印utf8编码数据：
			#ifdef Nero_DeBuging1_
/*				printf("%d.\n",i);*/
				testsadkfjao.tmp=words[i];
				testsadkfjao.end=0;
				printf("%s",(nero_s8int *)&testsadkfjao);
			
			#endif
			p=p+3;
			
					
		}
		#ifdef Nero_DeBuging1_
		printf("\n");
		#endif
		/*将获取的词加入链表*/
		tmp=malloc(sizeof(Utf8Word));
		tmp->next=NULL;
		
		tmp->num=wordsLen;
		tmp->words=words;
		if(last)
		{
			last->next=tmp;
		
			
		}
		else 
		{
			wordsHead->next=tmp;
/*			last=tmp;*/
		}
		last=tmp;
		/*搜索下一个  词的开始位置*/

		p++;
		
		
		
	}
	
	return NeroOK;
}
nero_s32int readUTF8FileData(nero_8int * FileName)//
{
	struct adkfjao  testsadkfjao;
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
	#ifdef Nero_DeBuging10_01_14_
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
		// the  highest  len of uft8  code  for chinese is 3
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
			// printf("p=%x.\n",p);
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
				default:
						// printf("p=%x.\n",p);
						NerReportMsgError(nero_error_Id);
						return NeroError;
			}
		}

		#ifdef Nero_DeBuging2_
		if(charLength  == 3)
		{

			printf("readUTF8FileData字符：%c%c%c(%x %x %x)\n",chChar[charCounts].first,chChar[charCounts].second,chChar[charCounts].third,chChar[charCounts].first,chChar[charCounts].second,chChar[charCounts].third);

		}
		#endif			
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
		#ifdef Nero_DeBuging10_01_14_
	if(charCounts >6700)
		{
			testsadkfjao.tmp=chChar[charCounts];
			testsadkfjao.end=0;
			printf("%s\n",(nero_s8int *)&testsadkfjao);
		}
		#endif

/* */
		// printf("%d:%d \n",charCounts,res);

		p+=6;
		charCounts++;
	}
	
	
	
	NerOkMsg;
	close(fd);
	munmap(mapped_mem, flength);

	return NeroOK;

}

nero_s32int nero_log(nero_8int * FileName,nero_8int * msg)
{


	 addLineToFile(FileName,msg);


	return 0;

}
nero_s32int nero_printNeroLink(nero_8int * FileName,void *n)
{
	nero_8int  msg[5000];
	nero_8int  str[500];
	nero_s32int p=0;

	NeuronObject * tmpObi;
	NeuronObject * nero;
	NerveFiber  *  curFiber;
	
	nero=(NeuronObject * )n;
	sprintf(str,"\n\n\n概念id=%d ，kind=%d \n",(int)nero,nero_GetNeroKind(nero));
	memcpy(msg+p, str, sizeof(str)+1);
	p=sizeof(str)+1;
	
	sprintf(str,"	其数据链表中的obj id：\n");
	memcpy(msg+p, str, sizeof(str)+1);
	p=sizeof(str)+1;
	
	curFiber=nero->inputListHead;
	while(curFiber)
	{
		tmpObi=curFiber->obj;
		
		sprintf(str,"		数据概念id=%d ，kind=%d \n",(int)tmpObi,(int)nero_GetNeroKind(tmpObi));
		memcpy(msg+p, str, sizeof(str)+1);
		p=sizeof(str)+1;		
		
		curFiber=curFiber->next;
	}
		
	sprintf(str,"	其输出链表中的obj id：\n");
	memcpy(msg+p, str, sizeof(str)+1);
	p=sizeof(str)+1;
	
	curFiber=nero->outputListHead;
	while(curFiber)
	{
		tmpObi=curFiber->obj;
		
		sprintf(str,"		输出概念id=%d ，kind=%d \n",(int)tmpObi,(int)nero_GetNeroKind(tmpObi));
		memcpy(msg+p, str, sizeof(str)+1);
		p=sizeof(str)+1;		
		
		curFiber=curFiber->next;
	}
	
		
	
	 addLineToFile(FileName,msg);
	return 0;
}






















