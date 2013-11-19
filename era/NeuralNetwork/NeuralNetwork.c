
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "NeuralNetwork.h"
/*#include "Neuron.h"*/




nero_us32int neroKind[]=
{
 NeuronNode_ForImage,    //当一个概念节点的类型为此时表示一个图像对象
NeuronNode_ForLine,    //当一个概念节点的类型为此时表示一个线条对象

NeuronNode_ForChCharacter,    //当一个概念节点的类型为此时表示一个汉字
NeuronNode_ForChWord ,    //当一个概念节点的类型为此时表示一个中文词语
 NeuronNode_ForChSentence,    //当一个概念节点的类型为此时表示一个中文句子
};



/*ActNero只是用来保存基础数据，它对应于一个单纯的神经元*/


/*
将指定位置的位设置为特定的值：
1111
	&
0101



msg：
低位			高位
1-------8  9-----16 17-----24  25----32
1111 1111 1111 1111 1111 1111 1111 1111 
1-8位表示该节点种类



32位：区别一般的概念和基类，1表示基类，0表示衍生类(网络中真正的数据)
*/




 
 
 
/*typedef struct ActivationNeuron  NeuronObject;*/




ActNero NeroPool[MaxNeroNum];
nero_us32int nextAvailableNeroInPool;//它指向NeroPool中当前可用的（即使未加入网络的nero）
NeuronObject *GodNero;/*所有神经元理论上都最终与这个相通*/

/*下面是几个简单的判断函数*/
 inline  void  nero_putDataIntoNero(ActNero *n,nero_us32int x,nero_us32int y,nero_us32int z)
 {
 	if(n)
 	{
 	
/* 		printf("xyz=%d%d%d,\n",x,y,z   );*/
 		n->x=x;n->y=y;n->z=z;
/* 		nero_printNeroMsg(n) ;*/
 	
 	}
 
 }
 inline  nero_us32int nero_GetNeroKind(ActNero * nero)
{
	if(nero ==NULL)
		return  NeuronNode_ForNone;
		/*取出低8位*/
	return  (nero->msg  &  0x000000ff);
}
static inline void setActNeroKind(ActNero *nero,nero_us32int kind)
{
	
	if(nero ==NULL || kind <NeuronNode_ForNone || kind >=NeuronNode_Max)
		return ;
	nero->msg =nero->msg & 0xffffff00;//低8位清零
	nero->msg =nero->msg | kind;//低8位清零
}
/*区别一般的概念和基类*/
static inline void setActNeroAsBaseObject(ActNero *nero,nero_us32int kind)
{
	
	if(nero ==NULL || kind <NeuronNode_DerivativeObject || kind >NeuronNode_BaseObject)
		return ;
		
		
	if (kind == 1)
	{	/*把末k位变成1          | (101001->101111,k=4)      | x | (1 < < k-1) */
		nero->msg =nero->msg | (1<<(32-1));
	}
	else	
		nero->msg =nero->msg & 0x7fffffff;//第32位清零

}
static inline NerveFiber * addNerveFiber(ActNero *  n,nero_s32int type)
{
/*
#define NerveFiber_Input 1 
#define Relationship_Output  2
*/	NerveFiber  *tmp,**p;
	if(n ==NULL ||  type <NerveFiber_Input || type >NerveFiber_Output)
	{
		NeroErrorMsg;
		return NULL;
	}	
	
	if (type == NerveFiber_Input)
	{
		p=& (n->inputListHead);
	}
	else if(type == NerveFiber_Output)
	{
		p= & (n->outputListHead);
	}
	else
	{
		NeroErrorMsg;
		return NULL;
	}	

	while(*p != NULL  && (*p)->next != NULL)
	{
		p=&((*p)->next);
		
	}
	tmp=(NerveFiber *)malloc(sizeof(NerveFiber));
	tmp->next=NULL;
	if (*p == NULL)
	{
		*p=tmp;
	}
	else	
		(*p)->next=tmp;
	return  tmp;

}
void donother()
{


}
nero_s32int CreateActNeroNet()
{
	
	nero_s32int res;
	nero_s32int i;
	NeuronObject *BaseNeuronObject;
	/*do some init*/
	initNeroPool();
	
	/*首先一个网络你是否导入了数据必须有一些基本的构建*/

	/*显然它必须生成一些基本的神经对象，就像面向对象语言中万物。之母一样*/
	GodNero=(NeuronObject *)getNeuronObject();
	res=initActNero(GodNero,NeuronNode_ForGodNero,NULL,NULL);
	if(res == NeroError)
	{
		NeroErrorMsg;
		return res;
	}
	setActNeroAsBaseObject(GodNero,NeuronNode_BaseObject);
		/*现在生成其他基类*/
	for(i=0;i<sizeof(neroKind)/sizeof(nero_us32int);i++)	
	{
	
		BaseNeuronObject=(NeuronObject *)getNeuronObject();
		res=initActNero(BaseNeuronObject,neroKind[i],NULL,NULL);
		if(res == NeroError)
		{
			NeroErrorMsg;
			return res;
		}	
		/*将其他基类加入网络，他们与GodNero是相互联系的关系*/	
		setActNeroAsBaseObject(BaseNeuronObject,NeuronNode_BaseObject);
		addNeuronChild(GodNero,BaseNeuronObject,Relationship_FatherToChild);
		
		

	
	}

		printf("CreateActNeroNet   ok.\n");

	return NeroOK;
}


nero_s32int PointingToObject(NeuronObject *lower,NeuronObject *higher)
{
	/*很明显各个对象之间的连接关系需要额外的空间来存储
	你有俩种选择：使用神经元来保存，---浪费空间
		：    使用额外的结构，----似乎会麻烦
	这里还是觉得该用第二种方案
	*/
	if(lower ==NULL || higher ==NULL )
	{
		NeroErrorMsg;
		return NeroError;
	}	
	NerveFiber * newfiber=addNerveFiber(lower,NerveFiber_Output);
	if (newfiber ==NULL)
	{
		NeroErrorMsg;
		return NeroError;
	}
	newfiber->obj=higher;
	
	
	
	return NeroOK;
}
nero_s32int addNeuronChild(NeuronObject *father,NeuronObject *child,nero_s32int Relationship)
{
	if(child ==NULL || father ==NULL || Relationship <Relationship_FatherToChild || Relationship >Relationship_bothTother)
	{
		NeroErrorMsg;
		return NeroError;
	}
	switch(Relationship)
	{
	case Relationship_FatherToChild:
		PointingToObject(father,child);
		break;
		
	case Relationship_ChildToFather:
		PointingToObject(child,father);
		break;
		
	case Relationship_bothTother:
		PointingToObject(father,child);
		PointingToObject(child,father);
		break;
	default:break;

	}
	
	return NeroOK;
}


nero_s32int initNeroPool()
{

	nextAvailableNeroInPool=0;
	return NeroOK;
}

nero_s32int initActNero(ActNero * nero,nero_us32int kind,NerveFiber *inputListHead,NerveFiber *outputListHead)
{
	if(nero ==NULL || kind <NeuronNode_ForNone || kind >=NeuronNode_Max)
		return NeroError;
	setActNeroKind( nero,kind);
	nero->inputListHead=inputListHead;
	nero->outputListHead=outputListHead;




	return NeroOK;
}

nero_s32int initActNeroNet()
{







	return NeroOK;
}

NeuronObject * getNeuronObject()
{
/*	nero_us32int nextAvailableNeroInPool,cur ;*/

	if(nextAvailableNeroInPool <0 || nextAvailableNeroInPool >=MaxNeroNum)
	{
	
		printf("nero  pools  erro\n");;
		exit(0);
		return NULL;
		
		
	}
/*	cur=nextAvailableNeroInPool;*/
/*	nextAvailableNeroInPool++;*/
	return (&(NeroPool[nextAvailableNeroInPool++]));

}
/*创建一个衍生神经概念,并初始化*/
NeuronObject * nero_createNeroObj(nero_s32int kind)
{

	nero_s32int res;
	NeuronObject * newObj=(NeuronObject *)getNeuronObject();
	res=initActNero(newObj,kind,NULL,NULL);
	if(res == NeroOK)
	{
		
		setActNeroAsBaseObject(newObj,NeuronNode_DerivativeObject);
	}	
	else
		NeroErrorMsg;

	
	return newObj;

}
/*创建一个数据存储 神经元,并初始化*/
ActNero * nero_createDataNero()
{

	nero_s32int res;
	ActNero * newObj=(NeuronObject *)getNeuronObject();
	res=initActNero(newObj,NeuronNode_ForData,NULL,NULL);
	if(res == NeroOK)
	{
		
/*		setActNeroAsBaseObject(newObj,NeuronNode_DerivativeObject);*/
	}	
	else
		NeroErrorMsg;

	
	return newObj;

}
/*ActNero * nero_createNeroForData(nero_s32int  num)*/
/*{*/





/*}*/
/* 首先申请足够多的神经元，来保存数据，这些神经元成单向连接起来，返回头节点*/
ActNero * nero_GetSomeNeroForData(nero_s32int  num)
{
	ActNero * head;
	ActNero * tail;
	ActNero * lasttail;
	nero_s32int i,res;
	NerveFiber * fiber;
	if (num<1)
	{
		return NULL;
	}
	for (i=1;i<=num;i++)
	{
		
		tail=nero_createDataNero();
		
		
		
		if (i==1)
		{
			head=tail;
		}
		
		else
		{
			/*将上一个tail指向这个新的tail*/
			fiber=addNerveFiber(lasttail,NerveFiber_Output);
			fiber->next=NULL;
			fiber->obj=tail;
		
		
		}
		lasttail=tail;
	}

	return head;

}

nero_s32int nero_addDataToZhNeroObj(NeuronObject * n,ChUTF8 *chChar)
{


	/* 首先申请足够多的神经元，来保存数据，这些神经元成单向连接起来，返回头节点*/
	ActNero * dataNero=nero_GetSomeNeroForData(1);//只要一个神经元就好了，因为，这里只需要chChar的前3个数据
	/*将将概念神经元的inputListHead指向这个数据链表*/
	if(dataNero)
	{
		NerveFiber * fiber=addNerveFiber(n,NerveFiber_Input);
		fiber->obj=dataNero;
	
		/*现在开始数据填充*/
		nero_putDataIntoNero(dataNero,chChar->first,chChar->second,chChar->third);
	}
	else
		return NeroError;
	return NeroOK;
}
nero_s32int nero_addZhCharIntoNet(NeuronObject *GodNero,ChUTF8 chChar[],nero_s32int charCounts)
{

	nero_s32int i,res;
	NeuronObject *newObj;
	/**/

	
	/*首先生成一个神经概念*/

	for (i=0;i<charCounts;i++)
	{
		#ifdef  Nero_DeBuging18_11_13_ 
		if (i>20)
		{
			break;
		}
		#endif
		
		 newObj= nero_createNeroObj(NeuronNode_ForChCharacter);
		if(newObj)
		{
			/*往概念填数据*/
			nero_addDataToZhNeroObj(newObj,&chChar[i]);
			
			#ifdef  Nero_DeBuging18_11_13_0
			printf("new nero:   kind=%d.\n",nero_GetNeroKind(newObj));
			#endif			
			
			/*最后加入网络*/
/*			nero_printNeroMsg(newObj) ;*/
			nero_addNeroIntoNet( GodNero,newObj);
		}
	}




	return NeroOK;
}

nero_s32int nero_addNeroIntoNet(NeuronObject *GodNero,NeuronObject *newObj)
{
	if(GodNero ==NULL || newObj ==NULL)
	{
		return NeroError;
		
		
	}
	nero_us8int tmp;
	nero_s32int i;
	nero_8int str[400];

	nero_s32int BaseObjectKind,newObjKind;
	NeuronObject * BaseObi;
	NerveFiber  *  curFiber;	
	/*要遍历整个以GodNero为起点(遍历它下层的对象）的网络*/
			#ifdef  Nero_DeBuging18_11_130
			printf("newObj Kind=%d.\n",nero_GetNeroKind(newObj));
			#endif	


	/*首先你需要清楚这个网络的特点：*/
	/*GodNero：指向所有的基类，且是单向联系*/
	curFiber=GodNero->outputListHead;
	newObjKind=nero_GetNeroKind(newObj);
	for (;curFiber !=NULL;curFiber=curFiber->next)
	{
		//首先遍历GodNero指向的基类
		
		BaseObi=curFiber->obj;
		BaseObjectKind=nero_GetNeroKind(BaseObi);
	
			#ifdef  Nero_DeBuging18_11_13_0
			printf("newObjKind=%d.BaseObjectKind=%d\n",newObjKind,BaseObjectKind);
			#endif			
		if(newObjKind == BaseObjectKind)
		{
			/*加入该区域*/
			
			 nero_addNeroIntoBaseObj(BaseObi,newObj);
			
		}		
		
/*		sprintf(str,"	%d -> %d;\n",nero_GetNeroKind(GodNero),ObjectKind);*/
/*		write(fd, str, strlen(str));*/
		
	}

	
	return NeroOK;
}
/*将一个已知类型的对象加入到该基类类型下面*/
/*将数据加入网络是一个核心的操作*/
/*其实这个BaseObi并不一定要是基类*/


nero_s32int nero_addNeroIntoBaseObj(NeuronObject *BaseObi,NeuronObject *newObj)
{
	nero_s32int res;
	if(BaseObi ==NULL || newObj ==NULL)
	{
		return NeroError;
		
		
	}

	res=addNeuronChild(BaseObi,newObj,Relationship_bothTother);
	return NeroOK;
}



























