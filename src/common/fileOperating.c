
#include <sys/ipc.h>
/*#include <syswait.h>*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <malloc.h>
#include <errno.h>
#include <glib.h>
// #include <gtk/gtk.h>
#include "fileOperating.h"


void  testDebugForFileOperating()
{

	

}




int    *    mystrToInt(char  *  str )
{
	
	if(str  == NULL)
		return  NULL;
    //~ char   szValue[]  =   "0x11";     
    int    nValude    =   0;         
    int   *   res;
    sscanf(str,"%x",&nValude);  
	
	res  =    (int  *)nValude;
	return  res;
	}

void createFile(char * fileName)
{

	FILE * myfile=fopen(fileName, "w+");
	printf("fileName: %s\n", fileName);
/*	usleep(2000);*/
	fclose(myfile);
	// printf("fclose  strerror: %s\n", strerror(errno));
}
void emptyFile(char * fileName)
{



	FILE * myfile=fopen(fileName, "w+");
	fclose(myfile);

}
int searchStrInFile(char * FileName,char * searchstr)
{

	g_return_if_fail( FileName != NULL);
	g_return_if_fail( searchstr != NULL);

	//char * fileName=configFile;
        int len1=0;
        int *loc= findAllLine(FileName,&len1);

//get every line by turns.and check if its ID is num
        int count=0;
        char * str;
        int start;
        int len;
	int lineLen=0;
	int findLine=-1;
        while(count <len1)
        {
                start=loc[(count)*2];
                len=(loc[(count+1)*2-1]-loc[(count)*2])+1;
		if(loc[(count+1)*2-1] != loc[(count)*2])
		{
		        str=(char *)getStrInFile(FileName,len,start);
	                //printf("line is:%s,len is%d,start is %d,count is %d \n",str,len,start,count);
			lineLen=strlen(str);
			if(lineLen <2  || (str[0]=='/' && str[1]=='/'))
			{
				count++;
				continue;
			}

			char *res=strstr(str,searchstr);

		        if(res != NULL)
		        {	findLine=count;
				break;
			}
		}
		count++;
	}

	return findLine;
	


}

char *myitoa(int value,char *string,int radix)
{
   int rt=0;   
   if(string==NULL)   
      return NULL;   
   if(radix<=0||radix>30)   
      return NULL;   
   rt=snprintf(string,radix,"%d",value);   
   if(rt>radix)   
      return NULL;   
   string[rt]='\0';   
   return string;   
}
void putSubStrIntoOther(char * a,char *b,int startA,int startB,int len)
{
	if(a && b)
	{
		
		int i=0;
		while(i <len)
		{
			if(a[i+startA] != '\0')
				b[i+startB]=a[i+startA];
			else 
				break;
			i++;
		}
		//printf("b is:%s\n",b);
	}
}
 char * getLineInFile(char * FileName,int num)
{
	g_return_if_fail( num > 0);
	//findAllLine(FileName,num);
        int line=0;
        int *loc= (int *)findAllLine(FileName,&line);
        printf("line is%d \n",line);
	int start=loc[(num-1)*2];
	int len=(loc[(num)*2-1]-loc[(num-1)*2])+1;
	//printf("start is%d \n",start);
//printf("len is%d \n",len);
	char * 	str;
	if(len > 0)
		{
		str=(char *)getStrInFile(FileName,len,start);
      		// len=strlen(str);

//		printf(" \n first len is:%d \n",len);
		}
	else 
		str=NULL;

	return str;
}


int * findAllLine(char * FileName,int * countline)
{	
	

	FILE * myfile=fopen(FileName, "r+");
	//to count how many lines in this file
	int count=0;
	char c;
/*
	while( ( c=fgetc(myfile) ) != EOF)
	{
	  printf("%c",c);
	  if( c == '\n')
		{
	 	count++;
		continue;
		}
	   else
	      {//if last line do not have \n,but file end
		c=fgetc(myfile);
		if( c == EOF)
		{
			count++;break;
		}	

	       }
	}
*/
	while( ( c=fgetc(myfile) ) != EOF)
	{
	if( c == '\n')
		{
	 	count++;
		//continue;
		}
	//printf("%c",c);
	}

        (* countline)=count;
	int * location=(int *)malloc(sizeof(int)*count*2);
	//now find all lines' location in file
	fseek(myfile,0, SEEK_SET);
	count=0;
	location[0]=0;
	count++;

	//printf("dd \n",ftell(myfile));
	char lastC='\0';
	char currentC;
	int countHuanHang=0;
        while( ( currentC=c=fgetc(myfile) ) != EOF)
        {
	//printf("currentC=%c \n",currentC);
        if( c == '\n')
	   {
		countHuanHang++;
		//printf("%d",ftell(myfile));
		int myPos=ftell(myfile)-2;
		location[count]=myPos;
		if(count >1 && lastC==c && c=='\n' )
			{
				location[count]=location[count-1];
				location[count+1]=location[count]+1;
				//fgetc(myfile);
				//location[count+2]=
			//	printf("here %d  count=%d \n",location[count],count);
				count+=2;
				lastC=c;
				continue;
			}
			
			//printf("%d  count=%d \n",location[count],count);
		count++;
		if( countHuanHang<(* countline))
		{
                location[count]=ftell(myfile);
				//printf("now %d  count=%d \n",location[count],count);
		}
		else if(countHuanHang == (* countline))//最后一个换行符
		{	count--;
			location[count]=ftell(myfile)-2;
		//	printf("last now %d  count=%d \n",location[count],count);
			break;
		}
		count++;
		
	     }      
	 lastC=c;
        }


	fclose(myfile);
	return location;
}
//向文件末尾插入一行
int addLineToFile(char * FileName,char * str)
{

	int  res=  putStrInFile( FileName, str,strlen(str),(-2));
	return res;




}

int     delStrInFile(char * FileName,int len,int point)//note:the point means the first char need to del
{
//check the filename first

//make sure that the' len' is not large than the whole file from point to end,but if it is,just del all char from 
//'point' to the end
	FILE * myfile=fopen(FileName, "r+");
	int res3;
	fseek(myfile,0, SEEK_END);

	int numFromPointToEnd=ftell(myfile);
	int wholeFileLen=numFromPointToEnd;
//	printf("1 %d\n",numFromPointToEnd);
	numFromPointToEnd=numFromPointToEnd-point;
	//if the' len' is  large than the whole file from point to end
	if(len >  numFromPointToEnd)
	{
	int fd=fileno(myfile);
	res3 = ftruncate(fd,point-1);
	return 2;
	}
	//now is normal case:	
          fseek(myfile,point+len, SEEK_SET);
          char chr;
          while((chr=fgetc(myfile))!=EOF)
          {
		 // printf("chr %c\n",chr);

                  fseek(myfile, (-1)*(len+1), SEEK_CUR);
                  fputc(chr, myfile);
                  fseek(myfile, len, SEEK_CUR);
          }
	  int fd=fileno(myfile);
	 res3 = ftruncate(fd,wholeFileLen-len-1);
	fclose(myfile);
	return 0;
}
void updataLine(char * FileName,char * newStr,int line)
{

	g_return_if_fail( FileName != NULL);
	g_return_if_fail( newStr != NULL);
	g_return_if_fail( line >= 0);
	printf("line is %d and newstr is :\n%s\n",line,newStr);

	char * fileName=FileName;
        int len1=0,res3;
        int *loc= findAllLine(fileName,&len1);
	//outPutSomeNumber(loc,len1*(-2));
	//int point =delLine(FileName,(line));
	//putStrInFile(FileName,newStr,strlen(newStr),point);


	//当line就是最后一行时单独处理
	if(line == (len1-1))
	{
		int fd = open(FileName,O_RDWR);
		
		res3=ftruncate(fd,loc[(line)*2]);
		close(fd);
		addLineToFile(FileName,newStr);
		addLineToFile(FileName,"\n");
		return;
	}

	//首先获取所要删除行之后的所以数据

	int len=(loc[(len1)*2-1]-loc[(line+1)*2])+1;
	printf("len1 is  \n%d\n",len1);
	//printf("len is  \n%d\n",len);
	
	char * laststr=getStrInFile(FileName,len,loc[(line+1)*2]);
	printf("laststr is  \n%s\n",laststr);


	//将该文件从所要删除行及后面的数据全部删除
		//FILE * myfile=fopen(FileName, "r+");
		//fseek(myfile,0, SEEK_END);
		//len= ftell(myfile);
		//fclose(myfile);

	int fd = open(FileName,O_RDWR);
	//int fd=fileno(myfile);
	//printf("loc[(line)*2]  \n%d\n",loc[(line)*2]);
	/*int res=*/ftruncate(fd,loc[(line)*2]);
	//int errsv = errno;
	close(fd);
	//printf("res is  and errno is\n%d  %d\n",res,errsv);
	//重新写入所要删除行之后的所以数据,bu不过得先写入一个换行符
//	if(line >0)
//		addLineToFile(FileName,"\n");
	addLineToFile(FileName,newStr);
	if(laststr)
		{
		addLineToFile(FileName,"\n");
		addLineToFile(FileName,laststr);
		}
	addLineToFile(FileName,"\n");
}



int delLine(char * FileName,int line)
{

	char * fileName=FileName;
        int len1=0;
        int *loc= findAllLine(fileName,&len1);	
	int start;
	int len;
	if(line == 0)
	{
		start=loc[(line)*2];//注意start是该行前面的那个换行符
		len=(loc[(line+1)*2-1]-start)+1+1;
	}
	else    
	{
		start=loc[(line)*2]-1;
       		len=(loc[(line+1)*2-1]-start)+1;
	}
	printf("delLine line is  and len1 is:\n%d  %d\n",line,len1);
	delStrInFile(fileName,len,start);

	//if(line == (len1-1))
	//	{
		//printf("addLineToFile  add 换行\n");
		addLineToFile(fileName,"\n");
		//}
	return start;
}


 char * getStrInFile(char * FileName,int len,int point)//返回获取的字符串，point为起始位置
{
//check the filename first

//try to open the file    sizeof(str)/sizeof(char)
        FILE * myfile=fopen(FileName, "r+");
	
	//if len <=0  or point <0 .just get the whole file
	if(len <=0 ||  point <0 )
	{
		fseek(myfile,0, SEEK_END);
		len= ftell(myfile);
		fseek(myfile,0, SEEK_SET);
	
	}
	else
        	fseek(myfile,point, SEEK_SET);

      //  int pos = ftell(myfile);
     //   printf("1 %d\n",pos);

	char * str=(char *)malloc(sizeof(char)*len+1);

	str[len]=0;// so you can see str as a string,and you can use strlen() to get its len
        fread(str,sizeof(char),len,myfile);
//printf("readNum is  %d\n",readNum);
	fclose(myfile);

        return str;

}
char * getStrInFile2(char * FileName,int len,int point)//返回获取的字符串，point为起始位置
{
//check the filename first

//try to open the file    sizeof(str)/sizeof(char)
        FILE * myfile=fopen(FileName, "r+");
	
	//if len <=0  or point <0 .just get the whole file
	if(len >0 &&  point <0 )//从文件末尾倒数取len个字符
	{
		fseek(myfile,-(1+len), SEEK_END);//SEEK_END最后一个字符应该是eof，且eof前面还有一个空字符，那么你
		//len=len*(-1);
		//len= ftell(myfile);
		//fseek(myfile,point, SEEK_SET);
		printf("1 \n");
	}
	else if(len <=0 && point <0 )
	{
		fseek(myfile,0, SEEK_END);

       int pos = ftell(myfile);
        printf("pos111 %d\n",pos);
		len= ftell(myfile);
		fseek(myfile,0, SEEK_SET);
	printf("2 \n");
	}
	else
		{
        	fseek(myfile,point, SEEK_SET);
/*		printf("3\n");*/
		}
       int pos = ftell(myfile);
/*        printf("pos %d\n",pos);*/

	char * str=(char *)malloc(sizeof(char)*len+1);

	str[len]=0;// so you can see str as a string,and you can use strlen() to get its len
        int readNum=fread(str,sizeof(char),len,myfile);
/*printf("readNum is  %d\n",readNum);*/
	fclose(myfile);

        return str;
	





}
int putStrInFile(char * FileName,char * str,int len,int point)//在文件特定位置插入一个字符串,point 												//is the  location of the str,if 
					// it is zero ,that means put the str at the start of file
{

//check the filename first

//try to open the file    sizeof(str)/sizeof(char)
	FILE * myfile=fopen(FileName, "r+");

	//if point = -1 means just add to the start of the file
	//if point = -2 means just add to the end of the file
	if(point == (-1))
	{
	fseek(myfile,0, SEEK_SET);
	}
	else if(point == (-2))
	{
	fseek(myfile,0, SEEK_END);
	}
	else
		fseek(myfile,point, SEEK_SET);

        ftell(myfile);
     //   printf("1 %d\n",pos);
	fwrite(str,sizeof(char),len,myfile);
	fclose(myfile);
	return 0;
}

void otput()
{
//	//const char *msg="Hello World\n";
//	printf("fileOperating is here \n");
//        gint32 file=open("data",O_RDWR);
//        gchar buffer[128];
//        gint32 haveRead=read(file,buffer,128);
//        //gint32 file=open("data",O_RDWR);
//        //gint32 haveRead=read(file,buffer,128);
//        buffer[haveRead]='\0';
//        g_print ("%s,\n",buffer);
//        close(file); 
////
//
//
//	 char * getstr= getStrInFile("data",0,1);
//	int len=strlen(getstr);
//	int count=0;
//	while(len>=0)
//	{
//		int cc=getstr[count++];
//  	len--;
//	printf("%d",cc);
//
//
//	}
//
//	printf("read str is here:%s",getstr);



//	int res=putStrInFile("data","=",1,0);
	
	//delStrInFile("data",4,3);

//	addLineToFile("data","\nim pig\n");



//	int line=0;
//	int *loc= findAllLine("data",&line);
//	printf("line is%d \n",line);
////	printf("%d \n",cc);
//      int len=line*2;
//      int count=0;
//      while(len>0)
//      {
//              int cc=loc[count++];
//      len--;
//      printf("%d \n",cc);
//	}




	 char * getstr= getLineInFile("data",4);
	if(getstr != NULL)
{
      int len=strlen(getstr);
      int count=0;
	printf(" \nlast len is:%d \n",len);
      while(len>0)
      {
              char cc=getstr[count++];
      len--;
      printf("%c \n",cc);


      }
}
   //   printf("read str is here:%s",getstr);


}















