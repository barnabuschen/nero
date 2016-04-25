//#define DeBugForaddNeoByLineMan

#ifndef Neuron_H
#define Neuron_H
#include "../common/type.h"
#define NoMSG		0	//没有数据
#define NeuronNumFirst  50000 //wen新文件创建是初始Neuron个数---全部未使用
#define FirstNoUseNeuWhenCreate  20//前面二十个node不使用



#define NeuronNodeForNone   0    //当一个概念节点的类型为此时表示一个未知对象
#define NeuronNodeForImage   50    //当一个概念节点的类型为此时表示一个图像对象
#define NeuronNodeForLine   51    //当一个概念节点的类型为此时表示一个线条对象
#define NeuronNodeForConnect   10    //当一个概念节点的类型为此时表示一个链接节点


#define CurrentLevel   0     //上下层关系表示    0--同层
#define UpLevel   1     //上下层关系表示    1--上层
#define NextLevel   (-1)     //上下层关系表示    -1--下层

#define LineThickness   1     //默认的线条粗细

//char * graphNeo1="NeuronData/NeuronData1";

//对于一个存储了Neuron的文件需要有一定信息
//整个结构4*20=80个字节
typedef struct NeuronFile
{
int NeuronType;//该文件Neuron的类型
int data;// 数据的起始位置，即第一个Neuron的字节位置
int NeuronNum;//存储的Neuron数量
int FleData;//整个文件大小
int FirstNoUseNeu;//第一个没有使用的neo----从0开始算-----为了在一些时候得到方便，前面20个节点永远不使用
int flag3;
int flag4;
int flag5;
int flag6;
int flag7;
int flag8;
int flag9;
int flag10;
int flag11;
int flag12;
int flag13;
int flag14;
int flag15;
int flag16;
int flag17;
}NeoF;


typedef struct Neuron
{
int x;
int y;
int z;
int next;
}Neo;

typedef struct NeuronManage
{
char * fileName;
int fd;//文件fileName句柄
NeoF neoFNode;
Neo *  NeoData;
char *  FileData;//文件数据的起始地址

}NeuronMan;

	struct    PrintNeroLInkTree_St
	{
			nero_s32int choseType;//默认是ChoseDerivativeObj
			nero_s32int degreeOutputType;//默认是ChoseOutputAddress
			nero_s32int *  obAddress;//这个不是个问题，虽然32和64位的长度是不一样的，但是不同系统下你需要传入的数据也不一样
			nero_s32int 	 treeDepth;
			nero_s32int    treeMaxdegree;
			nero_8int    outputMsgFile[300];
			nero_8int    outputTreeFile[300];
			nero_8int    obTypeName[300];
			nero_s32int  *     obData;			//问题来了这个数据是什么数据，显然这里就是编辑框上的字符串
																					//这里有一个问题是：你要确保编辑框的编码和你从自定义文件的编码方式是一致的，不然对于汉字是有问题的
																					//另外比如你输入了125，那么你显然输入的数据是123，而不是字符串‘123’，这是有区别的
																					//所以，你还是需要把字符串改为真正在系统中被识别的二进制数据
		};
///////////////////////////////////////


//如何管理Neo,用NeuronMan来管理
//它用来抽象一块区域上的网络
//	---fileName---物理位置
//	---neoFNode---一些信息
//	--NeoData---实际的数据



/////////////////////////////////////
//它会创建一个对应的文件
int InitiaNeuronMan(char * fileName,NeuronMan * nm);
//读取数据
int ReadDataNeuronMan(char * fileName,NeuronMan * nm);
//jie解除数据
int munmapNeuronMan(NeuronMan * nm);
//打印数据
int printNeuronManData(NeuronMan * nm);
void printNeuronFileMsg(NeoF * f);

int InitiaNeuronFileNode(NeoF * f);

//新建一个文件
int  CreateNewNeuronFile(NeoF * f,char * fileName);
//
int  GetNewNeuronFileMsg(NeoF * f,char * fileName);
//
//读取一个文件信息，将文件信息保存在f中，并返回数据指针，这个就是mmap的结果
void *  ReadNeuronData(NeoF * f,char * fileName,int  *Filefd);


//
void addNeoToFile(NeoF * f,char * fileName,int  addNeoNUM);

//更新文件头
int updataNeuronFileHead(NeoF * f,char * fileName);



//////////////////////网络节点常用操作//////////////////////////////
//从一个管理项中获取n个空的节点，获取成功则返回第一个节点的位置（就是第一个的序号），失败则返回负数
//hui会更新FirstNoUseNeu的值
int GetNeurons(int n,NeuronMan * nm);

//从序号为start的节点开始找，返回第一个指向序号为mark的节点序号
//max为查找的最多的个数（防止无法退出），包括start本身
int findListEnd(int mark,int start,NeuronMan * nm,int Max);



///////////////////////////////////////////////////////////////////////////////////////////////




/////////////////创建图像对象//////////////////////

int addNeoByLineMan(NeuronMan * nm,LineMan *manAllLine);
//在网络中创建一个图像对象，赋值后返回该对象的节点，初始状态下该对象下面没有线条
int CreateImageNode(NeuronMan * nm);
//在网络中创建一个线条对象，赋值后返回该对象的节点，初始状态下该对象下面没有点集合
int CreateListNode(NeuronMan * nm);
//用一个PosList给line复值
int FillListByPosList(int line,PosList * head,NeuronMan * nm);
//g给line（末尾）增加一个点
int addPosToLine(int line,PosList * posnode,NeuronMan * nm);

//将线条加入图像概念
int addLineToImage(int line,int image,NeuronMan * nm);


//addNeoByLineMan的反过程，但是仍然保留向量的数据形式
int GetLineManByNeo(int imageIndex,NeuronMan * nm,LineMan *manAllLine);
//找寻一个NeuronMan中的前面10个图像对象的索引，index因该是一个大于11的int 数组
//它的第一个位置将会保存实际找到的对象数量，而后面的10个位置保存相应的索引号
int findImageNode(NeuronMan * nm,int *index);

////////////////////////////////////////











#endif //Neuron_H














