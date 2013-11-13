#ifndef   FILENAME
#define   FILENAME
#include "type.h"
#endif

#include <sys/msg.h>
#include <sys/ipc.h>
#include<pthread.h>
#include <sys/shm.h>


#include <sys/mman.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <assert.h>

#include "Neuron.h"
/*#include "fileOperating.h"*/
/*#include "../graph/searchLine.h"*/
/*#include <cairo.h>*/
/*#include <cairo.h>*/


/*extern int   countNode(struct list_head  *cnt);*/
extern  int    putStrInFile(char * FileName,char * str,int len,int point);
extern	void createFile(char * fileName);
extern int putStrInFile(char * FileName,char * str,int len,int point);
extern char * getStrInFile2(char * FileName,int len,int point);



int   countNode2(struct list_head  *cnt)
{
	g_return_if_fail(cnt != NULL);

	struct list_head  *pos=cnt->next;
	ColorCnt * pcnt=NULL;
	int count=0;
	if(pos==cnt)
	{
/*	printf("now list is empty \n");*/
	return count;
	}
/*	printf("now list all spectialCol  \n");*/
	while(pos !=cnt && pos != NULL)
	{
		count++;
		pos=pos->next;

	}

		return count;


}
//获取序号为n的节点
inline Neo * GetNeuronNode(NeuronMan * nm,int n)
{
	if(n<0 || nm ==NULL )	
	{
		printf("GetNeuronNode error :n=%d,nm=%x\n",n,nm);
		return (NULL);
	}
	Neo * res=(nm->NeoData)+n;
	return res;

}
int GetLineManByNeo(int imageIndex,NeuronMan * nm,LineMan *manAllLine)
{
	if(imageIndex<=0 || nm ==NULL || manAllLine==NULL )	
	{
		printf("GetLineManByNeo error \n");
		return (-1);
	}
	
	
	//
	Neo * image=GetNeuronNode(nm,imageIndex);
	int lineCount=image->y;//不一定都是线条

	Neo * LineIndexNode;
	int LineIndex=(image->next)*(-1);//这个只是索引节点的id不是线条的
	int lineID;
	int i;
	Neo * line;
	int type;
	printf("lineCount =%d \n",lineCount);
	for (i = 0; i < lineCount; i += 1)
	{
		if(LineIndex == imageIndex)
		{
		printf("LineIndex == imageIndex error \n");
		return (-2);
		}
		
		LineIndexNode=GetNeuronNode(nm,LineIndex);//索引节点
		lineID=(LineIndexNode->y);
		line=GetNeuronNode(nm,lineID);
		type=(line->x);

		LineIndex=(LineIndexNode->next);
		//线判断是不是线条
		if(type != NeuronNodeForLine)
		{
			continue;
		}
		//这里默认所以线条的数据就在线条node的后面
		
		PosList *headOfPosList=malloc(sizeof(PosList));
		InitialInitialPOSHead( headOfPosList);
		PosList nodeOfPosList;

		int posCount=(line->y);
		int j;
		Neo * pos;
		int posID=(line->z);
/*		printf("\nline %d: posCount=%d,posID=%d\n",i,posCount,posID);*/
		for (j = 0; j < posCount; j += 1)
		{
			if(lineID == posID)
			{
				printf("lineID == posID \n");
				break;
			}
			pos=GetNeuronNode(nm,posID);//索引节点
/*			printf("	x=%d,y=%d \n",(pos->x),(pos->y));*/
			posID=(pos->next);


			nodeOfPosList.pos.x=(pos->x);
			nodeOfPosList.pos.y=(pos->y);
			insertPosList(&(headOfPosList->p),&nodeOfPosList);

		}
		LineMan LineManNode;
		LineManNode.posHead=headOfPosList;
		insertLineManNode2(&(manAllLine->p),&LineManNode);


	}
	return 1;
}

int GetNeurons(int n,NeuronMan * nm)
{
	if(n<=0 || nm ==NULL )	
	{
		printf("GetNeurons error \n");
		return (-1);
	}
	int FirstNoUseNeu=(nm->neoFNode).FirstNoUseNeu;
	int NeuronNum=(nm->neoFNode).NeuronNum;
	if((FirstNoUseNeu)<=0 || (NeuronNum) <=n || (FirstNoUseNeu)>= ((NeuronNum)-1))	
	{
		printf("GetNeurons error \n");
		return (-2);
	}

	int currentNeu=(FirstNoUseNeu);
	int LastNeu=(NeuronNum)-1;
	int restNeu=LastNeu-currentNeu+1;
	if(restNeu <=0 )	
	{
		printf(" no enough Neurons  \n");
		return (-3);
	}
	((nm->neoFNode).FirstNoUseNeu)+=n;

	return currentNeu;
}
int CreateImageNode(NeuronMan * nm)
{
	if( nm ==NULL )	
	{
		printf("CreateImageNode error \n");
		return (-1);
	}

	int node=GetNeurons(1,  nm);
	if(node<0)
	{
		return (-2);
	}
	
	Neo * res=(Neo *)(nm->NeoData)+node;
	res->x=NeuronNodeForImage;
	res->y=NoMSG;
	res->z=NoMSG;
	res->next=-1;//为负数表明是一个概念，而y等于0，表示没有线条
	
	return node;
}
int CreateListNode(NeuronMan * nm)
{
	if( nm ==NULL )	
	{
		printf("CreateListNode error \n");
		return (-1);
	}

	int node=GetNeurons(1,  nm);
	if(node<0)
	{
		return (-2);
	}
	
	Neo * res=GetNeuronNode(nm,node);
	res->x=NeuronNodeForLine;
	res->y=NoMSG;
	res->z=NoMSG;
	res->next=-1;//为负数表明是一个概念
	
	return node;

}
int FillListByPosList(int line,PosList * head,NeuronMan * nm)
{
	
	if(line <0  || head ==NULL )	
	{
		printf("FillListByPosList error \n");
		return 0;
	}

	//首先判断line节点是否正常
	Neo * linNode=GetNeuronNode(nm,line);
	int type =linNode->x;
	if(NeuronNodeForLine != type)
	{
		printf("FillListByPosList error \n");
		return -1;
	}
	int posCount= countNode2(&(head->p));
/*	printf("posCount  =%d \n",posCount);	*/
	PosList * curr;
	PosList * last;
	int j;
	for (j = 0; j < (posCount); j += 1)
	{
		curr=(PosList *)list_GetNUMn(&(head->p),(j+1));
		if(j==0)
		{
			int res=addPosToLine(line,curr,nm);
		}
		else
		{
			//除了第一个点直接填之外，其他的都填每个区间的向量
			//很简单，就是俩个点做减法
			last=(PosList *)list_GetNUMn(&(head->p),(j));
			PosList  new;
			new.pos.x=(curr->pos).x-(last->pos).x;
			new.pos.y=(curr->pos).y-(last->pos).y;
			int res=addPosToLine(line,&new,nm);
		}
		
	}
	return 1;
}
int addPosToLine(int line,PosList * posnode,NeuronMan * nm)
{
	if(nm == NULL || posnode ==NULL ||  line <0)	
	{
		printf("addPosToLine error \n");
		return 0;
	}
	Neo * linNode=GetNeuronNode(nm,line);
	int NewpotNode=GetNeurons(1,  nm);
	if(NewpotNode<0)
	{
		return (-2);
	}
	//只需要修改y，以及通过y找到最后一个点，加入链表就好了
	int potCount=(linNode->y );
	Neo * NewpotNodeNeu=GetNeuronNode(nm,NewpotNode);
	if(potCount == 0)//如果是第一个点就简单了，直接加入
	{
		(linNode->z )=NewpotNode;

	}
	else
	{
		//从
		int endNode=findListEnd(line,(linNode->z ),  nm, potCount);
		if(endNode <0)
		{
			return -3;
		}
		Neo * endpotNodeNeu=GetNeuronNode(nm,endNode);
		(endpotNodeNeu->next )=NewpotNode;
	
	}
		//给新加点赋值
		(NewpotNodeNeu->x )=(posnode->pos).x;
		(NewpotNodeNeu->y )=(posnode->pos).y;
		(NewpotNodeNeu->z )=LineThickness;
		(NewpotNodeNeu->next )=line;
	(linNode->y )+=1;

	return 1;
}
int findListEnd(int mark,int start,NeuronMan * nm,int Max)
{
	if(nm == NULL || mark <0 || start <0 ||  Max <=0 )	
	{
		printf("findListEnd error \n");
		return -1;
	}

	int i;
	Neo * Node;
	int endNode=start;
/*	int nextNode;*/
	for (i = 0; i < Max; i += 1)
	{
/*		endNode*/
		Node=GetNeuronNode(nm,endNode);
		if((Node ->next) == mark)
		{
			return endNode;
		}
		endNode=(Node ->next);
		
	}
	return -10;

}
int addNeoByLineMan(NeuronMan * nm,LineMan *manAllLine)
{
	if(nm == NULL || manAllLine ==NULL )	
	{
		printf("addNeoByLineMan error \n");
		return 0;
	}
	int coloutCount=manAllLine->count;
	if(coloutCount>0)
	{	int i=0,j=0;
		PosList * curr;
		PosList * next;
		PosList *currList;
		LineMan *currLineManNode;


		//创建一个图像概念   Neo * GetNeuronNode(NeuronMan * nm,int n)
		int imageNode=CreateImageNode( nm);
		#ifdef DeBugForaddNeoByLineMan
		printf("imageNode =%d \n",imageNode);
		#endif
		if(imageNode <0)
		{
			return -1;	
		}
		
		//
		for (i = 0; i < (coloutCount); i += 1)
		{

			currLineManNode=(LineMan *)list_GetNUMn(&(manAllLine->p),(i+1));
			currList=currLineManNode->posHead;

			if(currList == manAllLine)
			{
				printf("list_GetNUMn error  \n");	
				break;
			}

			//创建一个线条概念
			int newLineNode=CreateListNode( nm);
			if(newLineNode <0)
			{
				return -2;	
			}		
			#ifdef DeBugForaddNeoByLineMan
			printf("line (%d) newLineNode=%d \n",i,newLineNode);
			#endif
			//用PosList给线条概念填充数据
			int res1=FillListByPosList(newLineNode,currList,nm);
			if(res1 <1)
			{
				continue;
			}
			//将线条概念加入图像概念

			int res2=addLineToImage(newLineNode,imageNode, nm);
			if(res2 <0)
			{
				printf("sth error res2 <0\n");;
			}
 
		}
	}
	return 1;
}
int addLineToImage(int line,int image,NeuronMan * nm)
{
	if(nm == NULL || line <0 || image <0 )	
	{
		printf("addLineToImage error \n");
		return -1;
	}

	
	//line 和image都需要修改它们的链接关系
	//		image
	Neo * imageNode=GetNeuronNode(nm,image);
	int connectNUM=imageNode->y;
	if(connectNUM == 0)
	{
		int newConnecte=GetNeurons(1, nm);
		Neo * newConnecteNode=GetNeuronNode(nm,newConnecte);
		(newConnecteNode->x )=NeuronNodeForConnect;
		(newConnecteNode->y )=line;
		(newConnecteNode->z )=NextLevel;
		(newConnecteNode->next )=image;

		(imageNode->y)+=1;
		#ifdef DeBugForaddNeoByLineMan
		printf("0--imageNode->y =%d ,newConnecte=%d\n",imageNode->y,newConnecte);
		#endif
		(imageNode->next)=(-1)*(newConnecte);
	}
	else
	{
		int endNode=findListEnd(image,(imageNode->next )*(-1),  nm, connectNUM);
		Neo * endNodeConnecteNode=GetNeuronNode(nm,endNode);
		if(endNode <0)
		{
		#ifdef DeBugForaddNeoByLineMan
		printf("endNode =%d\n",endNode);
		#endif
			return -3;
		}

		int newConnecte=GetNeurons(1, nm);
		Neo * newConnecteNode=GetNeuronNode(nm,newConnecte);
		(newConnecteNode->x )=NeuronNodeForConnect;
		(newConnecteNode->y )=line;
		(newConnecteNode->z )=NextLevel;
		(newConnecteNode->next )=image;

		endNodeConnecteNode->next=newConnecte;
		#ifdef DeBugForaddNeoByLineMan
		printf("1--imageNode->y =%d ,newConnecte=\n",imageNode->y,newConnecte);
		#endif
		(imageNode->y)+=1;
/*		(imageNode->next)=(-1)*(newConnecte);*/
	}
	
	//		line
	Neo * lineNode=GetNeuronNode(nm,line);
	 connectNUM=lineNode->next;
	//线条对象没有指明与它联系的对象个数，一般情况下它只会有一个联系对象，所以在超过一个链接对象的情况暂时不处理
	if(connectNUM == -1)
	{
		int newConnecte=GetNeurons(1, nm);
		Neo * newConnecteNode=GetNeuronNode(nm,newConnecte);
		(newConnecteNode->x )=NeuronNodeForConnect;
		(newConnecteNode->y )=image;
		(newConnecteNode->z )=UpLevel;
		(newConnecteNode->next )=line;


		(lineNode->next)=(-1)*(newConnecte);
	}
	else
	{
			{
		printf("addLineToImage error \n");
		return -6;
			}
/*		int endNode=findListEnd(image,(imageNode->next ),  nm, connectNUM);*/
/*		Neo * endNodeConnecteNode=GetNeuronNode(nm,endNode);*/
/*		if(endNode <0)*/
/*		{*/
/*			return -3;*/
/*		}*/

/*		int newConnecte=GetNeurons(1, nm);*/
/*		Neo * newConnecteNode=GetNeuronNode(nm,newConnecte);*/
/*		(newConnecteNode->x )=NeuronNodeForConnect;*/
/*		(newConnecteNode->y )=line;*/
/*		(newConnecteNode->z )=NextLevel;*/
/*		(newConnecteNode->next )=image;*/

/*		endNodeConnecteNode->next=newConnecte;*/

/*		(imageNode->y)+=imageNode->y;*/

	}	


	return 1;
}
//读取数据
int ReadDataNeuronMan(char * fileName,NeuronMan * nm)
{
	nm->fileName=fileName;
	GetNewNeuronFileMsg(&(nm->neoFNode),fileName);
	nm->FileData=(char *)ReadNeuronData(&(nm->neoFNode),nm->fileName,&(nm->fd));
	(nm->NeoData)=(Neo *)(nm->FileData+(nm->neoFNode).data);
	return 1;
}
//jie解除数据
int munmapNeuronMan(NeuronMan * nm)
{
	//解除之前需要将NeoF进行更新
	
	memcpy( nm->FileData,&(nm->neoFNode),sizeof(NeoF));
	
	int res=munmap(nm->FileData, (nm->neoFNode).FleData); //解除映射
	if(res !=0)
	{
		printf("munmapNeuronMan error \n");
	}

    	close(nm->fd);
	return 1;
}
int printNeuronManData(NeuronMan * nm)
{
	//值打印被使用了的
	int len=(nm->neoFNode).FirstNoUseNeu;
	if(nm->FileData)
	{
		int i=0;
		for (i = 0; i <= (len); i += 1)
		{

			if((nm->NeoData)[i].x == 50 && (nm->NeoData)[i].next <0)
			{
				printf("\n\nimage:\n");
			}
			if((nm->NeoData)[i].x == 51 && (nm->NeoData)[i].next <0)
			{
				printf("\line:\n");
			}
			
			printf("	No %d:  %d %d  %d  %d \n",i,(nm->NeoData)[i].x,(nm->NeoData)[i].y,(nm->NeoData)[i].z,(nm->NeoData)[i].next);
		}	
	}
	else printf("read null\n");

		return 1;

}

int InitiaNeuronMan(char * fileName,NeuronMan * nm)
{
	nm->fileName=fileName;
	InitiaNeuronFileNode(&(nm->neoFNode));
	CreateNewNeuronFile(&(nm->neoFNode),nm->fileName);
	return 1;
}
int InitiaNeuronFileNode(NeoF * f)
{
/*	g_return_if_fail(f != NULL);*/
	
	f->NeuronType=1;
	f->data=sizeof(NeoF);
	f->NeuronNum=NeuronNumFirst;
	f->FleData=sizeof(NeoF)+sizeof(Neo)*NeuronNumFirst;
	f->FirstNoUseNeu=FirstNoUseNeuWhenCreate;
	f->flag3=0;
	f->flag4=0;
	f->flag5=0;
	f->flag6=0;
	f->flag7=0;
	f->flag8=0;
	f->flag9=0;
	f->flag10=0;
	f->flag11=0;
	f->flag12=0;
	f->flag13=0;
	f->flag14=0;
	f->flag15=0;
	f->flag16=0;
	f->flag17=0;
	return 1;
}
void printNeuronFileMsg(NeoF * f)
{
/*	g_return_if_fail(f != NULL);*/

	printf("NeuronType=%d\n",f->NeuronType);
	printf("data point=%d\n",f->data);
	printf("NeuronNum=%d\n",f->NeuronNum);
	printf("FleData=%d\n",f->FleData);
	printf("FirstNoUseNeu=%d\n",f->FirstNoUseNeu);

}
//新建一个文件
int  CreateNewNeuronFile(NeoF * f,char * fileName)
{
	if(f == NULL || fileName ==NULL )	
		return 0;

	createFile(fileName);

	//写入初始数据，除了文件头之外，默认初始化,20是NeoF
	int lenofNeoF=sizeof(NeoF)/sizeof(int);

	int len=lenofNeoF+NeuronNumFirst *4;//len指的是int个数
/*	printf("lenofNeoF=%d,len=%d\n",lenofNeoF,len);*/
	char * msg=calloc((len),sizeof(int));
	
/*	memset(msg,0, len*sizeof(int));*/
	memcpy( msg,f,sizeof(NeoF));
	putStrInFile(fileName,msg,(len)*sizeof(int),0);
	return 1;
}
int  GetNewNeuronFileMsg(NeoF * f,char * fileName)
{
	if(f == NULL || fileName ==NULL )	
	{printf("error GetNewNeuronFileMsg\n");
		return 0;
	}
	int msgLen=sizeof(NeoF);
	char * fileHead=getStrInFile2(fileName,msgLen,0);
	memcpy( f,fileHead,msgLen);	
/*	printNeuronFileMsg(f);*/
	return 1;
}
void *  ReadNeuronData(NeoF * f,char * fileName,int *Filefd)
{

	g_return_if_fail( f != NULL);
	g_return_if_fail( fileName != NULL);
	g_return_if_fail( Filefd != NULL);
	
	if(f == NULL || fileName ==NULL || Filefd ==NULL)
		{
		printf("error ReadNeuronData\n");
		return NULL;
		}
	GetNewNeuronFileMsg(f, fileName);

	int fd=open(fileName, O_RDWR);
	if(fd <0)
	{
		printf("error file\n");
		return NULL;
	}
	*Filefd=fd;
	int len=sizeof(Neo)*(f->NeuronNum);
	printf("len=%d,f->data=%d\n",len,f->data);
	printNeuronFileMsg( f);
/*	void * data=mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,fd, f->data);*/
	void * data=mmap(NULL,f->FleData,PROT_READ|PROT_WRITE,MAP_SHARED,fd, 0);//位移竟然必须是0

	if(data == 0xffffffff)
	{
		printf("error addr\n");
		return NULL;
	}
	
	return data;
}
int updataNeuronFileHead(NeoF * f,char * fileName)
{

	if(f == NULL || fileName ==NULL )
	{	
		printf("error updataNeuronFileHead\n");
		return 0;
	}

	int fd=open(fileName, O_RDWR);
	if(fd <0)
	{
		printf("error file\n");
		return 0;
	}
	int len=sizeof(NeoF);
/*	void * data=mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,fd, f->data);*/
	char * data=(char *)mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,fd, 0);//位移竟然必须是0
	memcpy(data,f,len);

	int res=munmap(data,len); //解除映射
	if(res !=0)
	{
		printf("end error \n");
	}
	
    	close(fd);
	return 1;
}

void addNeoToFile(NeoF * f,char * fileName,int  addNeoNUM)
{
	if(f == NULL || fileName ==NULL || addNeoNUM <1)
	{	
		printf("error addNeoToFile\n");
		return NULL;
	}

	GetNewNeuronFileMsg(f, fileName);
	
	f->NeuronType=1;
/*	f->data=sizeof(NeoF);*/
	f->NeuronNum+=addNeoNUM;
	f->FleData+=sizeof(Neo)*addNeoNUM;	
	updataNeuronFileHead(f,fileName);
	//写入数据大小
/*	int lenofNeoF=sizeof(NeoF)/sizeof(int);*/
	int len=addNeoNUM *4;//len指的是int个数
	char * msg=calloc((len),sizeof(int));
	
/*	memset(msg,0, len*sizeof(int));*/
/*	memcpy( msg,f,sizeof(NeoF));*/
	putStrInFile(fileName,msg,(len)*sizeof(int),-2);

}

int findImageNode(NeuronMan * nm,int *index)
{
	if(nm == NULL || index ==NULL )
	{	
		printf("findImageNode error\n");
		if(index)
		{
		
		index[0]=0;
		}
		return -1;
	}


	int startNode=FirstNoUseNeuWhenCreate;
	int endNode=(nm->neoFNode).FirstNoUseNeu;

	if(startNode >endNode)
	{
		printf("findImageNode error\n");
		if(index)
		{
		
		index[0]=0;
		}
		return -2;
	}
	
	int i=0,j=0;
	Neo *node=GetNeuronNode(nm,startNode);
	int type=0;
	for (i = startNode,j=0; i < endNode; i += 1)
	{
		if((node->next) <0  && (node->x) ==NeuronNodeForImage )
		{	
			j++;
			index[j]=i;
/*			printf("findImageNode =%d\n",i);*/
			if(j>=10)
			{
				break;
			}
			
		}
		node=node+1;
	}
	index[0]=j;
	printf("findImageCount =%d\n",j);
	return 1;
}





















