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
	NerveFiber  *  curFiber;
	
		
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
		
		
		
		#ifdef   Nero_DeBuging17/11/13 
		
		printf("%s.\n",str);
/*		printf("%d.\n",ObjectKind);*/
			
		#endif
		
		
		
		sprintf(str,"	%d -> %d;\n",nero_GetNeroKind(GodNero),ObjectKind);
		write(fd, str, strlen(str));
		
	}





	sprintf(str,"%s\n",str2);
	write(fd, str, strlen(str));
	
	close(fd);


}











