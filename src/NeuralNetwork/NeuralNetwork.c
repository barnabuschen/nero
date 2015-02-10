
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "NeuralNetwork.h"
/*#include "Neuron.h"*/
#include "../tools/readUTF8File.h"
#include "../tools/Nero_IO.h"


nero_us32int neroKind[]=
{
 NeuronNode_ForImage,    //当一个概念节点的类型为此时表示一个图像对象
NeuronNode_ForLine,    //当一个概念节点的类型为此时表示一个线条对象

NeuronNode_ForChCharacter,    //当一个概念节点的类型为此时表示一个汉字
NeuronNode_ForChWord ,    //当一个概念节点的类型为此时表示一个中文词语
 NeuronNode_ForChSentence,    //当一个概念节点的类型为此时表示一个中文句子
};


static struct  NeuronObjectMsg_    neroObjMsg_st;
static struct  NeuronObjectMsgWithStr_    neroObjMsgWithStr_st;






 
 
 
/*typedef struct ActivationNeuron  NeuronObject;*/



NeroConf neroConf;
ActNero NeroPool[MaxNeroNum];
/*ActNero NeroPool2[MaxNeroNum];*/
/*ActNero NeroPool3[MaxNeroNum];*/
nero_us32int nextAvailableNeroInPool;//它指向NeroPool中当前可用的（即使未加入网络的nero）
NeuronObject *GodNero;/*所有神经元理论上都最终与这个相通*/

#define tmpObiForTemporaryNUM   150
NeuronObject tmpObiForTemporary[tmpObiForTemporaryNUM];
/*下面是几个简单的判断函数*/
inline nero_s32int  nero_ifHasThisData(ActNero * n,nero_s32int x,nero_s32int y,nero_s32int z)
{
 	if(n)
 	{

 		
 		if(n->x == x  &&  n->y ==y  && n->z==z )
 		return  1; 

 	}
 	
		return  0; 
		
		

}
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
		/*取出低16位*/
	return  (nero->msg  &  0x0000ffff);
}
static inline void setActNeroKind(ActNero *nero,nero_us32int kind)
{
	
	if(nero ==NULL || kind <NeuronNode_ForNone || kind >=NeuronNode_Max)
		return ;
	nero->msg =nero->msg & 0xffff0000;//低16位清零
	nero->msg =nero->msg | kind;//低16位清零
}
/*设置纤维的更新时间*/
static inline void setFiberUpdataTime(NerveFiber * fiber,nero_us32int time)
{
	
	if(fiber ==NULL )
		return ;
	/*只修改低20位*/
	/*1111 1111 1111 1111 1111 0000 0000 0000 */
	time=time & 0x000fffff;//高12位清零,必须保证time不超最大值
	fiber->time =fiber->time & 0xfff00000;//低20位清零
	fiber->time =fiber->time | time;

}
/*获取纤维的更新时间*/
static inline nero_us32int getFiberUpdataTime(NerveFiber * fiber)
{
	nero_us32int time;
	if(fiber ==NULL )
		return  0;
	/*只修改低20位*/
	/*1111 1111 1111 1111 1111 0000 0000 0000 */
	time=fiber->time & 0x000fffff;//高12位清零,必须保证time不超最大值
/*	fiber->time =fiber->time & 0xfff00000;//低20位清零*/
/*	fiber->time =fiber->time | time;*/
	return time;
}
 inline nero_s32int getFiberPointToObjNum(NerveFiber * fiber)
{
	nero_us32int kind;
	
	if(fiber ==NULL )
		return nero_msg_unknowError;
/*1-------8  9-----16 17-----24  25----32
				  1111 1111 1100 1111 1111 1111 1111 1111 
				  
				  */		
	kind=fiber->msg1  & 0x00000c00;/*提取对应的俩位*/
	kind=kind  >>  10;
	
	
	if (kind >=Fiber_PointToUniqueObj  && kind <=Fiber_PointToUnnecessaryObj)
	{
/*	        if (kind == 3)*/
/*	        {*/
/*	                 printf("kind=%d.///////////////////////////\n",kind);*/
/*	        }*/
	       
		return kind;
	}
	else
	{
	        printf("getFiberPointToObjNum wrong  flag=%d\n",kind);
	
	}
	return nero_msg_unknowError;

}
NeuronObject *  getBaseObjName(NeuronObject * baseobj,NeuronObject * godNero)
{
	nero_s32int ObjectKind;
	NeuronObject *Obi;
	NerveFiber  *  curFiber;
        if (baseobj == NULL ||  nero_isBaseObj(baseobj) != 1)
        {
                return NULL;
        }
/*                Obi=getBasePointByObj(  godNero,baseobj);*/
	        curFiber=baseobj->outputListHead;
	        for (;curFiber !=NULL;curFiber=curFiber->next)
	        {
	        

		      Obi=curFiber->obj;
		      ObjectKind=nero_GetNeroKind(Obi); 
/*		      printf("getBaseObjName:  ObjectKind=%d.\n",ObjectKind);*/
	        
	                if ( (ObjectKind == NeuronNode_ForChCharacter  ||  ObjectKind ==NeuronNode_ForChWord) &&  getFiberType(curFiber)  ==   Fiber_PointToLowerLayer )
	                {
	                        return Obi;
	                }
	        
	        }
        return NULL;

}
/**/
 inline nero_s32int getFiberType(NerveFiber * fiber)
{
	nero_us32int kind;
	
	if(fiber ==NULL )
		return nero_msg_unknowError;
/*1-------8  9-----16 17-----24  25----32
				  1111 1111 0011 1111 1111 1111 1111 1111 
				  
				  */		
	kind=fiber->msg1  & 0x00000300;/*提取对应的俩位*/
	kind=kind  >>  8;
	
	
	if (kind >=Fiber_PointToData  && kind <=Fiber_PointToSameLayer)
	{
/*	        if (kind == 3)*/
/*	        {*/
/*	                 printf("kind=%d.///////////////////////////\n",kind);*/
/*	        }*/
	       
		return kind;
	}
	return nero_msg_unknowError;

}

NeuronObject * getBaseObjPoint(NeuronObject  *godNero,nero_s32int kind)
{

	nero_s32int ObjectKind;

	NeuronObject * BaseObi;
	NerveFiber  *  curFiber;
        if (godNero == NULL ||  kind <0  ||  kind >NeuronNode_MaxNewDerivativeClassId)
        {
                return NULL;
        }

	        curFiber=godNero->outputListHead;
	        for (;curFiber !=NULL;curFiber=curFiber->next)
	        {
	        

		      BaseObi=curFiber->obj;
		      ObjectKind=nero_GetNeroKind(BaseObi); 
	        
	                if (ObjectKind == kind)
	                {
	                        return BaseObi;
	                }
	        
	        }
        return NULL;
}
/*获取概念的基类*/
NeuronObject * getBasePointByObj(NeuronObject  *godNero,NeuronObject  * n)
{

	nero_s32int ObjectKind,tmpKind;

	NeuronObject * BaseObi;
	NerveFiber  *  curFiber;
	if (n == NULL)
	{
	        return NULL;
	}
	
        if (godNero == NULL   )
        {
                return NULL;
        }
                tmpKind=nero_GetNeroKind(n); 
	        curFiber=godNero->outputListHead;
	        for (;curFiber !=NULL;curFiber=curFiber->next)
	        {
	        

		      BaseObi=curFiber->obj;
		      ObjectKind=nero_GetNeroKind(BaseObi); 
	        
	                if (ObjectKind == tmpKind)
	                {
	                        return BaseObi;
	                }
	        
	        }
        return NULL;
}
 nero_s32int getObjOrderRule(NeuronObject  *godNero,ActNero *nero)
{
	nero_s32int rule;
	if(nero ==NULL)
		return nero_msg_ParameterError;
		
	NeuronObject * baseobj=getBasePointByObj(godNero,nero);
        
        return  getChildrenOrderRule(baseobj);
}
/*加强连接强度*/
static inline nero_s32int gainFiberStrengthen(NerveFiber * fiber,nero_us32int time)
{
	nero_us32int Strengthen,oldStrengthen;
	nero_us32int lasttime;
	nero_s32int chang=0;
	if(fiber ==NULL )
		return 0;
		
	/*此次和上次的更新时间会影响到底要不要增强这个链接*/
	lasttime=getFiberUpdataTime(fiber);
	setFiberUpdataTime(  fiber,time);
/*	chang=time-lasttime;*/
/*	chang=chang >>  ;*/
	if ((time-lasttime)  >  NeroForgetCycle)
	{
		chang=0;
	}
	else
		chang=1;
	/*1111 1111 1111 1111 1111 0000 0000 0000 */
	
	oldStrengthen =fiber->msg1 & 0x000000ff;//获取低8位
/*	Strengthen=Strengthen+chang;*/
/*printf("Strengthen=%d.  chang=%d\n",Strengthen,chang);*/
	if (oldStrengthen  == 1 && chang<0)
	{
		Strengthen=0;
		fiber->msg1=fiber->msg1   |  0x00000000;
	}
	else if (oldStrengthen < Fiber_StrengthenMax   )
	{
		Strengthen=oldStrengthen+chang;
		fiber->msg1=fiber->msg1  +chang;
	}
	else
	{
		/*如果原来链接强度离最大值已经非常近了，这次及直接设置为最大值*/
		Strengthen=Fiber_StrengthenMax;
		fiber->msg1=fiber->msg1   & 0xffffff00;
		fiber->msg1=fiber->msg1   | Strengthen ;
/*		printf("这次及直接设置为最大值\n");*/
/*		return (Fiber_StrengthenMax);*/
	}
	#ifdef Nero_DeBuging09_01_14_
/*	if ((fiber->msg1 & 0x000000ff)   >=Fiber_StrengthenMax)*/
	{
		printf("%d  chang=%d  to   %d\n",oldStrengthen,chang,Strengthen);
	}		
	#endif		
	return (Strengthen);
}
/*
#define Fiber_PointToUniqueObj	        0
#define	Fiber_PointToMutiObj	        1
#define	Fiber_PointToUnnecessaryObj	2
设置纤维指向的子概念的出现数量*/
static inline void setFiberPointToNums(NerveFiber * fiber,nero_us32int type)
{
	
	if(fiber ==NULL || type <Fiber_PointToUniqueObj || type >Fiber_PointToUnnecessaryObj)
		return ;
/*	printf("kind=%d.\n",kind);*/

        #define setFiberPointToNums_debug_msg_
	switch(type)
	{
	        fiber->msg1 =fiber->msg1  & 0xfffff3ff;/*先设置为00，再在下面修改*/
		case Fiber_PointToUniqueObj	:
				/*将第11  12位设置为00*/
				/*1-------8  9-----16 17-----24  25----32
				  1111 1111 1100 1111 1111 1111 1111 1111 
				  
				  */
				  /*不需要改了，已经是00了*/
/*				fiber->msg1 =fiber->msg1 | 0x00000c00;*/
                                #ifdef setFiberPointToNums_debug_msg
                                printf("1111111.\n");
                                #endif
				break;
		case Fiber_PointToMutiObj	:
				/*1111 1111 1110 1111 1111 1111 1111 1111 */
                                #ifdef setFiberPointToNums_debug_msg
                                printf("222222222. 1 fiber->msg1=%x\n",fiber->msg1);
                                #endif				
				fiber->msg1 =fiber->msg1  | 0x00000400;
                                #ifdef setFiberPointToNums_debug_msg
                                printf("222222222.  fiber->msg1=%x\n",fiber->msg1);
                                #endif
				break;
		case Fiber_PointToUnnecessaryObj	:
				/*1111 1111 1101 1111 1111 1111 1111 1111 */
				fiber->msg1 =fiber->msg1  | 0x00000800;	
                                #ifdef setFiberPointToNums_debug_msg
                                printf("3333333333.\n");
                                #endif					
				break;
/*		case Fiber_PointToSameLayer	:*/
				/*1111 1111 1111 1111 1111 1111 1111 1111 */
/*				fiber->msg1 =fiber->msg1  | 0x00000f00;	*/
/*				printf("kind=%d.///////////////////////////\n",kind);*/
/*				break;*/
		default:
	               #ifdef setFiberPointToNums_debug_msg
                                printf("44444444444.\n");
                                #endif
		break;	
		
	}
}


/*设置纤维指向的神经元与该纤维所属概念的关系*/
static inline void setFiberPointToKind(NerveFiber * fiber,nero_us32int kind)
{
	
	if(fiber ==NULL || kind <Fiber_PointToData || kind >Fiber_PointToSameLayer)
		return ;
/*	printf("kind=%d.\n",kind);*/
	switch(kind)
	{
	        fiber->msg1 =fiber->msg1  & 0xfffffcff;/*先设置为00，再在下面修改*/
		case Fiber_PointToData	:
				/*将第9  10位设置为00*/
				/*1-------8  9-----16 17-----24  25----32
				  1111 1111 0011 1111 1111 1111 1111 1111 
				  
				  */
				  /*不需要改了，已经是00了*/
/*				fiber->msg1 =fiber->msg1 | 0x00000c00;*/
				break;
		case Fiber_PointToUpperLayer	:
				/*1111 1111 1011 1111 1111 1111 1111 1111 */
				fiber->msg1 =fiber->msg1  | 0x00000100;
				break;
		case Fiber_PointToLowerLayer	:
				/*1111 1111 0111 1111 1111 1111 1111 1111 */
				fiber->msg1 =fiber->msg1  | 0x00000200;		
				break;
		case Fiber_PointToSameLayer	:
				/*1111 1111 1111 1111 1111 1111 1111 1111 */
				fiber->msg1 =fiber->msg1  | 0x00000300;	
/*				printf("kind=%d.///////////////////////////\n",kind);*/
				break;
		default:break;	
		
	}
}
/*设置基类的子类排序是否要固定*/
static inline void setChildrenOrderRule(ActNero *nero,nero_us32int rule)
{
	
	if(nero ==NULL || rule <0 || rule >1)
		return ;
		
		
	if (rule == 1)
	{	/*把末k位变成1          | (101001->101111,k=4)      | x | (1 < < k-1) */
		nero->msg =nero->msg | (1<<(32-2));
	}
	else	
		nero->msg =nero->msg & 0xbfffffff;// 1101  第31位清零

}
 inline nero_s32int getChildrenOrderRule(ActNero *nero)
{
	nero_s32int rule;
	if(nero ==NULL)
		return nero_msg_ParameterError;
		
		
		/*最后看31位是不是1*/
		rule=nero->msg   & 0x40000000;//
		if(rule != 0)
			return 1;
			
		return 0;

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

/*pointTotype指的是该纤维指向的概念与原来的概念的关系*/
static inline NerveFiber * addNerveFiber(ActNero *  n,nero_s32int type,nero_s32int pointTotype)
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
	tmp->msg1=0;
	tmp->time=0;/*neroConf.neroTime;*/
	setFiberPointToKind(tmp,pointTotype);
	setFiberUpdataTime(tmp,neroConf.neroTime);
	if (*p == NULL)
	{
		*p=tmp;
		
		
		
	}
	else	
		(*p)->next=tmp;
	return  tmp;

}
void resetNeroConf()
{
		neroConf.addNewObj=1;
	neroConf.addLevelObj=1;
	neroConf.neroTime=0;
	neroConf.ifReCreateLogFile=1;
	neroConf.addLevelObjAlways=0;
	neroConf.CreateNewBaseObjKind=0;
	neroConf.NewNeroClassID=NeuronNode_MinNewDerivativeClassId;
	
	
}
nero_s32int CreateActNeroNet()
{
	
	nero_s32int res;
	nero_s32int i;
	NeuronObject *BaseNeuronObject;
	/*do some init*/
	initNeroPool();
	/*设置neroConf全局配置*/
	resetNeroConf();

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
		
		switch(neroKind[i])
		{
		case   NeuronNode_ForChWord:
		        setChildrenOrderRule(BaseNeuronObject,1);
		        break;
		case   NeuronNode_ForChCharacter:
		        setChildrenOrderRule(BaseNeuronObject,1);
		        break;		
/*		*/
/*		case   :*/
/*		        break;		*/
/*		case   :*/
/*		        break;*/
		default:
		        setChildrenOrderRule(BaseNeuronObject,1);
		        break;		
		}

	
	}
	#ifdef Nero_DeBuging14_01_14_
	{
	NerveFiber  *curFiber=GodNero->outputListHead;
	printf("len=%d.\n",sizeof(neroKind)/sizeof(nero_us32int));
	for (;curFiber !=NULL;curFiber=curFiber->next)
	{
		nero_s32int BaseObi=curFiber->obj;
		nero_s32int ObjectKind=nero_GetNeroKind(BaseObi);
		#ifdef Nero_DeBuging14_01_14
		printf("BaseObi=%x.\n",BaseObi);
		printf("ObjectKind=%d.\n",ObjectKind);
/*		printf("curFiber->next=%x.\n\n",curFiber->next);*/

		#endif				
	
	
	
	}		
        }
	#endif	
		printf("CreateActNeroNet   ok.\n");

	return NeroOK;
}


nero_s32int PointingToObject(NeuronObject *lower,NeuronObject *higher,nero_s32int pointTotype)
{
	/*很明显各个对象之间的连接关系需要额外的空间来存储
	你有俩种选择：使用神经元来保存，---浪费空间
		：    使用额外的结构，----似乎会麻烦
	这里还是觉得该用第二种方案
	*/
	nero_s32int BaseObjectKind,newObjKind;
	NeuronObject * BaseObi;
	NerveFiber  *  curFiber;	
	
	
	
	if(lower ==NULL || higher ==NULL )
	{
		NeroErrorMsg;
		return NeroError;
	}	
	
	
	
	/*需要判断是不是已经有联系了*/

	curFiber=lower->outputListHead;
	for (;curFiber !=NULL;curFiber=curFiber->next)
	{
		if (curFiber->obj  == higher &&   getFiberType(curFiber)  == pointTotype)
		{
			return NeroOK;
		}
	
	
	}
		
	NerveFiber * newfiber=addNerveFiber(lower,NerveFiber_Output,pointTotype);
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
	/*
#define	Fiber_PointToData	0
#define	Fiber_PointToUpperLayer	1
#define	Fiber_PointToLowerLayer	2
#define	Fiber_PointToSameLayer	3
	*/
	case Relationship_FatherToChild:
		PointingToObject(father,child,Fiber_PointToLowerLayer);
		break;
		
	case Relationship_ChildToFather:
		PointingToObject(child,father,Fiber_PointToUpperLayer);
		break;
		
	case Relationship_bothTother:
		PointingToObject(father,child,Fiber_PointToLowerLayer);
		PointingToObject(child,father,Fiber_PointToUpperLayer);
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
	nero->msg=0;
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
	neroConf.UsedNeroNum=nextAvailableNeroInPool;
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
	#ifdef   Nero_DeBuging04_01_14_
	if (kind != NeuronNode_ForChCharacter)
	{
/*		printf("新建概念id=%d ，kind=%d \n",newObj,kind);*/
		char str[500];
		NeuronObject * tmpObj=newObj->inputListHead.obj;
		sprintf(str,"新建概念id=%d ，kind=%d data:<%x%x%x><%x %x %x>\n",newObj,kind,tmpObj->x,tmpObj->y,tmpObj->z,tmpObj->x,tmpObj->y,tmpObj->z);		
		nero_log("log/createNewObj.log",str);
	}
	
	
		
	#endif	
	
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
/* 首先申请足够多的神经元，来保存数据，这些神经元成单向连接起来，返回头节点
注意这行中的fiber指向类型

fiber=addNerveFiber(lasttail,NerveFiber_Output,Fiber_PointToSameLayer);
*/
ActNero * nero_GetSomeNeroForData(nero_s32int  num)
{
	ActNero * head;
	ActNero * tail;
	ActNero * lasttail;
	nero_s32int i;
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
			fiber=addNerveFiber(lasttail,NerveFiber_Output,Fiber_PointToData);
			fiber->next=NULL;
			fiber->obj=tail;
		
		
		}
		lasttail=tail;
	}

	return head;

}
/*往NeuronObject添加数据，使这个细胞存储一个字的数据，这个字保存在chChar中*/
nero_s32int nero_addDataToZhNeroObj(NeuronObject * n,ChUTF8 *chChar)
{


	/* 首先申请足够多的神经元，来保存数据，这些神经元成单向连接起来，返回头节点*/
	ActNero * dataNero=nero_GetSomeNeroForData(1);//只要一个神经元就好了，因为，这里只需要chChar的前3个数据
	/*将将概念神经元的inputListHead指向这个数据链表*/
	if(dataNero)
	{
		NerveFiber * fiber=addNerveFiber(n,NerveFiber_Input,Fiber_PointToData);
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

	nero_s32int i;
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
			
			#ifdef  Nero_DeBuging18_11_13_
/*			printf("new nero:   kind=%d.\n",nero_GetNeroKind(newObj));*/

		        char str[500];
		        char TMPstr[50];
		        NeuronObject * tmpObj=newObj->inputListHead->obj;
/*		         sprintf(TMPstr,"%d%d%d",tmpObj->x,tmpObj->y,tmpObj->z);*/
		         sprintf(TMPstr,"%c%c%c",(int)tmpObj->x,(int)tmpObj->y,(int)tmpObj->z);
		        sprintf(str,"概念id=%x ，kind=%d data:<%x%x%x><%s>\n",newObj,nero_GetNeroKind(newObj),tmpObj->x,tmpObj->y,tmpObj->z,TMPstr);		
		        nero_log("log/createNewObj.log",str);

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
		return nero_msg_ParameterError;
		
		
	}
/*	nero_us8int tmp;*/
/*	nero_s32int i;*/
/*	nero_8int str[400];*/

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
			 return  nero_msg_ok;
			
		}		
		
/*		sprintf(str,"	%d -> %d;\n",nero_GetNeroKind(GodNero),ObjectKind);*/
/*		write(fd, str, strlen(str));*/
		
	}

	
	return nero_msg_fail;
}
/*将一个已知类型的对象加入到该基类类型下面*/
/*将数据加入网络是一个核心的操作*/
/*其实这个BaseObi并不一定要是基类*/


nero_s32int nero_addNeroIntoBaseObj(NeuronObject *BaseObi,NeuronObject *newObj)
{
/*	nero_s32int res;*/
	if(BaseObi ==NULL || newObj ==NULL)
	{
		return NeroError;
		
		
	}

/*	res=*/addNeuronChild(BaseObi,newObj,Relationship_bothTother);
	return NeroOK;
}
/*判断是不是基类,是返回1*/
nero_s32int  nero_isBaseObj(NeuronObject *Obi)
{
	nero_s32int IsBase=0,kind;
	nero_us32int i;
	if(Obi ==NULL)
	{
		return IsBase;
		
	}
	kind=nero_GetNeroKind(Obi);
	if(kind  != NeuronNode_ForNone  && kind !=NeuronNode_ForData)
	{
		/*最后看最高位是不是1*/
		i=Obi->msg   & 0x80000000;//
		if(i != 0)
			return 1;
		 
	
	}
	return 0;

}
/*判断这俩个概念是不是在网络中存在，如果不存在，0，在返回1*/
nero_s32int nero_isInNet(NeuronObject *Obi)
{
	NerveFiber *tmpFiber;
	NeuronObject *tmpObi;
	nero_s32int IsInNet=0,isbase,isSame;
	/*首先你要判断这俩个概念是不是在网络中存在，如果不存在，则报错返回*/
	/*对于一个已经在网络中的数据一定满足系列条件*/
	if(Obi ==NULL)
	{
		return IsInNet;
		
	}
		/*1:它指向一个基类*/
	isbase=0;
	tmpFiber=Obi->outputListHead;
	while (tmpFiber)
	{
		tmpObi=tmpFiber->obj;
		/*判断是不是基类*/
		isbase=nero_isBaseObj(tmpObi);
		if(isbase ==1)
			break;
		tmpFiber=tmpFiber->next;
	}
		/*2:基类也指向它:这个条件现在成立，那以后呢 我觉得还是应该确保这一点，否则你要查询一个概念*/
		/*是否在网络中就没法查了*/
	isSame =0;
	if(isbase ==1)
	{
		/*tmpObi现在应该指向一个基类*/
		tmpFiber=tmpObi->outputListHead;
		while (tmpFiber)
		{
			tmpObi=tmpFiber->obj;
			/*判断是不是就是Obi*/
/*			isSame=nero_isBaseObj(NeuronObject *Obi)*/
			if(tmpObi == Obi)
			{
				isSame =1;
				break;
			}
			tmpFiber=tmpFiber->next;
		}	
		if(isSame == 1)	
			IsInNet=1;
	
	
	}
	
	return IsInNet;

}

/*判断该复杂对象是否由数组内的对象组成或者部分组成*/
/*
现在有一个逻辑上的bug，如果这些概念之间都是俩俩相连接的，那么无论如何最后发现
都是已经有高层概念的，
解决方法：判断该链接的类型是不是链接高层概念的
*/

/*2014年02月05日 星期三 14时42分56秒 ：新版本加入考虑子概念的顺序*/
nero_s32int  nero_ifMakeUpWithTheseObjs(NeuronObject *obj,NeuronObject *childred[],nero_s32int objNum)
{
/*	nero_us32int kind;*/
	nero_s32int orderRule,res;	
/*	NerveFiber *tmpFiber1;*/
/*	NeuronObject  *tmpObi;*/
	
	if (childred == NULL  || objNum <1  || obj==NULL) 
	{
	        printf("ifMakeUpWithTheseObjs childred=%x   objNum=%x  obj=%d\n",childred,objNum,obj);
	        return nero_msg_ParameterError	;
	}
		
	/**/
/*	flag=1;*/
	res=nero_msg_unknowError;
        orderRule=getObjOrderRule(GodNero,obj);
/*        printf("orderRule=%d.\n",orderRule);*/
        if (orderRule == 1)
        {
                res=nero_ifMakeUpWithTheseObjsInOrder( obj,childred, objNum);
        }
	else if (orderRule == 0)
        {
        
                res=nero_ifMakeUpWithTheseObjsNotInOrder( obj,childred, objNum);
        
        }
           
	

	return res;
}
/*完全按照顺序*/
nero_s32int  nero_ifMakeUpWithTheseObjsInOrder(NeuronObject *obj,NeuronObject *childred[],nero_s32int objNum)
{
	nero_s32int flag,i,orderRule,res;	
	NerveFiber *tmpFiber1;
	NeuronObject  *tmpObi;
		tmpFiber1=obj->inputListHead;
		flag=1;
/*		printf("tmpFiber1=%x.\n",tmpFiber1);*/
		for (i=0;tmpFiber1 != NULL   &&  i<objNum ;tmpFiber1=tmpFiber1->next,i++)
		{
			tmpObi=tmpFiber1->obj;
			
			if (tmpObi   !=  childred[i]  )
			{
/*			        printf("ifMakeUpWithTheseObjsInOrder: tmpObi=%x.\n",tmpObi);*/
				flag=0;
				break;
			}
		
		
			
		}
		if (flag  == 1 &&  i == (objNum ))
		{
/*		        printf("i=%d. objNum=%d\n",i,objNum);*/
		        return NeroYES;
		}
		
        return  NeroNO;

}
/*不完全按照顺序*/
nero_s32int  nero_ifMakeUpWithTheseObjsNotInOrder(NeuronObject *obj,NeuronObject *childred[],nero_s32int objNum)
{

	nero_s32int flag,i;	
	NerveFiber *tmpFiber1;
	NeuronObject  *tmpObi;
	/*遍历子概念数组，判断是不是构成了obj(看他有没有指向obj)，只要一个不是，就判断不是*/
	for (i=0;i<objNum ;i++)
	{
	
		tmpFiber1=childred[i]->outputListHead;
		flag=0;
		for (tmpObi=tmpFiber1->obj;tmpObi != NULL &&  tmpFiber1 != NULL;tmpFiber1=tmpFiber1->next)
		{
			tmpObi=tmpFiber1->obj;
			/*增加判断：判断该链接的类型是不是链接高层概念的*/
			if (obj   ==  tmpObi  &&  getFiberType(tmpFiber1)== Fiber_PointToUpperLayer)/*看能不能咋obj中找到子概念*/
			{
				flag=1;
				break;
			}
		
		
			
		}		
		/*如果flag为0就说明不能在obj中找到子概念*/
		if (flag == 0)
		{
			return NeroNO;
		}
	
	}

        return NeroYES;

}
nero_s32int  nero_ifMakeUpWithTheseObjs_old(NeuronObject *obj,NeuronObject *childred[],nero_s32int objNum)
{
/*	nero_us32int kind;*/
	nero_s32int flag,i;	
	NerveFiber *tmpFiber1;
	NeuronObject  *tmpObi;
	
	if (childred == NULL  || objNum <1  || obj==NULL) 
	{
	        printf("ifMakeUpWithTheseObjs childred=%x   objNum=%x  obj=%d\n",childred,objNum,obj);
	        return nero_msg_ParameterError	;
	}
		
				
	
	/**/
	flag=1;
/*	kind =nero_GetNeroKind(obj);*/
/*	if (kind != NeuronNode_ForChWord)*/
/*	{*/
/*		return NeroNO;*/
/*	}*/
	
	
	
	
	/*遍历子概念数组，判断是不是构成了obj(看他有没有指向obj)，只要一个不是，就判断不是*/
	for (i=0;i<objNum ;i++)
	{
	
		tmpFiber1=childred[i]->outputListHead;
		flag=0;
		for (tmpObi=tmpFiber1->obj;tmpObi != NULL &&  tmpFiber1 != NULL;tmpFiber1=tmpFiber1->next)
		{
			tmpObi=tmpFiber1->obj;
			/*增加判断：判断该链接的类型是不是链接高层概念的*/
			if (obj   ==  tmpObi  &&  getFiberType(tmpFiber1)== Fiber_PointToUpperLayer)/*看能不能咋obj中找到子概念*/
			{
				flag=1;
				break;
			}
		
		
			
		}		
		/*如果flag为0就说明不能在obj中找到子概念*/
		if (flag == 0)
		{
			return NeroNO;
		}
	
	}
	
	/*运行到这里说明都找到了*/
	return NeroYES;
}

/*判断tmpFiber2指向的对象是否一个词的概念，并且这个词由Obis里面的字，依次组成*/
/*返回1表示是*/
nero_s32int  nero_ifHasThisData_word(NeuronObject *obj,NeuronObject *childred[],nero_s32int objNum)
{
	nero_us32int kind;
	nero_s32int flag,i;	
	NerveFiber *tmpFiber1;
	NeuronObject  *tmpObi;
	
	if (childred == NULL  || objNum <2  || obj==NULL) 
		return 0;
				
	
	/**/
	flag=1;
	kind =nero_GetNeroKind(obj);
	if (kind != NeuronNode_ForChWord)
	{
		return 0;
	}
	
	
	tmpFiber1=obj->inputListHead;
	/*如果循环是因为if语句退出的，说明该对象是要找的概念*/
	for (i=0;i<objNum && tmpFiber1 !=NULL;i++)
	{
		tmpObi=tmpFiber1->obj;
		if (tmpObi   !=  childred[i])
		{
			flag=0;
			break;
		}
		
		
		tmpFiber1=tmpFiber1->next;
	}		
	if (i<  (objNum-1))
	{
		flag=0;
	}
	return flag;
			
}
/*判断是否已经从几个已知道俩个概念中生成一个了新的概念*/
/*判断方法是寻找几个对象是否指向一个共同的衍生对象*/
/*但是这里好像不同于只有俩个的时候，
函数NeuronObject * nero_createObjFromPair(NeuronObject *Obi1,NeuronObject *Obj2)
保证了任意俩个汉字概念不会有多个只有俩个字联系的概念（顺序不能乱），但是保证不了多个字之间，俩俩之间都只有一个
共同的概念

*/
nero_s32int   nero_IfHasObjFromMultiples(NeuronObject *Obis[],nero_s32int objNum)
{
	nero_s32int i;
	NerveFiber *tmpFiber1;
	NeuronObject *obj;
	nero_s32int flag;	
	if (Obis == NULL  || objNum <3)
		return NeroError;
	
	flag=0;
	for (i=0;i<objNum;i++)
	{
		/*基本思路可以是这样，首先判断指向的概念中是否有objNum个字符的词的概念，有则判断是不是
		所要查找的，只要第一个字的概念，没有指向这个词应该有的概念的数据，就说明没有*/
		tmpFiber1=Obis[i]->outputListHead;

		while(tmpFiber1 )
		{
			
			obj=tmpFiber1->obj;
			/*判断obj指向的对象是否一个词的概念，并且这个词由Obis里面的字，依次组成*/
			flag=  nero_ifHasThisData_word( obj,Obis, objNum);
			
			/*只要找到一个符合，就说明已经存在想要添加的新概念了，但是为了万一*/
			/*你也可以每个都检查一遍*/
			if (flag == 1)
			{
				return 1;
			}
			tmpFiber1=tmpFiber1->next;
		}			
	}	


	return 0;



}
NeuronObject *   nero_IfHasObjFromMultiples2(NeuronObject *Obis[],nero_s32int objNum)
{
	nero_s32int i;
	NerveFiber *tmpFiber1;
	NeuronObject *obj;
	nero_s32int flag;	
	if (Obis == NULL  || objNum <2)
		return NULL;
	
	flag=0;
	for (i=0;i<objNum;i++)
	{
		/*基本思路可以是这样，首先判断指向的概念中是否有objNum个字符的词的概念，有则判断是不是
		所要查找的，只要第一个字的概念，没有指向这个词应该有的概念的数据，就说明没有*/
		tmpFiber1=Obis[i]->outputListHead;

		while(tmpFiber1 )
		{
			
			obj=tmpFiber1->obj;
			/*判断obj指向的对象是否一个词的概念，并且这个词由Obis里面的字，依次组成*/
			#ifdef Nero_DeBuging09_01_14_
				neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;
				neroObjMsg_st.fucId = 2;
				neroObjMsg_st.Obi = obj;
				printf("nero_IfHasObjFromMultiples2 msg%x.\n",obj);
				msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);			
			#endif	
			#ifdef Nero_DeBuging09_01_14_
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi =obj;
			sprintf(neroObjMsgWithStr_st.str,"在nero_IfHasObjFromMultiples2中等待查找高层概念");
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
			#endif	
			/*你需要在这里判断输入的概念他们的连接是否是对的*/
			flag=  nero_ifHasThisData_word( obj,Obis, objNum);
			
			/*只要找到一个符合，就说明已经存在想要添加的新概念了，但是为了万一*/
			/*你也可以每个都检查一遍*/
			if (flag == 1)
			{
				return obj;
			}
			tmpFiber1=tmpFiber1->next;
		}			
	}	


	return NULL;



}
/* 这个函数与前面俩个相比，它无关乎概念的类型，Obis里面可以是任何类型，只关乎层次上是否有共同的上层  */
		/*基本思路可以是这样，就是判断*Obis里面是否有一个共同的上层概念，这个概念恰好包含了所有Obis
		里面的子概念，但是不一定集合记录的等价于的关系，而是这个上层概念包含了Obis里面的概念，还可能、
		有别的概念，
		*/
		/*如果是词组的话只要，判断obj指向的对象是否一个词的概念，并且这个词由Obis里面的字，依次组成
		但是这里不一定是词组，所以，你不清楚数据的存储格式，这里只能采用笨办法了：
		首先判断是否有共同的概念
		再次判断这些共同概念是否有一个切好包括所有Obis里面的概念
		*/
		
		/*因为根据系统的设计，所有的概念的outputListHead指向所有  相关的  其他概念  或者  上层基类
		即:一个概念并不一定知道他的下层是哪些
		
		
		这样就有一个问题了，就是说一个概念的必要组成是什么，是数据还是他的关系，因为有些抽象的概念是
		没有数据的，这样的概念就只剩下关系了
		
		那到底在该怎么判断Obis里面是否有一个共同的上层概念，这个概念恰好包含了所有Obis
		里面的子概念，可以存在这样的概念，它与obis里面所以的概念都相关，但是却并不是他们的上层概念
		
		这里需要区分由基类直接指向的衍生类和那些没有明确类型的抽象类
		这里必须说明的是，那些新生成的概念究竟是什么类型的  见神经网络记录 sheet   5系统概略图
		
		
		
		*/
nero_s32int   nero_IfHasObjFromMultiples3(NeuronObject *Obis[],nero_s32int objNum)
{

	/*根据类（对象）的定义：这个函数的目的是判断是否有一个类恰好或部分由这些子类构成*/
	nero_s32int i;
	NerveFiber *tmpFiber1;
	NeuronObject *obj;
	nero_s32int flag,kind,makeup;	
	if (Obis == NULL  || objNum <2)
		return NeroError;
	
	flag=0;
	/*循环首先找到所以Obis概念都指向的概念*/
	/*考虑到数组一般不会太过庞大，所以使用最简单的暴力遍历就好了*/
	for (i=0;i<objNum;i++)
	{

		tmpFiber1=Obis[i]->outputListHead;/*数据的神经链表，任何概念都会指向所有他的上层概念*/
						/*你只需要在这些上层概念中找有没有需要的概念*/
		while(tmpFiber1 )
		{			
			obj=tmpFiber1->obj;
			/*首先判断这个概念的类型是不是可能是要找的，有些基类类型不可能是要找的*/
			kind=nero_GetNeroKind(obj);
			switch(kind)
			{
				case NeuronNode_ForChCharacter: 
				case NeuronNode_ForNone: 
				case NeuronNode_ForGodNero: 
				case NeuronNode_ForData: 
				case NeuronNode_ForConnect: 

					flag=0;/*直接排除*/
					break;				
				default:
					flag=1;
					break;
		
			}			
/*			printf("flag=%d.  obj=%x\n",flag,obj);*/
			/*判断这个对象是否包含所以子类型*/
			if (flag == 1   &&  nero_isBaseObj(obj) !=1)
			{
				makeup=nero_ifMakeUpWithTheseObjs(obj, Obis,objNum);
/*				printf("makeup=%d\n",makeup);*/
				if (makeup == NeroYES)/*找到了要找的对象*/
				{
/*				        printf("找到了要找的对象\n");*/
					return NeroYES;
				}
			}
			
			
			tmpFiber1=tmpFiber1->next;
		}			
	}	


	return NeroNO;



}
NeuronObject *   nero_IfHasObjFromMultiples4(NeuronObject *Obis[],nero_s32int objNum)
{

	/*根据类（对象）的定义：这个函数的目的是判断是否有一个类恰好或部分由这些子类构成*/
	nero_s32int i;
	NerveFiber *tmpFiber1;
	NeuronObject *obj;
	NeuronObject *findobj;
	nero_s32int flag,kind,makeup;	
	if (Obis == NULL  || objNum <2)
		return NULL;
	
	flag=0;
	/*循环首先找到所以Obis概念都指向的概念*/
	/*考虑到数组一般不会太过庞大，所以使用最简单的暴力遍历就好了*/
	for (i=0;i<objNum;i++)
	{

		tmpFiber1=Obis[i]->outputListHead;/*数据的神经链表，任何概念都会指向所有他的上层概念*/
						/*你只需要在这些上层概念中找有没有需要的概念*/
		while(tmpFiber1 )
		{			
			obj=tmpFiber1->obj;
			/*首先判断这个概念的类型是不是可能是要找的，有些基类类型不可能是要找的*/
			kind=nero_GetNeroKind(obj);
			switch(kind)
			{
				case NeuronNode_ForChCharacter: 
				case NeuronNode_ForNone: 
				case NeuronNode_ForGodNero: 
				case NeuronNode_ForData: 
				case NeuronNode_ForConnect: 

					flag=0;/*直接排除*/
					break;				
				default:
					flag=1;
					break;
		
			}			
/*			printf("flag=%d.  obj=%x\n",flag,obj);*/
			/*判断这个对象是否包含所以子类型*/
			if (flag == 1  &&  nero_isBaseObj(obj) !=1)
			{
				makeup=nero_ifMakeUpWithTheseObjs(obj, Obis,objNum);
/*				printf("makeup=%d\n",makeup);*/
				if (makeup == NeroYES)/*找到了要找的对象*/
				{
					return obj;
				}
			}
			
			
			tmpFiber1=tmpFiber1->next;
		}			
	}	


	return NULL;



}
/*判断是否已经从俩个已知道俩个概念中生成一个了新的概念,有则返回这个对象*/
/*问题是万一不止一个共同的对象怎么办*/
NeuronObject *  nero_findSameObjFromPair(NeuronObject *Obi1,NeuronObject *Obj2)
{
	NerveFiber *tmpFiber1, *tmpFiber2;
	nero_s32int has;
	if(Obi1 ==NULL  || Obj2 ==NULL)
	{
		return NULL;
		
	}
	
	/**/
	tmpFiber1=Obi1->outputListHead;
	has=0;
	while(tmpFiber1 && has !=0)
	{
	
		tmpFiber2=Obj2->outputListHead;
		while(tmpFiber2)
		{		
	
			if(tmpFiber1->obj  ==  tmpFiber2->obj)
			{
			
				if(nero_isBaseObj(tmpFiber1->obj)   !=1)
				{
					has=1;
					break;
				}
	
			}
			tmpFiber2=tmpFiber2->next;
		}
	
		tmpFiber1=tmpFiber1->next;
	}
	if (has ==1)
	{
		return tmpFiber1->obj;
	}
	else
		return NULL;
		
/*	return has;*/

}
/*判断是否已经从俩个已知道俩个概念中生成一个了新的概念*/
/*判断方法是寻找俩个对象是否指向一个共同的衍生对象*/
nero_s32int  nero_IfHasObjFromPair(NeuronObject *Obi1,NeuronObject *Obj2)
{
	NerveFiber *tmpFiber1, *tmpFiber2;
	nero_s32int has;
	if(Obi1 ==NULL  || Obj2 ==NULL)
	{
		return NeroError;
		
	}
	
	/**/
	tmpFiber1=Obi1->outputListHead;
	has=0;
	while(tmpFiber1 && has !=0)
	{
	
		tmpFiber2=Obj2->outputListHead;
		while(tmpFiber2)
		{		
	
			if(tmpFiber1->obj  ==  tmpFiber2->obj)
			{
			
				if(nero_isBaseObj(tmpFiber1->obj)   !=1)
				{
					has=1;
					break;
				}
	
			}
			tmpFiber2=tmpFiber2->next;
		}
	
		tmpFiber1=tmpFiber1->next;
	}
	
	
	return has;

}


/*修改已有的基类，基类名为objs第一个的第一个字符，词组概念*/
NeuronObject * nero_ModifyBaseKind(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf)
{

        NeuronObject * tmp;
        nero_s32int i,mark,objKind,baseKind,baseDataKind,flag,ObjectKind,hasModify;
        NeuronObject *Obi,*tmpChildObi,* baseObj;
	NerveFiber *curFiber,* tmpFiber;

	/*参数检查*/
	if (objs == NULL  || godNero ==NULL  ||  objNum <2 || conf ==NULL)
	{
		return nero_msg_ParameterError;
	}

        /*首先把第一个字符或者词组类放在objs第一个位置*/
        mark=-1;
        for (i=0;i<objNum;i++)
        {
                tmp=objs[i];
                if (tmp == NULL)
                {
                        printf("ModifyBaseKind 严重错误\n");
                        return nero_msg_unknowError;
                }
                
                objKind=nero_GetNeroKind(tmp);
                if (objKind == NeuronNode_ForChWord   ||  objKind == NeuronNode_ForChCharacter)
                {
                        mark=i;
                      break;  
                }
        }
        if (mark != 0)
        {
               /*交换了*/ 
/*               printf("交换了\n");*/
               tmp=objs[0];
               objs[0]=objs[mark];
               objs[mark]=objs[0];
        }
        /*找到相应名字的抽象概念了*/
        curFiber=godNero->outputListHead;
        baseObj=NULL;
	for (;curFiber !=NULL ;curFiber=curFiber->next)
	{
	         Obi=curFiber->obj;
	         baseDataKind=nero_GetNeroKind(Obi);	        
	        if (baseDataKind >=NeuronNode_MinNewDerivativeClassId)
	        {
	                /*判断基类名和objs[0]是否相同*/
/*	                printf("IfCreateNewBaseObj:  baseDataKind=%d.\n",baseDataKind);*/
/*	                printf("getBaseObjName= %x  objs[0]=%x.\n",getBaseObjName(Obi,godNero),objs[0]);*/
	                if (getBaseObjName(Obi,godNero) ==  objs[0])
	                {
/*	                         printf("找到了相同名的基类  kind=%d\n",baseDataKind);*/
	                         baseObj=Obi;
	                         break;
	                }
	                
	        }
	
	
	}
 	if (baseObj ==NULL)
	{
		return nero_msg_ParameterError;
	}       
	/*从objs[1]开始判断，objs[0]是类型名字*/
	
	/*根据新数据流的子概念情况修改基类的子概念结构
	目前只实现在原来基础上增加已有数据类型的情况
	*/
/*	printf("ModifyBaseKind  :开始修改 objNum=%d\n",objNum);*/
	hasModify=flag=0;/*curFiber->obj被识别标志*/
	for (i=1,tmpFiber=baseObj->inputListHead; i<objNum &&  tmpFiber !=NULL;)
	{
	        ObjectKind=nero_GetNeroKind(objs[i]);            
		/*修改新概念的数据链表模板*/
		
		tmpChildObi=tmpFiber->obj;
/*		 printf("ObjectKind=%d  i=%d.  基类结构kind=%d\n",ObjectKind,i,nero_GetNeroKind(tmpChildObi));*/
		if (nero_GetNeroKind(tmpChildObi)  ==  ObjectKind )
		{
		        
		        if (flag ==0)/*该子概念第一次被识别*/
		        {
		                flag=1;
		        }
		        else if(flag ==1)
		        {
		                setFiberPointToNums(tmpFiber,Fiber_PointToMutiObj);
		                hasModify=1;
/*		                printf("hasModify=%d.\n",hasModify);*/
                               
                                printf("new PointToNums=%d. v=%x\n",getFiberPointToObjNum(tmpFiber),tmpFiber->msg1);
		        }
		        i++;
		}
		else
		{
		        tmpFiber=tmpFiber->next;
		        flag =0;
		}

/*		tmpFiber->obj= getBaseObjPoint(godNero,ObjectKind);*/
/*		if (tmpFiber->obj == NULL )*/
/*		{*/
/*                        printf("基类创建失败3\n");*/
/*		        return NULL; 		        */
/*		}*/
		/*建立新概念已经子对象之间的关系*/
/*		PointingToObject(Obis[i],newObi,Fiber_PointToUpperLayer);*/
/*#define Fiber_PointToUniqueObj	        0*/
/*#define	Fiber_PointToMutiObj	        1*/
/*#define	Fiber_PointToUnnecessaryObj	2*/


/*                setFiberPointToNums(tmpFiber,Fiber_PointToUniqueObj);	*/
			        
	}


        if (hasModify==1)
        {
               printf("ModifyBaseKind  :已经修改,baseKInd=%d\n",nero_GetNeroKind(baseObj));
        }
        return nero_msg_ok;
}

NeuronObject * nero_CreateNewBaseObj(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf)
{
	if (objs == NULL  || objNum <2 ||  godNero==NULL ||  conf==NULL)
	{
	        printf("nero_CreateNewBaseObj  参数错误\n");
	        return NULL;
	}
	
	
	nero_s32int i,ObjectKind,res;
	NeuronObject * BaseObi,* BaseNeuronObject;
        NerveFiber * tmpFiber;

        if (conf->NewNeroClassID  > NeuronNode_MaxNewDerivativeClassId)
        {
                printf("基类创建失败,种类过多\n");
		return NULL;              
        }
	BaseNeuronObject=(NeuronObject *)getNeuronObject();
	res=initActNero(BaseNeuronObject,conf->NewNeroClassID,NULL,NULL);
	(conf->NewNeroClassID)++;
	if(res == NeroError)
	{
                        printf("基类创建失败0\n");
			return NULL;
	}	
	/*现在设置基类的孩子概念类型*/
	/*从objs[1]开始判断，objs[0]是类型名字*/
	
	/*这里先不管数组中的子概念的类型是不是有重复连续相同的，都一一的在
	基类中做对应
	*/
	for (i=1;i<objNum;i++)
	{
	        ObjectKind=nero_GetNeroKind(objs[i]);            
		/*生成新概念的数据链表模板*/
		tmpFiber= addNerveFiber(BaseNeuronObject,NerveFiber_Input,Fiber_PointToData);
		if (tmpFiber == NULL )
		{
                        printf("基类创建失败2\n");
		        return NULL; 		        
		}
		tmpFiber->obj= getBaseObjPoint(godNero,ObjectKind);
		if (tmpFiber->obj == NULL )
		{
                        printf("基类创建失败3\n");
		        return NULL; 		        
		}
		/*建立新概念已经子对象之间的关系*/
/*		PointingToObject(Obis[i],newObi,Fiber_PointToUpperLayer);*/
/*#define Fiber_PointToUniqueObj	        0*/
/*#define	Fiber_PointToMutiObj	        1*/
/*#define	Fiber_PointToUnnecessaryObj	2*/
                setFiberPointToNums(tmpFiber,Fiber_PointToUniqueObj);	
			        
	}
	
	/*将其他基类加入网络，他们与GodNero是相互联系的关系*/	
	setActNeroAsBaseObject(BaseNeuronObject,NeuronNode_BaseObject);
	addNeuronChild(godNero,BaseNeuronObject,Relationship_FatherToChild);
		
	setChildrenOrderRule(BaseNeuronObject,1);/*默认为1*/



        /*添加基类和基类名称(默认就是objs[0]))的链接*/
        PointingToObject(objs[0],BaseNeuronObject,Fiber_PointToUpperLayer);
        PointingToObject(BaseNeuronObject,objs[0],Fiber_PointToLowerLayer);
         #ifdef  Nero_DeBuging06_02_14
        printf("基类创建成功，kind=%d,基类名称：%x\n",nero_GetNeroKind(BaseNeuronObject) ,objs[0]);
        
        printf("基类名=%x.-------------------------------------\n",getBaseObjName(BaseNeuronObject,  godNero));
        
       
        tmpFiber=BaseNeuronObject->inputListHead;
        while(tmpFiber)
        {
                printf("        childKind=%d.\n",nero_GetNeroKind(tmpFiber->obj));    
                tmpFiber=tmpFiber->next;
        }
        #endif
        
 	#ifdef Nero_DeBuging10_01_14_
				neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
				neroObjMsgWithStr_st.fucId =2;
				neroObjMsgWithStr_st.Obi =NULL;
				nero_s32int xxxxxx=NeuronNode_ForChCharacter;
				memcpy(neroObjMsgWithStr_st.str,&xxxxxx,sizeof(nero_s32int));


				msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
        #endif	       
        
        
        
        return BaseNeuronObject; 	

}
/*判断新概念的种类 	见神经网络记录 sheet   5系统概略图
	*/
nero_s32int nero_judgeNewObjKind(NeuronObject *Obis[],nero_s32int objNum)
{
	nero_s32int kind=NeuronNode_ForNone;
	nero_s32int i,sameKind,findKind;
	
	nero_s32int ObjectKind;
	NeuronObject * BaseObi;
	NerveFiber  *  curFiber;
	/*首先判断参数合法性*/
	if (Obis ==NULL || objNum <1)
	{
		return kind;
	}
	for (i=0;i<objNum;i++)
	{
		if (Obis[0] == NULL)
		{
			return kind;
		}
	}
	
	/*首先判断是不是特殊情况
	1:是不是都是同一个类
	
	
	如果objNum=1，sameKind=1;
	*/
	if (objNum >=2)
	{
	        
	
	        for (sameKind=1,i=1;i<objNum;i++)
	        {
		        if (nero_GetNeroKind(Obis[i])  != nero_GetNeroKind(Obis[i-1]))
		        {
			        sameKind=0;
			        break;
		        }
	        }
	}	
	/*排除objNum =1的情况*/
	if (sameKind == 1  &&  objNum >=2)
	{
		kind=nero_GetNeroKind(Obis[0]);
		switch(kind)
		{
			case NeuronNode_ForChCharacter: 
				kind=NeuronNode_ForChWord;/*难道就不是句子么*/
				break;
			case NeuronNode_ForNone: 
			case NeuronNode_ForGodNero: 
			case NeuronNode_ForData: 
				kind=NeuronNode_ForNone;
				break;				
			default:
				kind=NeuronNode_ForComplexDerivative;
				break;
		
		}	
		
		if (kind  !=NeuronNode_ForComplexDerivative)
		{
		        return kind;
		}		
		
	}
	
	{
                findKind=NeuronNode_ForComplexDerivative;
                
	        /*从基类列表中查找是否有合适的类*/
	        curFiber=GodNero->outputListHead;
/*	        printf("curFiber=%x.\n",curFiber);*/
	        for (;curFiber !=NULL;curFiber=curFiber->next)
	        {	
		        BaseObi=curFiber->obj;
		        ObjectKind=nero_GetNeroKind(BaseObi);
		        	                        
	                if (ObjectKind >= NeuronNode_MinNewDerivativeClassId)
	                {
	                      /*判断是不是可以组成ObjectKind类*/  
	                      
	                      /*逻辑上有问题：
	                      如果你在后面创建基类时，只要有重复就设置相应的fiber的标识
	                      来识别重复的类型，这样会导致nero_IfIsThisKind错误对应基类
	                      
	                      */
/*	                      printf("ObjectKind: =%d.\n",ObjectKind);*/
	                      i=nero_IfIsThisKind(Obis,objNum,BaseObi);
/*	                      printf("IfIsThisKind=%d.\n",i);*/
	                      if (i == NeroYES)
	                      {
/*	                               printf("judgeNewObjKind   findKind=%d.\n",ObjectKind);*/
	                              findKind=ObjectKind;
	                              break;
	                      }
	                      
	                }
	
	        }
	        kind=findKind;
	
	}
	
	
	return kind;
}
/*判断Obis里面的概念是否可以组成一个baseKindObj类型*/
nero_s32int nero_IfIsThisKind(NeuronObject *Obis[],nero_s32int objNum,NeuronObject * baseKindObj)
{
	if (Obis == NULL  || objNum <1 ||  baseKindObj==NULL)
	{
	        printf("nero_IfIsThisKind  参数错误\n");
	        return nero_msg_ParameterError;
	}
        NeuronObject *Obi;
	NerveFiber *curFiber;
        nero_s32int Kind,res,i,baseDataKind,childKind,FLAG,IfRepeat;

        Kind= nero_isBaseObj(baseKindObj);
        if (Kind == 0)
        {
 	        printf("nero_IfIsThisKind  参数不满足要求1\n");
	        return nero_msg_ParameterError;               
        }
        Kind= nero_GetNeroKind(baseKindObj);
        if (Kind  < NeuronNode_MinNewDerivativeClassId)
        {
                printf("nero_IfIsThisKind  参数不满足要求2\n");
                return nero_msg_ParameterError;    
        }
        /*接下来判断baseKindObj类型（基类）是否可以包含Obis：
        
        子类的排列顺序是否固定，1表示顺序固定，0表示顺序无所谓
        */
        i=getChildrenOrderRule(baseKindObj);
        res=NeroNO;
        FLAG=0;
/*        printf("getChildrenOrderRule=%d.objNum=%d\n",i,objNum);*/
        if (i == 1  )/*我觉得顺序固定的方式应该是符合大多数情况的才对*/
        {
                
                curFiber=baseKindObj->inputListHead;
/*                printf("curFiber=%x.\n",curFiber);*/
	       for (i=0;curFiber !=NULL  &&  i<objNum;curFiber=curFiber->next)
	       {
	         Obi=curFiber->obj;
	         baseDataKind=nero_GetNeroKind(Obi);
	         childKind=nero_GetNeroKind(Obis[i]);
	         IfRepeat=getFiberPointToObjNum(curFiber);
/*	         printf("baseDataKind=%d. childKind=%d\n",baseDataKind,childKind);*/
	         if (baseDataKind != childKind)
	         {
/*	                printf("end i=%d. baseDataKind=%d  childKind=%d \n",i,baseDataKind,childKind);*/
	                FLAG=1;
	                 break;
	         }
	         /*这里假设在基类的数据结构中没有连续几个都是一个类型的情况，这种连续几个相同类型
	         的都是用在Fiber加标记实现的
	         */
	          i++;   
	         if (getFiberPointToObjNum(curFiber) == Fiber_PointToMutiObj )
	         {
	                 while(i < (objNum)  &&   nero_GetNeroKind(Obis[i]) ==  baseDataKind)
	                 {
                                i++;        
	                 }	                 
	         }

	         
	       }         
/*	       printf("FLAG=%d.\n",FLAG);     */
                if ( FLAG ==0)
                {
                       res=NeroYES;  
                }   
                
                
                
        }
        else/*顺序无所谓*/
        {
        
                /*等待实现*/
                printf(" nero_IfIsThisKind 顺序无所谓 \n");
        }
        

	       
        
        
        
        return  res;
        

}
/*从多个>=2已知道俩个概念中生成一个新的概念，新概念的种类在函数内部自动判断，最后返回新对象指针*/
/**/
NeuronObject * nero_createObjFromMultiples(NeuronObject *Obis[],nero_s32int objNum)
{
	NeuronObject *newObi;
	NerveFiber *tmpFiber;
	nero_s32int newObiKind,res,i;
        #define createObjFromMultiples_DeBug_Msg
	if (Obis == NULL  || objNum <1)
	{
	        printf("nero_createObjFromMultiples  参数错误\n");
	        return NULL;
	}
		
		
	/*首先你要判断这些个概念是不是在网络中存在，如果不存在，则报错返回*/
	for (i=0;i<objNum;i++)
	{
		
		if ( nero_isInNet(Obis[i]) !=1  )
		{
		        #ifdef   createObjFromMultiples_DeBug_Msg
		        printf("nero_createObjFromMultiples  概念不在网络中\n");
		        #endif	
		        return NULL;	
		}
				
	}
	/*判断这些个对象是不是已经有生成过新概念了*/
	
	res=nero_IfHasObjFromMultiples3(Obis, objNum);
/*	printf("判断这些个对象是不是已经有生成过新概念了=%d.\n",res);*/
	if(res == NeroYES)
	{
	        #ifdef   createObjFromMultiples_DeBug_Msg
	        printf("nero_createObjFromMultiples  要创建的概念已经存在在网络中,objNum=%d\n",objNum);
	        #endif	
	        return NULL;	
	}
		
		
		
			
	/*判断新概念的种类 
	见神经网络记录 sheet   5系统概略图
	*/
	newObiKind= nero_judgeNewObjKind(Obis, objNum);
	if (newObiKind == NeuronNode_ForNone)
	{
	        #ifdef   createObjFromMultiples_DeBug_Msg
	        printf("nero_createObjFromMultiples  没有合适的类别\n");
	        #endif	
		return NULL;
	}	
		
/*	printf("createObjFromMultiples :newObiKind=%d.   childKind=%d\n",newObiKind,nero_GetNeroKind(Obis[0]));*/
	
	
	
	

	/*生成新概念，并加入网络*/
	newObi= nero_createNeroObj(newObiKind);
	res= nero_addNeroIntoNet( GodNero,newObi);
	if(nero_msg_ok != res)
	{
	        #ifdef   createObjFromMultiples_DeBug_Msg
		printf("概念加入网络失败id=%x Kind %d ,objNum=%d\n",newObi,newObiKind,objNum);
		
		

/*		neroObjMsg_st.MsgId = MsgId_Log_PrintObjMsg;*/
/*		neroObjMsg_st.fucId = 1;*/
/*		nero_us32int tmpi=0;*/
/*		*/
/*		for (;tmpi < objNum;tmpi++)*/
/*		{*/
/*			neroObjMsg_st.Obi = Obis[tmpi];*/
/*			msgsnd( Log_mq_id, &neroObjMsg_st, sizeof(neroObjMsg_st), 0);			*/
/*			*/
/*		}		*/
		 #endif	
		return NULL;
	
	}
		
	
	
	
	
	/*将新概念与旧概念生成联系,此外俩个旧概念，这里仅仅第一个指向第二个*/
	
	/*将新概念的数据指向这俩个对象*/
	#ifdef   Nero_DeBuging04_01_14_
	if (NeuronNode_ForComplexDerivative  == newObiKind)
	{
				char str[500];
				char str2[500];
				sprintf(str,"data/wordspic%d.dot",5);
				sprintf(str2,"xdot data/wordspic%d.dot",5);
				createNeroNetDotGraphForWords(GodNero, str);
				system(str2);
	}
	#endif	
	for (i=0;i<objNum;i++)
	{
		/*生成新概念的数据链表*/
		tmpFiber= addNerveFiber(newObi,NerveFiber_Input,Fiber_PointToData);
		tmpFiber->obj=Obis[i];	
		
		/*建立新概念已经子对象之间的关系*/
/*		addNeuronChild(newObi,Obis[i],Relationship_ChildToFather);*/
		PointingToObject(Obis[i],newObi,Fiber_PointToUpperLayer);
		if (i>0)
		{
/*			addNeuronChild(Obis[i],Obis[i-1],Relationship_ChildToFather);	*/
			PointingToObject(Obis[i-1],Obis[i],Fiber_PointToSameLayer);
		}
		#ifdef   Nero_DeBuging04_01_14_
/*		if (kind != NeuronNode_ForChCharacter)*/
		{
			printf("子概念id=%d linkto %d \n",Obis[i],newObi);
			printf("子概念id=%d linkto %d \n\n",Obis[i-1],Obis[i]);
		}
		nero_printNeroLink("log/ObjLink.log",(void *)Obis[i]);
		#endif				
	}
/*	nero_printNeroLink("log/ObjLink.log",(void *)newObi);*/

	#ifdef   createObjFromMultiples_DeBug_Msg
	if (newObi == NULL)
	{
	        printf("nero_createObjFromMultiples  未知错误，newObi=%x\n",newObi);
	}
	#endif	
	return newObi;


}
/*从俩个已知道俩个概念中生成一个新的概念，新概念的种类在函数内部自动判断，最后返回新对象指针*/
/**/
NeuronObject * nero_createObjFromPair(NeuronObject *Obi1,NeuronObject *Obj2)
{
	NeuronObject *newObi;
	NerveFiber *tmpFiber;
	nero_s32int newObiKind,res;
	/*首先你要判断这俩个概念是不是在网络中存在，如果不存在，则报错返回*/
	if ( nero_isInNet(Obi1) !=1  ||  nero_isInNet(Obj2) !=1)
	{
	
	        printf("nero_createObjFromPair:不在网络中\n");
	        return NULL;
	}
		
	
	
	
	/*判断这俩个对象是不是已经有生成过新概念了*/
	
	res=nero_IfHasObjFromPair( Obi1, Obj2);
	if(res == 1)
	{
	printf("nero_createObjFromPair:已经存在\n");
	return NULL;
	}
		

	/*判断新概念的种类*/
	if (nero_GetNeroKind(Obi1)   ==  nero_GetNeroKind(Obj2) )
	{
		newObiKind=nero_GetNeroKind(Obi1);
		switch(newObiKind)
		{
			case NeuronNode_ForChCharacter: 
				newObiKind=NeuronNode_ForChWord;
				break;
				
			default:
				newObiKind=NeuronNode_ForNone;
				break;
		
		
		}
		
		
	}
	else/*暂时只处理类型相同的情况*/
		return NULL;
	
	if (newObiKind == NeuronNode_ForNone)
	{
		return NULL;
	}
	/*生成新概念，并加入网络*/
	newObi= nero_createNeroObj(newObiKind);
	res= nero_addNeroIntoNet( GodNero,newObi);
	if(NeroOK != res)
		return NULL;
	
	
	
	
	/*将新概念与旧概念生成联系,此外俩个旧概念，这里仅仅第一个指向第二个*/
	
	/*将新概念的数据指向这俩个对象*/
	
	tmpFiber= addNerveFiber(newObi,NerveFiber_Input,Fiber_PointToData);
	tmpFiber->obj=Obi1;
	tmpFiber= addNerveFiber(newObi,NerveFiber_Input,Fiber_PointToData);
	tmpFiber->obj=Obj2;	
	
	
	
	
	
	addNeuronChild(newObi,Obi1,Relationship_ChildToFather);
	addNeuronChild(newObi,Obj2,Relationship_ChildToFather);
	addNeuronChild(Obj2,Obi1,Relationship_ChildToFather);
	
	
	return newObi;
}

/*将词的链表中的每个词加入网络(包含许多词)*/
nero_s32int  nero_AddWordsIntoNet(NeuronObject *GodNero,Utf8Word * wordsHead)
{
	
	NeuronObject *  newWords;
	Utf8Word   *last;
	NeuronObject  *word[25];/*定义一个指针数组，该数组保存一个词中每个字的概念的指针*/
	last=wordsHead->next;
	nero_s32int i,res;	

	
	while(last)
	{
		
	
		res=0;
		if(last->num > 1)
		{
		
			
			for (i=0;i<last->num;i++)
			{	
			
				/*搜索是不是已经有该字存在在网络中*/
				#ifdef   Nero_DeBuging22_11_13_
				ttt.tmp=last->words[i];
				printf("%s",(nero_s8int *)&ttt);				
				#endif
				
				word[i]=nero_IfHasZhWord( GodNero,&(last->words[i]),NeuronNode_ForChCharacter);
				/*如果找不到这个字，就结束此次循环，表示该词不适合这时候加入网络*/
				if (word[i] == NULL)
				{
				
					#ifdef   Nero_DeBuging22_11_13

					printf("找不到啊  \n");				
					#endif				
					res=0;
					break;
				}
				
				#ifdef   Nero_DeBuging22_11_13_
				else
				{
/*					printf("找到两人  \n");	*/
					ttt.tmp.first=word[i]->x;
					ttt.tmp.second=word[i]->y;
					ttt.tmp.third=word[i]->z;
					printf("%s",(nero_s8int *)&ttt);
				
				}				
				#endif					
				res=1;


	
			}
			/*如果组成该词的几个字都能在网络里面找到,则尝试生成新概念，就是把该词就加入网络*/
			if (res == 1)
			{
				if (last->num == 2)
				{
					newWords= nero_createObjFromPair(word[0],word[1]);
				}
				else/*last->num > 2*/
				{
/*					nero_s32int j=0;*/
/*					*/
/*					for (j=0;j<=i;j++)*/
/*					{*/
/*						tmpObiForTemporary[j]=*(word[0]);*/
/*					}*/
					newWords=nero_createObjFromMultiples(word,(i));
				
				}
				

				
								
				#ifdef   Nero_DeBuging22_11_13_

				if (newWords )
				{	
						PrintUtf8 ttt;
	ttt.end=0;	
					ttt.tmp=last->words[0];
					printf("%s",(nero_s8int *)&ttt);
					ttt.tmp=last->words[1];
					printf("%s \n",(nero_s8int *)&ttt);					
					printf("obj=%x.\n",newWords);
				}
				else
					printf("创建失败  \n");
				nero_printNeroMsg(word[0]);
				nero_printNeroMsg(word[1]);				
				#endif
					
				

				
					
			}			
			
		}
		last=last->next;
	}


	return NeroOK;
}
/*根据dataKind概念的种类，在增加一个特定种类的数据，在这个函数里面不需要判断是否已经存在这个概念*/
NeuronObject *  nero_addNeroByData(void *Data,nero_s32int dataKind)
{
	NeuronObject  *str[400];
	ChUTF8  words[400];
	NeuronObject *tmp;
	nero_s32int strlenInData,i;
	ChUTF8_  *wordP;
	ChUTF8 * wordP2;	
	#define nero_addNeroByData_debug_msg
	
	if (Data == NULL  || dataKind<NeuronNode_ForNone  || dataKind>NeuronNode_Max   )
	{
	        #ifdef nero_addNeroByData_debug_msg
	        printf("nero_addNeroByData:参数错误\n");
	        #endif
		return NULL;
	}
	tmp=NULL;
	switch(dataKind)
	{
	case NeuronNode_ForChCharacter:
		wordP2=(ChUTF8  *)Data;/*实际上只是一个ChUTF8而非ChUTF8_结构的数据，但是不影响结果*/

		tmp=nero_IfHasZhWord( GodNero,wordP2, dataKind);/*多余的*/
		
		if (tmp  == NULL)
		{
			 tmp= nero_createNeroObj(NeuronNode_ForChCharacter);
			if(tmp)
			{
				/*往概念填数据*/
				nero_addDataToZhNeroObj(tmp,wordP2);
			
				#ifdef  Nero_DeBuging18_11_13_
				printf("new nero:   kind=%d.data:%x %x %x \n",nero_GetNeroKind(tmp),wordP2->first,wordP2->second,wordP2->third);
				#endif			

			}			
		}
		else
		{
		         #ifdef nero_addNeroByData_debug_msg
		        printf("nero_addNeroByData，已经有该字符\n");
		         #endif			
		
		}
		break;
	
	case NeuronNode_ForChWord:
	
		/*这个时候Data就是一个由中文汉字组成的字符串*/
		/*首先找到这个字符串每个字的概念*/
		wordP=(ChUTF8_  *)Data;
		strlenInData=strlen((char *)Data);
		strlenInData=strlenInData/3;
		if (strlenInData >=400)
		{
		        printf("nero_addNeroByData，数据太大\n");
			break;
		}
		for (i=0;i<strlenInData;i++)
		{
			
			words[i].first=wordP[i].first;
			words[i].second=wordP[i].second;
			words[i].third=wordP[i].third;
			str[i]=nero_IfHasZhWord( GodNero,&(words[i]),NeuronNode_ForChCharacter);
			if (str[i]  == NULL)
			{
	                        #ifdef nero_addNeroByData_debug_msg
				printf("nero_addNeroByData:  找不到词组中的字符\n",strlenInData);

	                        #endif			        
			}
			
		}
		tmp= nero_IfHasObjFromMultiples2(str,i);
		if (tmp  == NULL)
		{
			 tmp= nero_createNeroObj(NeuronNode_ForChWord);
			if(tmp)
			{
				/*往概念填数据*/
/*				nero_addDataToZhNeroObj(tmp,wordP);*/
/*				if (strlenInData >=3)*/
				{
					tmp= nero_createObjFromMultiples(str, strlenInData);
				}
/*				else*/
/*					tmp=nero_createObjFromPair(str[0],str[1]);*/
/*					*/
				
	                        #ifdef nero_addNeroByData_debug_msg
				if (tmp == NULL)
				{
				        printf("nero_addNeroByData:概念创建失败,strlenInData=%d\n",strlenInData);
				}
	                        #endif				

			}
			else
			{
			
		                 #ifdef nero_addNeroByData_debug_msg
		                printf("nero_addNeroByData，createNeroObj失败\n");
		                 #endif				
			}
		}		
		else
		{
		         #ifdef nero_addNeroByData_debug_msg
		        printf("nero_addNeroByData，已经存在高层概念\n");
		         #endif	
		}
		break;
	case NeuronNode_ForChSentence:
		/*先不处理*/
		         #ifdef nero_addNeroByData_debug_msg
		        printf("nero_addNeroByData，ForChSentence\n");
		         #endif			
		break;

	default:
	#ifdef nero_addNeroByData_debug_msg
	printf("nero_addNeroByData:默认处理\n");
	 #endif	
	break;	
	
	
	}
	/*最后加入网络*/
	if (tmp != NULL)
	{
		i=nero_addNeroIntoNet( GodNero,tmp);
		if (i !=  nero_msg_ok)
		{
		        printf("addNeroByData  addNeroIntoNet fail \n");
		}
	}
	
	return tmp;



}
/*判断是否有这个数据相应类型的概念*/
NeuronObject * nero_IfHasNeuronObject(void *Data,nero_s32int dataKind,NeuronObject *GodNero)
{
	NeuronObject  *str[400];
	ChUTF8  words[400];
	NeuronObject *tmp;
	nero_s32int strlenInData,i;
	ChUTF8  wordP2;
	ChUTF8_  *wordP;
	nero_us8int  * ttt22;
	if (Data == NULL  || dataKind<NeuronNode_ForNone  || dataKind>NeuronNode_Max  || GodNero == NULL )
	{
		return NULL;
	}

	tmp=NULL;
	switch(dataKind)
	{
	case NeuronNode_ForChCharacter:
		ttt22=Data;
/*		wordP2=(ChUTF8  *)Data;*/
		wordP2.first=ttt22[0];
		wordP2.second=ttt22[1];
		wordP2.third=ttt22[2];
		#ifdef Nero_DeBuging14_01_14_
		
/*			printf("寻找字符2：%x %x %x.\n",wordP2[i].first,words[i].second,words[i].third);*/
			printf("寻找字符2：%c%c%c.\n",ttt22[0],ttt22[1],ttt22[2]);
		#endif	
		tmp=nero_IfHasZhWord( GodNero,&wordP2, dataKind);
		break;
	
	case NeuronNode_ForChWord:
	
		/*这个时候Data就是一个由中文汉字组成的字符串*/
		/*首先找到这个字符串每个字的概念*/
		wordP=(ChUTF8_  *)Data;
		strlenInData=strlen((char *)Data);
		strlenInData=strlenInData/3;
		if (strlenInData >=400)
		{
			printf("nero_IfHasNeuronObject strlenInData >=400  fail \n");
			break;
		}
		for (i=0;i<strlenInData;i++)
		{
			
			words[i].first=wordP[i].first;
			words[i].second=wordP[i].second;
			words[i].third=wordP[i].third;
			
			
			str[i]=nero_IfHasZhWord( GodNero,&(words[i]),NeuronNode_ForChCharacter);
			
			#ifdef Nero_DeBuging14_01_14_
			neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
			neroObjMsgWithStr_st.fucId = 1;
			neroObjMsgWithStr_st.Obi = str[i];
			sprintf(neroObjMsgWithStr_st.str,"在nero_IfHasNeuronObject中找到该对象");
			msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);			
			#endif				
			
		}
		tmp= nero_IfHasObjFromMultiples2(str,i);
		
		break;
	case NeuronNode_ForChSentence:
		/*先不处理*/
		break;

	default:break;	
	
	
	}

	return tmp;
}

/*根据给定数据寻找是否网络中已经有该   字   概念了，这里只搜索一个字,找到则返回该概念的指针*/
/*kind  控制搜索的领域*/
NeuronObject * nero_IfHasZhWord(NeuronObject *GodNero,ChUTF8 * word,nero_s32int kind)
{
	nero_s32int res;
/*	nero_us8int tmp;*/
	nero_s32int ObjectKind;
	NeuronObject * BaseObi;
	NeuronObject * tmpObi;
	NerveFiber  *  curFiber;
	NerveFiber  *  outputFiberOfbaseObj;
	
		#ifdef Nero_DeBuging14_01_14_
		
			printf("寻找字符3：%c%c%c.\n",word->first,word->second,word->third);
/*			printf("寻找字符3：%x %x %x.\n",ttt22[0],ttt22[1],ttt22[2]);*/
		#endif	
	#ifdef Nero_DeBuging14_01_14_
	curFiber=GodNero->outputListHead;
	for (;curFiber !=NULL;curFiber=curFiber->next)
	{
		BaseObi=curFiber->obj;
		ObjectKind=nero_GetNeroKind(BaseObi);
		#ifdef Nero_DeBuging14_01_14
		printf("BaseObi=%x.\n",BaseObi);
		printf("ObjectKind=%d.\n",ObjectKind);
		printf("curFiber->next=%x.\n\n",curFiber->next);

		#endif				
	
	
	
	}		

	#endif			
		
		
		
		
	curFiber=GodNero->outputListHead;
	for (;curFiber !=NULL;curFiber=curFiber->next)
	{
		//首先遍历GodNero指向的基类
		
		BaseObi=curFiber->obj;
		ObjectKind=nero_GetNeroKind(BaseObi);
			
		/*现在遍历基类下面的数据：*/
		#ifdef Nero_DeBuging14_01_14_
		printf("BaseObi=%x.\n",BaseObi);
		printf("ObjectKind=%d.\n",ObjectKind);
		printf("curFiber->next=%x.\n",curFiber->next);

		#endif			
		
		if(kind != ObjectKind)
			continue;
			
		outputFiberOfbaseObj=BaseObi->outputListHead;
		while(outputFiberOfbaseObj)
		{
			tmpObi=outputFiberOfbaseObj->obj;/*基类链表中的每一个衍生对象*/
			ObjectKind=nero_GetNeroKind(tmpObi);	
				
			/*比对该对象的数据是否是要找的数据*/	
			NeuronObject * tmp=tmpObi->inputListHead->obj;/*衍生对象的第一个数据*/
			
			res=nero_ifHasThisData(tmp,word->first,word->second,word->third);
			if(res == 1)
				return tmpObi;
			
/*			sprintf(str,"	%d -> %c%c%c;\n",nero_GetNeroKind(BaseObi),tmp->x,tmp->y,tmp->z);*/
/*			write(fd, str, strlen(str));	*/
			outputFiberOfbaseObj=outputFiberOfbaseObj->next;
		}
		
		
		
	}


	return NULL;
}


/*增强俩个对象的链接强度(a-》b单向的连接)，如果没有连接就添加一个*/

nero_s32int nero_StrengthenLink(NeuronObject * a,NeuronObject * b)
{
	nero_s32int res,iffind;
	NeuronObject * findObi;
	NerveFiber  *  curFiber;

	if (a == NULL  || b== NULL)
	{
		return nero_msg_ParameterError;
	}




	curFiber=a->outputListHead;
	for (iffind=0;curFiber !=NULL;curFiber=curFiber->next)
	{

		findObi=curFiber->obj;
			
		if (findObi == b)
		{
			/*找到了*/
			iffind=1;
			res=gainFiberStrengthen(curFiber,neroConf.neroTime);

		}
		
	}

	if (iffind == 0)
	{
		/*没有找到的话，就加一个连接*/
		PointingToObject(a ,b,Fiber_PointToSameLayer);
/*		res=gainFiberStrengthen(curFiber,neroConf.neroTime);*/
		res=0;
	}
	return res;	



}











