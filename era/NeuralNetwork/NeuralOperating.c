


#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "NeuralNetwork.h"
#include "NeuralOperating.h"
#include "../tools/readUTF8File.h"

#include "../common/error.h"




/*
DataFlow  数据的指针数组，该数据的解析方式需要依据dataKind来指定,一些特殊的数据，其长度该数据本身指定
dataKind   数据的类型,指出每个数据的类型
		dataKind=NeuronNode_ForChCharacter  则DataFlow就是一个汉字的utf8码
		dataKind=NeuronNode_ForChWord  则DataFlow就是一个中文词组的utf8码，不能有符号,必须保证最后一个字节为0
		dataKind=NeuronNode_ForChSentence 则DataFlow就是一个中文句子的utf8码

2013年12月18日newStart

dataNum	   数据的指针数组数据的个数，就是数组的长度

0：首先默认所以数据流的类型是一样的
1:dataNum必须>=1
	dataNum=1   直接判断该数据是否在系统中已经存在该数据，不存在则默认添加一个新概念
	dataNum=1 

2:这个函数貌似只有输入数据，那么输出数据呢，因为暂时你先不管这个，所以这里先不输出数据
3:

	
*/
nero_s32int DataFlowProcess(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf)
{
	nero_s32int i,j,hasAddObj/*,hasNewObj*/;
	NeuronObject * tmpObi;
	NeuronObject ** objs=NULL;
	/*参数检查*/
	if (DataFlow == NULL  || dataKind ==NULL  ||  dataNum <1)
	{
		return nero_msg_ParameterError;
	}
/*	system("xdot data/wordspic.dot");*/
	(objs)=(NeuronObject **)malloc(sizeof(NeuronObject *)*dataNum);
	/*先不比对DataFlow  dataKind  dataNum*/
	/*断DataFlow中的数据是否在系统中已经存在该数据*/
	for (i=0,j=0,hasAddObj=0;i<dataNum;i++)
	{
		/*先不管有句子的情况*/
		/*通过objs[j]里面的值就可以知道有没有在网络中找到这个对象*/

/*		objs[i]*/
		tmpObi =nero_IfHasNeuronObject(DataFlow[i],dataKind[i], GodNero);
		
		#ifdef Nero_DeBuging21_12_13 
		if (tmpObi == NULL  )
		{
			printf("找不到子概念\n",i);
		}
		else 
		{
			printf("找到子概念\n",i);
		}			
		#endif
		/*如果不存在则尝试将该对象加入网络*/
		if (tmpObi == NULL  && conf->addNewObj == 1)
		{
			tmpObi=  nero_addNeroByData(DataFlow[i],dataKind[i]);
			#ifdef Nero_DeBuging21_12_13 
			if (tmpObi != NULL  )
			{
				printf("添加子概念成功\n\n",i);
			}
			else 
			{
				printf("添加子概念失败\n\n",i);
			}			
			#endif	
/*			createNeroNetDotGraphForWords(GodNero, "data/wordspic.dot");		*/
/*			system("xdot data/wordspic.dot");*/
			if (tmpObi != NULL)
			{
				hasAddObj=1;/*只要添加过新概念就设置为1*/
				objs[j]=tmpObi;
				j++;				
			}
			
		}
		else if (tmpObi != NULL )
		{
			objs[j]=tmpObi;
			j++;
		}
	}
	/*将这几个对象形成层次结构*/
	


	/*首先判断这几个对象是否已经形成了新的概念*/
	/*hasAddObj=1直接认为没有形成了新的概念*/
	/*多余的，因为createObjFromMultiples已经有这个操作了*/

	/*形成层次结构*/
	/*其实就是将这几个对象形成一个新的对象，见神经网络记录 sheet   5系统概略图*/
	if (/*hasNewObj == 0 && */conf->addLevelObj == 1)
	{
		/*这里必须说明的是，这个新生成的概念究竟是什么类型的，createObjFromMultiples内部会根据子类型自动指定*/
/*		nero_createObjFromMultiples( objs, j);*/
	}


	return nero_msg_ok;

}
































