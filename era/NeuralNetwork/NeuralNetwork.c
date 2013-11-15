


#include "NeuralNetwork.h"
/*#include "Neuron.h"*/

#include "../common/type.h"






/*ActNero只是用来保存基础数据，它对应于一个单纯的神经元*/
typedef struct ActivationNeuron
{
nero_s32int x;
nero_s32int y;
nero_s32int z;
struct ActivationNeuron   * inputListHead;
struct ActivationNeuron   * outputListHead;
}ActNero;



nero_s32int CreateActNeroNet()
{

	/*首先一个网络你是否导入了数据必须有一些基本的构建*/

	/*显然它必须生成一些基本的神经对象，就像面向对象语言中万物。之母一样*/
	


	return NeroOK;
}

nero_s32int initActNeroNet()
{







	return NeroOK;
}



