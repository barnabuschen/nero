#ifndef NeuralOperating_H
#define NeuralOperating_H
#include "../common/type.h"
//#include "../tools/readUTF8File.h"






/*这里的所有函数定位为神经系统与外界的数据沟通接口*/
/*理想的接口是你的这个机制最好符合所有的概念类型*/

/*
输入接口


输出接口
*/


/*从数据流中分离出俩个或者多个概念，并组合成一个新的概念，这个新的概念的类型由子概念决定*/
nero_s32int DataFlowProcess(void *DataFlow[],nero_s32int dataKind[],nero_s32int dataNum,NeuronObject  *GodNero,NeroConf * conf);




nero_s32int Process_StrengthenLink(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf);


#endif
