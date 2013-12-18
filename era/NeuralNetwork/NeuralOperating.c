


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
		dataKind=NeuronNode_ForChWord   则DataFlow就是一个中文词组的utf8码，不能有符号
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
nero_s32int DataFlowProcess(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject *GodNero)
{
	nero_s32int i,j;
	NeuronObject * objs, * tmpObi;
	/*参数检查*/
	if (DataFlow == NULL  || dataKind ==NULL  ||  dataNum <1)
	{
		return nero_msg_ParameterError;
	}

	objs=(NeuronObject *)malloc(sizeof(NeuronObject *)*dataNum);
	/*先不比对DataFlow  dataKind  dataNum*/
	/*断DataFlow中的数据是否在系统中已经存在该数据*/
	for (i=0,j=0;i<dataNum;i++)
	{
		/*先不管有句子的情况*/
		/*通过objs[j]里面的值就可以知道有没有在网络中找到这个对象*/
		objs[i] =nero_IfHasNeuronObject(DataFlow[i],dataKind[i], GodNero);
/*		if (tmpObi != NULl)*/
/*		{*/
/*			objs[j]=tmpObi;*/
/*			j++;*/
/*		}*/
	}



	return nero_msg_ok;

}
































