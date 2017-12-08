



#ifndef NeuralNetwork_H
#define NeuralNetwork_H
#include "../common/type.h"
#include "../tools/readUTF8File.h"
// #include "NeuralOperating.h"
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
*/

/*系统配置结构
对一些全局配置进行统一管理

*/
 typedef struct NeroConfiguration
 {
	nero_us32int  addNewObj; /*是否在DataFlowProcess中添加在网络中的没有的数据为一个新概念，
					为1添加否则不添加,这个变量针对，做为整体输入的数据流（如一个词组），
					在网络中的不存在相应概念时是否添加概念
					*/
	nero_us32int  addLevelObj; /*是否在DataFlowProcess中形成层次结构，为1添加否则不添加
					这个变量针对，某次依次输入的的数据流（如一个句子，被分割为词组或单个
					字符时），
					在网络中的不存在由这些子概念组成的相应 高层概念 时是否添加概念
					这个选项激活时会根据当前链接的活跃情况创建新概念，否则不会添加新概念
					*/
	nero_us32int  addLevelObjAlways; /*在DataFlowProcess中总是形成层次结构，为1添加否则不添加

					这个选项激活时会不管当前链接的活跃情况和addLevelObj的值
					，总是会创建新  高层概念
					*/
	nero_us32int CreateNewBaseObjKind;/*是否创建新基类的标志，默认打开为1*/

	nero_us32int	  OutPutFlag;//for fuc  Process_ObjForecast 是否将预测列表得结果进行输出得标志，为1进行输出，默认为0
								//the result of the output is 等同于分类结果
	nero_us32int  	  neroTime;     /*系统运行时间单位，初始化为0，隔1秒钟增加1*/
	nero_us32int  	  ifReCreateLogFile;     /*系统运行时是否重新生成log文件*/
	nero_us32int	  UsedNeroNum;
	nero_us32int	  UsedSANeroNum;
	nero_us32int	  NewNeroClassID;/*创建新类时候可以使用的编号，它从
	                                #define NeuronNode_MinNewDerivativeClassId  2001
	                                开始
	                                */
	nero_us32int	  WantCreateObjKind;//在特殊情况下指定希望生成得obj得类型，危险，尽量别使用   NeuronNode_ForNone
										//必须在使用后立即回复初始值NeuronNode_ForNone
	//some flags  for  OperatFlowProcess  fuc
	nero_us32int	OperatFlowTarget;//该次数据流的目标,默认 为0 ,do nothing
						// 						数据初态		数据终态			操作中间过程		总的操作类型

						// OperatFlow_FindKind	    1   知道		知道 			求？						求？
						// OperatFlow_SetNewKind    2	知道		知道 			知道						建立新的kind
						// 					 	    3	知道		只知道数据的类型	求？						求？


 }NeroConf;
/*ActNero只是用来保存基础数据，它对应于一个单纯的神经元*/
/*
msg：
低位			高位
1-------8  9-----16 17-----24  25----32
1111 1111 1111 1111 1111 1111 1111 1111
1-16位表示该节点种类,注意，17到24位尽量别用，以免节点种类用16位不够用需要扩展

25-26
	对象所在区域为永久得NeroInPool			#define	Nero_ObjInNeroPool	00
	对象所在区域为StagingAreaNeroPool	 	#define	Nero_ObjInSAPool	01
27:当所在区域为StagingAreaNeroPool时，此位为1(Nero_TransferToNeroPool)表示该对象已经可以被转化为永久对象了
28:当所在区域为StagingAreaNeroPool时，此位为1(Nero_AlreadyTransfered)表示该对象已经被转化为永久对象了 ,可以在适当时间进行回收了

29：该标志位为1时表示该obj为操作类时，正在进行输出列表的生成(在修改obj)，一旦有其他线程进行同一操作时可以暂停
30:该标志位为1时表示该obj为操作类
31：在一些基类中，用来表示该种类别的子类的排列顺序是否固定，1表示顺序固定，0表示顺序无所谓
32位：区别一般的概念和基类，1表示基类，0表示衍生类(网络中真正的数据)
*/

 struct ActivationNeuron;
typedef struct ActivationNeuron  NeuronObject;
typedef struct ActivationNeuron ActNero;
 typedef struct NerveFiber_ NerveFiber;

 struct ActivationNeuron
{
	nero_us32int msg;/*记录该nero的种类，性质等信息*/
	nero_s32int x;/*取值范围-2147483648 ~ 2147483647       use x  to  recond  how many chind has  if  its  a  baseObj */
	nero_s32int y;	/*	it use  to  recond  how many times  this obj  has been input  recently只在临时区域中使用这个变量*/
	nero_s32int z;
	struct NerveFiber_  * inputListHead;
						/*outputListHead链表，则指向所有该类下的所以神经元*/

	struct NerveFiber_   * outputListHead;
};
/*神经纤维---用来连接各个神经元*/
/*
msg1：
低位			高位
1-------8  9-----16 17-----24  25----32
1111 1111 1111 1111 1111 1111 1111 1111 neroConf
1-8位表示该链接的强度，就是没被搜索并匹配成功到一次，加1,暂时100最大，初始化为0,对于计算机来说，250次算是很少的了,如果实在不够用，只好放在变量time里面了
9-10位表示该神经纤维的类型：
	指向该纤维所属神经元存储数据的神经元      #define	Fiber_PointToData	00
	指向该纤维所属神经元的上层概念的神经元	#define	Fiber_PointToUpperLayer	01
	指向该纤维所属神经元的下层概念的神经元 	#define	Fiber_PointToLowerLayer	10
	指向该纤维所属神经元的同层次概念的神经元	#define	Fiber_PointToSameLayer	11
	就是说如果为Fiber_PointToUpperLayer	01，那么第9位为1，第10位为0

11-12位（）：
        有且必须存在一个                         #define Fiber_PointToUniqueObj	00
	可重复，但必须存在一个	                #define	Fiber_PointToMutiObj	01
	可有可没有 	                        #define	Fiber_PointToUnnecessaryObj	10
	有且只能存在一个						
	就是说如果为Fiber_PointToMutiObj	01，那么第11位为1，第12位为0

13-14位：用来表明该纤维结构中指针obj所指向对象得所属区域

	所指向对象所在区域为永久得NeroInPool			#define	Fiber_ObjInNeroPool	00
	所指向对象所在区域为StagingAreaNeroPool	 	#define	Fiber_ObjInSAPool	01
15位：	当该fiber所有者是一个OP obj的实例时，该位为1,用来标识该fiber指向的是该op的输出对象{且，9-10位为  Fiber_PointToLowerLayer  }，普通对象为0

time：
低位			高位
1-------8  9-----16 17-----24  25----32
1111 1111 1111 1111 1111 1111 1111 1111
1-20位存储上次被访问的时间（包括修改，被成功匹配的情况），初始化为当前系统时间
		该信息会被修改的时间包括：
			1:创建时候
			2:被成功匹配的时候，相应链接被修改  Process_StrengthenLink=》gainFiberStrengthen

*/

struct NerveFiber_
{
	struct ActivationNeuron   *obj;
	struct NerveFiber_ * next;
	nero_us32int msg1;/*存储额外的信息*/
	nero_us32int time;/*有关修改时间的信息*/
};


extern NeuronObject *GodNero;
extern NeuronObject *SAGodNero;
extern NeroConf neroConf;
#define NeuronNode_ForNone   0    //当一个概念节点的类型为此时表示一个未知对象,you can not use it
#define NeuronNode_ForGodNero   1    //GodNero
#define NeuronNode_ForData   2    //表示是一个数据存储的神经元，不是一个概念
#define NeuronNode_ForUndefined   3   //表示是一个未定义类型的神经元，是一个概念,表示这个对象实际存在，但是没有一个类型去给他进行分类
// #define NeuronNode_Unassigned   4   //表示是一个未赋值的神经元，是一个概念,表示这个对象实际存在，但是dui他的数据空间进行过赋值
 
/*强调一点，人的大脑所以的输入信息都是通过各种感觉器官输入的，也就是说
任何信息在输入大脑之前一定是通过一定加工和处理的

*/
// #define NeuronNode_ForConnect   10    //当一个概念节点的类型为此时表示一个链接节点
#define NeuronNode_ForImage   50    //当一个概念节点的类型为此时表示一个图像对象
#define NeuronNode_ForLine   51    //当一个概念节点的类型为此时表示一个线条对象

// 问题来了，是不是有一个基础类，它是所有高级衍生类的基础类，就是一层层找下去的话所有类都是由一种元数据构成的
//到目前为止，NeuronNode_ForChCharacter扮演者这个角色,但是我现在无法确定，以后是否会有新的sys内部类，打破这种
//唯一性-=-------------理论上讲，应该是有一个kind来扮演这样角色的，但是,是不是可以用NeuronNode_ForChCharacter?????
//还是先别决定这么基础的问题吧，先放着
#define NeuronNode_ForChCharacter   61    //当一个概念节点的类型为此时表示一个汉字，包括中文标点符号
#define NeuronNode_ForChWord   62    //当一个概念节点的类型为此时表示一个中文词语
#define NeuronNode_ForChSentence   63    //当一个概念节点的类型为此时表示一个中文句子
/*
操作的实现形式：
		最基础的那些动作一定是链接一些实现特定功能的函数
			可以参见新基类的创建方式，建立新类，不同的动作就是一个新类别
			但是那种先天就有的操作和后天学习来的技能是需要区分的么	,很明显是需要区分的
			因为后天学习来的如果不涉及具体的操作的话，就是逻辑推理方面的东西
			而推理恐怕就是记忆而已吧，就是形成一些临时性质的记忆把
		关于操作的功能的神经元的存在形式：
			可以是指定操作的具体函数或者一个函数编号 
			 几个parameter就指定几个输入列表的节点， 给每个
			操作类给定一个名称呢 。其存在方式参见新类的方式

			这里主要是对一个操作类的参数的确定以及因该有的参数个数  持谨慎态度
			你可能定义的操作种类：								1				   2						3
				鼠标，键盘点击     --------- 				点击的种类             持续时间      				力度
				字符打印			--------- 				具体那个字符
				层次定义 									定义一个基类a是另一个基类b得上层类，that is  mean：基类b得输出列表会指向基类a
				/////////////////////////////////////////////////////////////////////////////////////
				[[[[[[[[特别的：所有操作类型得obj或者说基类都看成一个普通得obj来看，只是需要单独得函数来处理]]]]]]]]]]]]]]]]
					///////////////////////////////////////////////////////////////////////////////////////////
		这样来定义一个操作类型的nero：只要该nero  kind确定就由系统指定与一个实际完成操作的函数绑定
			 struct ActivationNeuron							基类          	 				    衍生类
			{
			nero_us32int msg;									/*记录该nero的种类，性质等信息
			nero_s32int x;      								recond  how many chindren			x指向实际执行操作的函数的地址或者ID 
			nero_s32int y;																			y该操作的持续时间,默认为0表示无该项数据
			nero_s32int z;																			z该操作的执行次数.默认为0表示无该项数据
			struct NerveFiber_  * inputListHead;				指向输出的类型		 					指向操作的数据
			struct NerveFiber_   * outputListHead; 				指向所以衍生类	
			};
		关于复杂操作类中，存在多个子操作对象时，如何处理前后两个子操作共享数据对象的情况：
			op基类的inputListHead中的对象并不一定都是基类
			op基类的inputListHead中的对象，要么有且只有多个子数据对象，要么有且只有多个子操作类对象
			假设op基类有op1,op2,俩个子操作类，它们共享数据D，把op1,op2在操作基类的定义中不指向op1,op2的基类，而是新建立一个未赋值的obj，该obj不加入sys网络中
				这样op1,op2通过指向同一个地址来表明是同一个数据， 可以在fiber中加入一个标记，表明该数据是共享的
				在op的一个实例中，在op1没有执行前，D是什么东西呢，是已经确定类型了，但没有赋值的obj,既然没有赋值，就不应该加入到sys中，而是在临时区域中
					这样的obj如果不放在临时区域中而是放在godNero中，一旦释放，就无法利用了。所以最好是动态分配
			复杂操作类并不能生成sys原本没有的子操作，它仅仅是对这些操作进行组合，如果sys足够成熟就能对所以的子操作进行自由的排列组合
			执行一个操作的本质就是执行它的所有子操作
			问题来了，对于一个op基类来说它的outputListHead是什么？显然它需要链接所有的该kind的实例（基类没有输出，也不需要输出）
					对于一个op基类的实例来说它的outputListHead链接的除了输出外和普通看没区别
					实际上，对于一个复杂衍生操作类的实例OP来说，它的输出理论上既可以保存在它自己的输出链表中，页可以保存在它的子操作的输出链表中
						区别在于？在概念上来说明显是要放在它自己的输出链表上的，但是你需要加的仅仅是一个fiber而已，因为它们是同一个输出结果
						所以，结论是：一个输出结果在OP和它的子操作上都要链接

*/
//kind down here is  some kind for operating
#define  NeuronNode_ForInputWord      100     //talk to  outside  wangts to get  words (just include one Character  )  motivated  by  sys-self
#define  NeuronNode_ForOutputWord      101     //(just include one Character  )
#define  NeuronNode_ForLayering      110   //定义一个基类a是另一个基类b得上层类，that is  mean：基类b得输出列表会指向基类a
											// inputListHead  为俩个数据，前者是基类a 得kind值(save  in x)，后者是基类b得得kind值

#define  NeuronNode_ValueCompare      120			//数据的大小比较
#define  NeuronNode_GainValue      121				//数据值加1
#define  NeuronNode_DecreaseValue      122			//数据值减1
#define  NeuronNode_FiberConnect      131			//链接的解除与增加
		 // NeuronNode_ForConnect

#define  NeuronNode_ForLoop      150		//循环类:


#define NeuronNode_ForComplexDerivative  2000    //高级衍生类			//never use it
#define NeuronNode_MinNewDerivativeClassId  2001   //you can  create it  by code.


#define NeuronNode_MaxNewDerivativeClassId  65530
#define NeuronNode_Max   65535


/*区别一般的概念和基类*/
#define NeuronNode_BaseObject   1  /*就是一般的概念和基类进行区别*/
#define NeuronNode_DerivativeObject   0  /*就是一般的概念（从基类衍生的概念）和基类进行区别*/

#define	Nero_ObjInNeroPool	0	//对象所在区域为永久得NeroInPool
#define	Nero_ObjInSAPool	1	//对象所在区域为StagingAreaNeroPool


#define	Nero_TransferToNeroPool	1	//标志临时区域对象可以转移至永久区域
#define	Nero_AlreadyTransfered	1	//标志临时区域对象already转移至永久区域






/*神经纤维类型*/
/*有一种特殊的情况，就是一些类的数据就是一些保存数据的神经元，这些神经元用纤维连接起来
组成了一个链表，这么纤维的指向类型是Fiber_PointToData*/
#define	Fiber_PointToData	0
#define	Fiber_PointToUpperLayer	1
#define	Fiber_PointToLowerLayer	2
#define	Fiber_PointToSameLayer	3


/*基类中子类的出现情况定义*/
#define Fiber_PointToUniqueObj	        0
#define	Fiber_PointToMutiObj	        1
#define	Fiber_PointToUnnecessaryObj	2
/*神经纤维链接强度的最大值*/
#define	Fiber_StrengthenMax	250     //在结构体中共用了8位来记录这个值，所以最大为255
/*你需要定义个内存池，来管理使用和未使用的神经元*/

#define	Fiber_ObjInNeroPool	0  //所指向对象所在区域为永久得NeroInPool
#define	Fiber_ObjInSAPool	1  //所指向对象所在区域为StagingAreaNeroPool


#define OperatFlow_FindKind  1		
#define OperatFlow_SetNewKind  2



// NeuronObject *  nero_IfHasObjFromMultiples5(NeuronObject *Obis[],nero_s32int objNum,struct DataFlowResultRecordInfo * listRes,nero_s32int objPos,NeuronObject  *godNero );
nero_s32int nero_ifisThisChild( nero_us32int basekind,nero_us32int  objPos , nero_us32int dataKind ,NeuronObject  *godNero );
nero_s32int  nero_getObjDataNum(ActNero * obj);
NeuronObject *  nero_getBaseObjByKind(nero_s32int kind,ActNero * godNero);
nero_s32int getFiberPointToObjNum(NerveFiber * fiber);
NeuronObject  *  nero_searchObjInOutputlistByKind(NeuronObject  * n,nero_us32int searchKind,nero_us32int ifIsBase,nero_us32int flag);

nero_s32int testBaseObjNum(NeuronObject * baseobj,NeuronObject * godNero);
nero_us32int  nero_getChildKind(NeuronObject  * n,nero_us32int positoinOfChild);
void donother();
// 将一个obj从基类得子类列表中删除
void nero_deleteObjFromBaseKindList(NeuronObject * deleteObj,NeuronObject  *godNero);
nero_s32int CreateStagingAreaNeroNet();
void setNeroTransferTag(ActNero *nero,nero_us32int tag);
nero_us32int getNeroTransferTag(ActNero *nero);
nero_us32int getFiberPointToPool(NerveFiber * fiber );

nero_s32int nero_TransferSAPoolObj(NeuronObject  *NPgodNero,NeuronObject  *SAPgodNero,NeroConf * conf,NeuronObject * needTransferNero);


nero_s32int initActNero(ActNero * nero,nero_us32int kind,NerveFiber *inputListHead,NerveFiber *outputListHead);
nero_s32int initNeroPool();
/*从神经元库存中获取一个空闲的神经元*/
NeuronObject * getNeuronObject();
/*从StagingArea神经元库存中获取一个空闲的神经元*/
NeuronObject * getSANeuronObject();
/*创建一个指定类型的神经概念*/
NeuronObject *  nero_createNeroObj(nero_s32int kind);
NeuronObject *  nero_createNeroObjSAP(nero_s32int kind,NeuronObject * godNero);
/*创建一个数据存储 神经元,并初始化*/
ActNero * nero_createDataNero();

nero_s32int getFiberType(NerveFiber * fiber);

/*下面是几个处理神经元链表的函数，，，必须快速和方便使用*/


	/*在higher下面增加一个下层概念，仅仅lower指向higher  in  outputlist*/
nero_s32int PointingToObject(NeuronObject *lower,NeuronObject *higher,nero_s32int pointTotype);
	/*在higher下面增加一个下层概念，仅仅lower指向higher  in  inputlist*/
nero_s32int PointingToObject2(NeuronObject *lower,NeuronObject *higher,nero_s32int pointTotype);

	/*根据Relationship来增加一个下层概念*/
nero_s32int addNeuronChild(NeuronObject *father,NeuronObject *child,nero_s32int Relationship);

/*给一个神经元添加一条神经纤维,type指出添加的是输出，输出纤维*/
/*返回添加的那个纤维的指针，它已经被添加到ActNero *  n中了，你需要自己继续往里面添加具体的连接对象，就是NerveFiber-》obj*/
//static inline NerveFiber * addNerveFiber(ActNero *  n,nero_s32int type);

/*将新概念加入网络*/
nero_s32int nero_addNeroIntoNet(NeuronObject *GodNero,NeuronObject *newObj);

nero_s32int nero_addNeroIntoBaseObj(NeuronObject *BaseObi,NeuronObject *newObj);
/*从俩个已知道俩个概念中生成一个新的概念，新概念的种类在函数内部自动判断，最后返回新对象指针*/
NeuronObject * nero_createObjFromPair(NeuronObject *Obi1,NeuronObject *Obj2);
/*从多个>=3已知道俩个概念中生成一个新的概念，新概念的种类在函数内部自动判断，最后返回新对象指针*/
/**/
NeuronObject * nero_createObjFromMultiples(NeuronObject *Obis[],nero_s32int objNum);
NeuronObject * nero_CreateObjInSAP(NeuronObject *Obis[],nero_s32int objNum,nero_s32int basekind,NeuronObject *godNero);
/*判断是否已经从俩个已知道俩个概念中生成一个了新的概念,有则返回这个对象*/
NeuronObject *  nero_findSameObjFromPair(NeuronObject *Obi1,NeuronObject *Obj2);

/*根据dataKind概念的种类，在增加一个特定种类的数据，在这个函数里面不需要判断是否已经存在这个概念*/
NeuronObject *  nero_addNeroByData(void *Data,nero_s32int dataKind,NeuronObject * godNero);
nero_s32int    nero_getObjKindByName(void *name,NeuronObject  * godNero);

/*增强俩个对象的链接强度，如果没有连接就添加一个*/
nero_s32int nero_StrengthenLink(NeuronObject * a,NeuronObject * b);

/*判断tmpFiber2指向的对象是否一个词的概念，并且这个词由Obis里面的字，依次组成*/
/*返回1表示是*/
nero_s32int  nero_ifHasThisData_word(NeuronObject *obj,NeuronObject *childred[],nero_s32int objNum);
/*将词的链表中的每个词加入网络*/
NeuronObject *  nero_AddWordsIntoNet2(NeuronObject *GodNero,NeuronObject  *str[400],nero_s32int strLen);
/*将词的链表中的每个词加入网络*/
nero_s32int  nero_AddWordsIntoNet(NeuronObject *GodNero,Utf8Word * wordsHead);
/*根据给定数据寻找是否网络中已经有该   字   概念了，这里只搜索一个字,找到则返回该概念的指针*/
/*kind  控制搜索的领域*/
NeuronObject * nero_IfHasZhWord(NeuronObject *GodNero,ChUTF8 * word,nero_s32int kind);

nero_s32int nero_addZhCharIntoNet(NeuronObject *GodNero,ChUTF8 chChar[],nero_s32int charCounts);
/*判断是否有这个数据相应类型的概念*/
NeuronObject *nero_IfHasNeuronObject(void *Data,nero_s32int dataKind,NeuronObject *GodNero);

nero_us32int nero_GetNeroKind(ActNero * nero);
 inline  void  nero_putDataIntoNero(ActNero *n,nero_us32int x,nero_us32int y,nero_us32int z);
#define  nero_printNeroMsg(n)   (printf("kind=%d,xyz=%x %x %x,\n",nero_GetNeroKind(n),n->x,n->y,n->z   )  )
/*判断这俩个概念是不是在网络中存在，如果不存在，0，在返回1*/
nero_s32int nero_isInNet(NeuronObject *Obi);
/*判断是不是基类*/
nero_s32int  nero_isBaseObj(NeuronObject *Obi);
/*判断该神经中的数据是否是指定的数据*/
inline nero_s32int  nero_ifHasThisData(ActNero * nero,nero_s32int x,nero_s32int y,nero_s32int z);


nero_s32int   nero_IfHasObjFromMultiples(NeuronObject *Obis[],nero_s32int objNum);
NeuronObject *   nero_IfHasObjFromMultiples2(NeuronObject *Obis[],nero_s32int objNum);
NeuronObject *   nero_IfHasObjFromMultiples4(NeuronObject *Obis[],nero_s32int objNum);

nero_s32int nero_IfIsThisKind(NeuronObject *Obis[],nero_s32int objNum,NeuronObject * baseKindObj);
NeuronObject * nero_CreateNewBaseObj(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf);
NeuronObject *  getBaseObjName(NeuronObject * baseobj,NeuronObject *);
NeuronObject * nero_CreateNewBaseObjInSAP(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf);

nero_s32int  nero_ifMakeUpWithTheseObjsInOrder(NeuronObject *obj,NeuronObject *childred[],nero_s32int objNum);
NeuronObject * nero_ModifyBaseKind(NeuronObject * objs[],nero_s32int objNum,NeuronObject  *godNero,NeroConf * conf,nero_s32int basekind);
// 加强a得 outputlist中指向得所有 属于 UpperObjKind类得实例得fiber链接强度
nero_s32int nero_StrengthenLinkWithK(NeuronObject * a,nero_s32int UpperObjKind,NeuronObject * targetObj);
void resetNeroConf();
void   testDataIn201608();


NeuronObject * nero_IfHasNeuronObjectKindUnknow(void *Data ,nero_s32int basekind,NeuronObject *godNero);
NeuronObject *  nero_createObjFromSingleObj(NeuronObject *childObi,nero_s32int upkind,NeuronObject * godNero);
nero_s32int getFiberStrengthen(NeuronObject   * low,NeuronObject  * upper);
nero_s32int nero_getObjsByStr(nero_s32int metaDataKind,void * Data ,nero_s32int hang,NeuronObject * objsStore[],NeuronObject * godNero);



NeuronObject * nero_IfHasObjFromMultiples5(NeuronObject *Obis[],nero_s32int objNum,struct DataFlowResultRecordInfo * listRes,nero_s32int objPos,NeuronObject *godNero);
void setNeroOperateFlag(ActNero *nero,nero_us32int flag);
nero_s32int getNeroOperateFlag(ActNero *nero);
NeuronObject *  nero_createNeroObjInSpecifyPool(nero_s32int kind,NeuronObject * godNero);

ActNero * nero_getOneDataNero(NeuronObject *godNero);
nero_s32int nero_addDataToZhNeroObj(NeuronObject * n,ChUTF8 *chChar,NeuronObject *godNero);
    NerveFiber * addNerveFiber(ActNero *  n,nero_s32int type,nero_s32int pointTotype);
nero_s32int nero_getOpObjDataNum(ActNero *OpBaseObj);
nero_s32int nero_checkOpObjDataSuitable(NeuronObject *OpBaseObj, NeuronObject **inputNodeObjs, nero_us32int inputNodeNum);
NeuronObject *nero_createOpByBaseKindInInSAP(nero_s32int baseKind, NeuronObject * Obis[], nero_s32int objNum,NeuronObject  *godNero);
NeuronObject *   nero_IfHasOpObjFrMulti(ActNero *Obis[], nero_s32int objNum,nero_s32int basekind ,NeuronObject  *godNero);
nero_s32int getNeroOpOpCreateOutputFlag(ActNero *nero);
void setNeroOpOpCreateOutputFlag(ActNero *nero,nero_us32int rule );
void setFiberOpOutputFlag(NerveFiber * fiber,nero_us32int rule );
void getFiberOpOutputFlag(NerveFiber * fiber  );




#endif
