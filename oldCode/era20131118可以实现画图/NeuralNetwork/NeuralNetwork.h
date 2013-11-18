



#ifndef NeuralNetwork_H
#define NeuralNetwork_H
#include "../common/type.h"
/*
在这个部分中，主要集中了有关神经网络部分的代码，包括网络在内存中的存放方式，
网络节点常用操作，还有网络和数据输入模块和数据输出模块的交互代码

神经网络模块：分为俩部分；
1:网络结构---就是数据保存方式
	这里提出一个神经节的概念：神经节是功能相同的神经元细胞体在中枢以外的周围部位集合而成的结节状构造。
	这里忽略“中枢以外的周围部位”，专指具有相似功能的神经细胞的集合
	
	之前给出的
	
typedef struct Neuron
{
int x;
int y;
int z;
int next;
}Neo;
	只是数据在文件中的结构，这里必须给出在内存中的结构：就是Neuron Objects。它可以是一个神经元，也可以是一组
	神经元的集合，他们共同实现一组功能或者表达一个概念。显然他们会有大量的连接，来表示他与其他概念的关系
	就是说这里弱化神经元(在内存中为ActivationNeuron)的概念，就强调一个存储在网络中的一个个对象
	
	但是如何区别神经对象和神经元他们各自的连接的含义呢
	
	：神经对象的连接是不同对象的存在的一种相互关系，或者说逻辑上的联系，而神经元之间的连接表示了数据之间的连接关系
	
	但实际上他们是同一个东西啊啊，因为一个对象也应该是用一个神经元来表示的，不然说不过去啊。

2:操作------对数据的操作方式



*/






 struct ActivationNeuron;
typedef struct ActivationNeuron  NeuronObject;
typedef struct ActivationNeuron ActNero;
 typedef struct NerveFiber_ NerveFiber;


 struct ActivationNeuron
{
nero_us32int msg;/*记录该nero的种类，性质等信息*/
nero_s32int x;/*取值范围-2147483648 ~ 2147483647*/
nero_s32int y;
nero_s32int z;
struct NerveFiber_  * inputListHead;/*其实究竟对于一个神经元来说是不是需要这个输入神经元的链表，*/
					/*好像其实不是那么必要的是吧，因为，这实际上会造成冗余数据*/
					/*那么如果该神经元是一个NeuronObject，那么inputListHead指向该*/
					/*对象的数据，如果该神经元表示一个存抽象概念，则没有数据*/
					/*如果是一个ActNero，则该神经元的功能是保存数据*/
					/*但是如果是一个NeuronObject，是指向第一个数据呢，还是*/
					/*说inputListHead指向一个数据的链表，显然指向一个数据的神经元*/
					/*然后让这个神经元指向后续的数据*/
					
					/*特别的，对于一个基类来说，它的inputListHead为NULL*/
					/*而他的outputListHead链表，则指向所有该类下的所以神经元*/
					
struct NerveFiber_   * outputListHead; 
};
/*神经纤维---用来连接各个神经元*/
  struct NerveFiber_
 {
struct ActivationNeuron   *obj;
struct NerveFiber_ * next;
 };



extern NeuronObject *GodNero;

#define NeuronNode_ForNone   0    //当一个概念节点的类型为此时表示一个未知对象
#define NeuronNode_ForGodNero   1    //GodNero
#define NeuronNode_ForData   2    //表示是一个数据存储的神经元，不是一个概念
/*强调一点，人的大脑所以的输入信息都是通过各种感觉器官输入的，也就是说
任何信息在输入大脑之前一定是通过一定加工和处理的

*/
#define NeuronNode_ForConnect   10    //当一个概念节点的类型为此时表示一个链接节点
#define NeuronNode_ForImage   50    //当一个概念节点的类型为此时表示一个图像对象
#define NeuronNode_ForLine   51    //当一个概念节点的类型为此时表示一个线条对象


#define NeuronNode_ForChCharacter   61    //当一个概念节点的类型为此时表示一个汉字
#define NeuronNode_ForChWord   62    //当一个概念节点的类型为此时表示一个中文词语
#define NeuronNode_ForChSentence   63    //当一个概念节点的类型为此时表示一个中文句子


#define NeuronNode_Max   250


/*区别一般的概念和基类*/
#define NeuronNode_BaseObject   1  /*就是一般的概念和基类进行区别*/
#define NeuronNode_DerivativeObject   0  /*就是一般的概念（从基类衍生的概念）和基类进行区别*/


/*你需要定义个内存池，来管理使用和未使用的神经元*/





void donother();



nero_s32int initActNero(ActNero * nero,nero_us32int kind,NerveFiber *inputListHead,NerveFiber *outputListHead);
nero_s32int initNeroPool();
/*从神经元库存中获取一个空闲的神经元*/
NeuronObject * getNeuronObject();
/*创建一个指定类型的神经概念*/
NeuronObject *  nero_createNeroObj(nero_s32int kind);
/*创建一个数据存储 神经元,并初始化*/
ActNero * nero_createDataNero();


/*下面是几个处理神经元链表的函数，，，必须快速和方便使用*/


	/*在higher下面增加一个下层概念，仅仅lower指向higher*/
nero_s32int PointingToObject(NeuronObject *lower,NeuronObject *higher);

	/*根据Relationship来增加一个下层概念*/
nero_s32int addNeuronChild(NeuronObject *father,NeuronObject *child,nero_s32int Relationship);

/*给一个神经元添加一条神经纤维,type指出添加的是输出，输出纤维*/
/*返回添加的那个纤维的指针，它已经被添加到ActNero *  n中了，你需要自己继续往里面添加具体的连接对象，就是NerveFiber-》obj*/
//static inline NerveFiber * addNerveFiber(ActNero *  n,nero_s32int type);

/*将新概念加入网络*/
nero_s32int nero_addNeroIntoNet(NeuronObject *GodNero,NeuronObject *newObj);




 inline  nero_us32int nero_GetNeroKind(ActNero * nero);
 inline  void  nero_putDataIntoNero(ActNero *n,nero_us32int x,nero_us32int y,nero_us32int z);



















#endif
