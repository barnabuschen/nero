#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */
#include <stdio.h>
#include <string.h>





#include "createDot.h"
#include "../common/fileOperating.h"

/*extern void emptyFile(char * fileName);*/
/*extern void createFile(char * fileName);*/



/*nero_s8int str1[]={ "digraph graphname {\n\*/
/*     a -> b -> c;			\n\*/
/*     b -> d;				\n\*/
/* }\n"};*/


nero_s8int str1[]={ "digraph graphname {\n"};
nero_s8int str2[]={ "\n}\n"};

/*2013年11月16日下一步要遍历整个以GodNero为起点(遍历它下层的对象的网络*/
void createNeroNetDotGraphForWords(NeuronObject *GodNero,  char *fileName)
{

	nero_s32int fd;
	nero_s8int * p;
	nero_s32int flength = 1024,kind;
	nero_us8int tmp;
	nero_s32int i,isbase_;
	nero_8int str[400];
/*	nero_us32int i,j;*/
	nero_s32int ObjectKind,ObjectKind2;
	NeuronObject * BaseObi;
	NeuronObject * tmpObi,*tmpObi2;
	NerveFiber  *  curFiber;
	NerveFiber  *  outputFiberOfbaseObj,*outputFiberOfDerivativeObj;
						NerveFiber *tmpFiber1;
						nero_us32int c=1;		
	void * start_addr = 0;
	
	emptyFile( fileName);
	fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	flength = lseek(fd, 1, SEEK_END);
	write(fd, "\0", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
	lseek(fd, 0, SEEK_SET);
/*	write(fd, "7", 1);*/

	
	
	
	sprintf(str,"%s\n",str1);
	write(fd, str, strlen(str));
	/*要遍历整个以GodNero为起点(遍历它下层的对象）的网络*/



	/*首先你需要清楚这个网络的特点：*/
	/*GodNero：指向所有的基类，且是单向联系*/
	curFiber=GodNero->outputListHead;
	for (;curFiber !=NULL;curFiber=curFiber->next)
	{
		//首先遍历GodNero指向的基类
		
		BaseObi=curFiber->obj;
		ObjectKind=nero_GetNeroKind(BaseObi);
		
			
		#ifdef   Nero_DeBuging0_1
		printf("%s\n",str);
		printf("BaseObi地址%d   ObjectKind=%d.\n",BaseObi,ObjectKind);			
		#endif	
		
		
		/*现在遍历每个基类下面的数据：*/
					
		outputFiberOfbaseObj=BaseObi->outputListHead;
		while(outputFiberOfbaseObj)
		{
			tmpObi=outputFiberOfbaseObj->obj;/*每一个衍生类*/
			ObjectKind=nero_GetNeroKind(tmpObi);	
			
			/*输出所以不是指向基类的连接*/	
			outputFiberOfDerivativeObj=tmpObi->outputListHead;
			while(outputFiberOfDerivativeObj)
			{	
	
				tmpObi2=outputFiberOfDerivativeObj->obj;/*每一个衍生类的输出对象*/
				ObjectKind2=nero_GetNeroKind(tmpObi2);	
				isbase_=nero_isBaseObj(tmpObi2);

				if(isbase_ != 1  || ObjectKind ==NeuronNode_ForChWord )
				{
				#ifdef   Nero_DeBuging22_11_13_

				printf("find words   \n");				
				#endif
				NeuronObject * tmp,*tmp2;
				switch(ObjectKind)
				{
					case NeuronNode_ForChCharacter:
						#ifdef   Nero_DeBuging22_11_13_
						
						printf("paint  Character   \n");	
						printf("概念id=%d ，输出对象id=%d \n",tmpObi,tmpObi2);
						#endif	
						tmp=tmpObi->inputListHead->obj;
						tmp2=tmpObi2->inputListHead->obj;
						if (ObjectKind2 == NeuronNode_ForChCharacter)
						{
							
							sprintf(str,"	%c%c%c -> %c%c%c;\n",tmp->x,tmp->y,tmp->z,tmp2->x,tmp2->y,tmp2->z);
						}
						else if(ObjectKind2 == NeuronNode_ForChWord )
							sprintf(str,"	%c%c%c -> %d;\n",(int)tmp->x,(int)tmp->y,(int)tmp->z,(int)tmpObi2);
							
/*						sprintf(str,"	%d -> %d;\n",tmp,tmp2);*/
						write(fd, str, strlen(str));			
							break;
					case NeuronNode_ForChWord:
						#ifdef   Nero_DeBuging22_11_13
						
						printf("paint  words   \n");	
						printf("概念id=%d ，输出对象id=%d \n",(int)tmpObi,(int)tmpObi2);
						#endif					
						/*只是输出这个词汇有那几个字*/
						if (ObjectKind2 == NeuronNode_ForChWord ){
						
							
						
						tmpFiber1=tmpObi2->inputListHead;
						static nero_s32int counttt=0;
						 c=1;
						while(tmpFiber1)
						{
							counttt++;
							if (c ==1)
							{
							tmp=tmpFiber1->obj->inputListHead->obj;
							sprintf(str,"	%d ->  %c%c%c",(int)tmpObi,/*counttt,*/(int)tmp->x,(int)tmp->y,(int)tmp->z);
							write(fd, str, strlen(str));								
							}
							else
							{
							
							
							tmp2=tmpFiber1->obj->inputListHead->obj;
							sprintf(str,"%c%c%c",(int)tmp2->x,(int)tmp2->y,(int)tmp2->z);
							write(fd, str, strlen(str));							}
							
							tmpFiber1=tmpFiber1->next;
							c++;
						}
						if (c != 1)
						{
							sprintf(str,";\n");
							write(fd, str, strlen(str));							
						}
						else
						{
							printf("没有数据%d \n",tmpObi2);	
						}
	
						}
						else
						{
							sprintf(str,"	%d -> %d;\n",(int)tmpObi,(int)tmpObi2);
							write(fd, str, strlen(str));						
						}
						break;
					
					
					
					default:break;
				}
				
				}
				outputFiberOfDerivativeObj=outputFiberOfDerivativeObj->next;
	
			}
			outputFiberOfbaseObj=outputFiberOfbaseObj->next;
		}
		
		
		
	}


	sprintf(str,"%s\n",str2);
	write(fd, str, strlen(str));
	
	close(fd);


}
void createNeroNetDotGraph(NeuronObject *GodNero,  char *fileName)
{
	nero_s32int fd;
	nero_s8int * p;
	nero_s32int flength = 1024;
	nero_us8int tmp;
	nero_s32int i;
	nero_8int str[400];
/*	nero_us32int i,j;*/
	nero_s32int ObjectKind;
	NeuronObject * BaseObi;
	NeuronObject * tmpObi;
	NerveFiber  *  curFiber;
	NerveFiber  *  outputFiberOfbaseObj;
		
	void * start_addr = 0;
	
	emptyFile( fileName);
	fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	flength = lseek(fd, 1, SEEK_END);
	write(fd, "\0", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
	lseek(fd, 0, SEEK_SET);
/*	write(fd, "7", 1);*/

	
	
	
	sprintf(str,"%s\n",str1);
	write(fd, str, strlen(str));
	/*要遍历整个以GodNero为起点(遍历它下层的对象）的网络*/



	/*首先你需要清楚这个网络的特点：*/
	/*GodNero：指向所有的基类，且是单向联系*/
	curFiber=GodNero->outputListHead;
	for (;curFiber !=NULL;curFiber=curFiber->next)
	{
		//首先遍历GodNero指向的基类
		
		BaseObi=curFiber->obj;
		ObjectKind=nero_GetNeroKind(BaseObi);
		
		
		
		#ifdef   Nero_DeBuging18_11_13_
		
		printf("%s\n",str);
/*		printf("%d.\n",ObjectKind);*/
			
		#endif
		
		
		
		sprintf(str,"	%d -> %d;\n",nero_GetNeroKind(GodNero),ObjectKind);
		write(fd, str, strlen(str));
		
		
		
		/*现在遍历每个基类下面的数据：*/
		
		outputFiberOfbaseObj=BaseObi->outputListHead;
		while(outputFiberOfbaseObj)
		{
			tmpObi=outputFiberOfbaseObj->obj;
			ObjectKind=nero_GetNeroKind(tmpObi);	
					
			NeuronObject * tmp=tmpObi->inputListHead->obj;
			
			sprintf(str,"	%d -> %c%c%c;\n",(int)nero_GetNeroKind(BaseObi),(int)tmp->x,(int)tmp->y,(int)tmp->z);
			write(fd, str, strlen(str));	
				
			outputFiberOfbaseObj=outputFiberOfbaseObj->next;
		}
		
		
		
	}





	sprintf(str,"%s\n",str2);
	write(fd, str, strlen(str));
	
	close(fd);


}











