#ifndef NeuralOperating_H
#define NeuralOperating_H
#include "../common/type.h"
//#include "../tools/readUTF8File.h"

struct ZhCharArg
{
	ChUTF8 *chChar;
	nero_s32int charCounts;

};
struct DataFlowProcessArg
{
void **DataFlow;
nero_s32int *dataKind;
nero_s32int dataNum;
//NeuronObject  *GodNero;
NeroConf * conf;

};
struct  DataFlowDataMsg_twoNum
{
nero_us32int  kind;
nero_us32int  baseORDerivative;
} ;

struct NeroObjForecastList
{
	struct list_head p;
	NeuronObject * obj;
	nero_s32int Strengthen;//在一次预测过程中可能一个对象被多次预测,初始化为0
	                        //普通对象在FindObjInForecastList被增加，上层衍生概念
	                        //在Process_GetNewActivateForecastObj  和
	                        //Process_IfIsChildOfActivateForecastObj被增加

	nero_s32int Strengthen2;
	nero_s32int times;//在整个预测成功中，该节点存在的时间长度,初始化为0
	                  //头节点的times用来记录该列表中的节点个数，不包括头节点
	nero_s32int start;//如果该节点对象是一个被识别出来的子集衍生对象，start end 是objs中这个子集的起始位置
	nero_s32int end;

};
//Forecast  Control  struct
struct NeroObjForecastControl
{
	nero_s32int expectedKind;//外界期望输出得对象类型,如果没有要求，或者暂时不知道就设置为NeuronNode_ForUndefined
	nero_s32int baseORDerivative;//是基类还是衍生对象得实例,
								 // 1 ：基类，
								 // 0 :衍生类
								//  3  :  all kind obj is ok
	nero_s32int Refreshed;		//该信息是否在本次DataFlowProcess运行前被刷新过
								 // 1 ：has  Refreshed
								 // 0 :old msg  --------set 0  at the end of fuc DataFlowProcess

	nero_s32int  DurationTime;  //持续时间,就是指该expectedKind得有效期
								// 0: Duration all the time  until changed
								// 1: just be used  one time ,so if(Refreshed == 1  &&  DurationTime ==1),that mean ,expectedKind is Invalid 无效的

	nero_s32int  metaData;  //元数据,in sys ,all  Obj is made up of child obj,for example :
								//a obj of kind XXX  ,is made up of NeuronNode_ForChWord
								// but NeuronNode_ForChWord is made up of  NeuronNode_ForChCharacter
								//so NeuronNode_ForChCharacter is  the metaData of kind XXX
};
struct DataFlowForecastInfo
{
	NeuronObject ** objs;//实际对象指针
	nero_s32int objNum;//实际对象个数，也是objs这个数组的有效长度，数组长度必须大于objNum，不然越界
	nero_s32int objPoint;//指向一个objs中可以读取的位置,初始为0，最大值为objNum,  is the pos you can read to forecast
    struct NeroObjForecastList   headOfUpperLayer;//指向第一个预测对象
	struct NeroObjForecastList   headOfLowerLayer;//指向第一个预测对象
	struct NeroObjForecastList   headOfSameLayer;//指向第一个预测对象
	struct NeroObjForecastList   *activateForecastObj;//在headOfUpperLayer中，当前被预测的，等待
	                                                        //后续输入判断的节点，it is not a list

	struct NeroObjForecastControl  controlMsg;//该数据是由外部指定的，你要使用时必须判断是否可以读取这个数据
	NeuronObject * waitForRecognise; /*如果没有在预测列表中的数据会先放在这里，
	                                       看下次能不能被识别*/
	nero_s32int waitForRecogniseObjPos;/*waitForRecognise的位置*/

	nero_s32int start;//start end 是objs中某个子集的起始位置，用来指示该位置有衍生概念
	nero_s32int end;
	nero_s32int timeToMerage;//合并子集标志
	nero_s32int DeBugMsg;
	nero_s32int DeBugMsgTwo;
};
struct DataNode
{
	void *data;//一维指针，指向一个动态分配内存快的数据，数据快末尾一定是0，如果末尾不是0,则只能读取固定大小的内存快，这里容易产生bug啊
	nero_us32int kind;//dataKind不为0时指明了该数据的类型，
	nero_s32int dataLen;//为0时表明Data为NULL
						//不为0时表明Data不为NULL，其值为data长度，不包括末尾的0
};
//用于输入操作类对象的结构
struct OPInput
{
	// struct list_head p;//考虑到一次只处理一个cp的话不需要这个
	nero_us32int Kind;
	struct  OPInputNode * inputNodeHead;
	struct  OPInputNode * outputNodeHead;
};
struct  OPInputNode
{
	struct list_head dataP;
	// struct OPInputNode  *op;
	struct DataNode		d;
};

























// extern nero_8int DataFlowPool[DataFlowPoolListNum][DataFlowPoolStrMaxLen];

// extern nero_s32int DataFlowPoolDataKind[DataFlowPoolListNum];


void *thread_for_Operating_Pic(void *arg);
void * thread_for_Sys_Pic(void *arg);


/*这里的所有函数定位为神经系统与外界的数据沟通接口*/
/*理想的接口是你的这个机制最好符合所有的概念类型*/

/*
输入接口


输出接口
*/



nero_s32int   Process_SearchObjForStr(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,struct DataFlowForecastInfo  * forecastInfo,NeuronObject * godNero);

NeuronObject *  Process_ClassiFication(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * godNero);
nero_us32int  Process_ModifyObjsForForecastList(struct DataFlowForecastInfo  * forecastInfo,nero_us32int  checkObjPoint,nero_us32int expectedKind,NeuronObject * godNero);
nero_us32int Process_ModifyObjsForClassiFication(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * referenceObj,NeuronObject  *godNero);
/*从数据流中分离出俩个或者多个概念，并组合成一个新的概念，这个新的概念的类型由子概念决定*/
nero_s32int DataFlowProcess(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf);

NeuronObject *  Process_ObjsClassiFication(struct DataFlowForecastInfo  * forecastInfo);



nero_s32int Process_StrengthenLink(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf,NeuronObject ** alreadyTransferNero);


NeuronObject * Process_IfHasNextObjToread(struct DataFlowForecastInfo  * forecastInfo);


struct NeroObjForecastList   * Process_CompareWithForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * findObi);


struct NeroObjForecastList   *  FindObjInForecastList(struct NeroObjForecastList   *head,NeuronObject * findObi);

NeuronObject * Process_IfFindDerivativeObj(struct DataFlowForecastInfo  * forecastInfo);

nero_s32int Process_UpdataForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * newObj);

void  UpdataLastTimeINForecastList(struct DataFlowForecastInfo  * forecastInfo);

void AddNewObjToForecastList(struct DataFlowForecastInfo  * forecastInfo,NeuronObject * newObj);

void AddNodeIntoForecastList(struct list_head  * listHead,NeuronObject * Obj,nero_us32int  FiberStrengthen);

void CleanForecastList(struct DataFlowForecastInfo  * forecastInfo);
 struct NeroObjForecastList   * Process_IfHasThisObjINList(struct list_head  * listHead,NeuronObject * Obj);

void Process_ObjForecast(struct DataFlowForecastInfo  * forecastInfo);

nero_s32int Process_IfIsChildOfActivateForecastObj(struct DataFlowForecastInfo  * forecastInfo,struct NeroObjForecastList   *findObiPoint);
nero_s32int Process_GetNewActivateForecastObj(struct DataFlowForecastInfo  * forecastInfo,struct NeroObjForecastList   *findObiPoint);

 void ReSetForecastList(struct DataFlowForecastInfo  * forecastInfo);
void Process_MerageObjsList(struct DataFlowForecastInfo  * forecastInfo);
nero_s32int Operating_NeroConfigurationModify(void * operateKind,void *c);

nero_s32int  Process_IfCreateNewBaseObj(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf);

//void nero_CreateNewBaseObj(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf);

void  Process_IoFuc(struct DataFlowForecastInfo   * forecastInfo_st,  NeuronObject *  complexObj,NeuronObject  *godNero);


int qSortCmp1(const void *a,const void *b);
nero_s32int  Process_AddNewBaseKindByname(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf);
NeuronObject *   Operating_ValueCompare(NeuronObject * obj);

NeuronObject * Operating_GainValue(NeuronObject * obj,nero_s32int val);
nero_s32int nero_getOutputNodeInObj(NeuronObject **  outputNodePoint );
 // nero_s32int OperatFlowProcess(struct OPInputNode *input,NeuronObject  *godNero,NeroConf * conf);
 nero_s32int OperatFlowProcess(struct OPInput *inputSteam,NeuronObject  *godNero,NeroConf * conf);




#endif
