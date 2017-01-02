#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iconv.h>

#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <unistd.h>    /* for lseek and write */


#include <time.h>
#include "../NeuralNetwork/NeuralNetwork.h"
#include "../NeuralNetwork/NeuralOperating.h"
#include "../tools/readUTF8File.h"
#include "Nero_IO.h"
/*#include "../common/error.h"*/
#include "../common/fileOperating.h"
#include "../common/type.h"
#include "../common/Neuron.h"
// #include "common/Neuron.h"
// #include "../MainWindow.h"
#include "neroMain.h"
/**************Log函数族**************/
BEGIN_ONE_ARG_MESSAGE_MAP(nero_msg_print_map)
    MSG_NAME(1, Log_printNeroObjMsg)
    MSG_NAME(2, Log_printNeroObjLink)
    MSG_NAME(3, Log_printNeroObjLinkTree)
    MSG_NAME(4, Log_printAllNeroMsg)
/*    MSG_NAME(4, sort_data)*/
END_ONE_ARG_MESSAGE_MAP


BEGIN_TWO_ARG_MESSAGE_MAP(nero_msg_WithStr_map)
    MSG_NAME(1, Log_printSomeMsgForObj)
    MSG_NAME(2, Log_printAllKindOf)
/*    MSG_NAME(3, del_data)*/
/*    MSG_NAME(4, sort_data)*/
END_TWO_ARG_MESSAGE_MAP



/*************IO函数族/**************/
BEGIN_ONE_ARG_MESSAGE_MAP(IO_msg_print_map)
    MSG_NAME(1, IO_GetNeroObjMsg)
    MSG_NAME(2, IO_ForOutputWord)
/*    MSG_NAME(3, Log_GetNeroObjMsg)*/
/*    MSG_NAME(4, sort_data)*/
END_ONE_ARG_MESSAGE_MAP


BEGIN_TWO_ARG_MESSAGE_MAP(IO_msg_dataIO_map)
    MSG_NAME(1, IO_InputDataToSys)
    MSG_NAME(2, IO_NeroConfigurationModify)
/*    MSG_NAME(3, del_data)*/
/*    MSG_NAME(4, sort_data)*/
END_TWO_ARG_MESSAGE_MAP




#define  LenOfstrTmp   5000



time_t   now;         //实例化time_t结构
struct  tm  *timenow;         //实例化tm结构指针

/*nero_8int  logFile[500]="../log/log.txt";*/
nero_8int  logFile[FILEPATH_MAX]="/tmp/log.txt";
nero_8int  AllKindOfFile[FILEPATH_MAX]="/tmp/AllKindOfFile.txt";
static nero_8int  file_path_getcwd[FILEPATH_MAX]="/tmp";/*保存当前目录*/
nero_8int  strTmp[LenOfstrTmp];

 extern struct    PrintNeroLInkTree_St    objTreeSt;

nero_s32int IO_NeroConfigurationModify(void * operateKind,void *c)
{
        NeroConf * tmpConf=c;
        nero_s32int kind=*((nero_s32int *)operateKind);
/*        printf("i=%d.\n",tmpConf->addLevelObjAlways);*/
        switch(kind)
        {
        
        case Conf_Modify_addLevelObjAlways:
/*                neroConf.addLevelObjAlways=tmpConf->addLevelObjAlways;*/
                break;
        
        
        
        
        default:break;
        }

}
/*这个函数用来从文件中读取输入数据，自动向系统数据数据流*/
nero_s32int IO_InputDataToSys(void * operateKind,void *dataFilePath)
{
	nero_s32int operate,isZh;
	nero_8int zhWord[20];

	nero_s32int fd;
	nero_s8int *mapped_mem, * p,* strBegin,*strEnd;
	nero_s32int flength = 1024;
	nero_us8int tmpChar;
	nero_s32int i,charNum/*strBegin*/,over;
	nero_s32int charLength=0;//该字节所占位数
	
	nero_s32int ObjRecognizeLen[DataFlowProcessObjNum_MAX];//记录识别出的字符长度
	nero_s32int ObjRecognizeCount=0;//记录已经识别出的字符数量


			int sum;
			
			void **DataFlow;
			nero_s32int *dataKind;
/*			Utf8Word  *wP;*/
			char *linc;
			nero_s32int dataNum,k,countOfWord,m;	
			struct DataFlowProcessArg arg2;			
			struct { long type; char text[100]; } mymsg;


	
	if (operateKind==NULL || dataFilePath==NULL)
	{
		return nero_msg_ParameterError;
	}
/*	printf("%s\n",dataFilePath);*/
	operate=*((nero_s32int *)operateKind);
	void * start_addr = 0;
	fd = open((char *)dataFilePath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	flength = lseek(fd, 1, SEEK_END);/*flength不包括文件的结尾符*/
//	write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
	lseek(fd, 0, SEEK_SET);
/*	write(fd, "7", 1);*/
	mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
	MAP_PRIVATE,       //不允许其它进程访问此内存区域
	fd, 0);
	p=mapped_mem;	
	strEnd=mapped_mem+flength-2;/*指向最后一个字节的位置，正常就是ox0A*/
	/*这里想系统输入信息的方式是按信息的最小单位进行，即为一个字符*/
	/*然后吧这些信息的地址以消息的形式发送给thread_for_Operating_Pic*/
	
	
	/*从文件中依次获取一个UTF8的字符，然后判断是不是汉字，是则可以传送给系统*/
	/*DataFlowProcessObjNum_MAX*/
	over=1;
	while(over != 0)
	{
		tmpChar=*p;
		charNum=0;
		
		if (p <  strEnd) 	
		{
			/*因为可能有逗号，所以可能只有1个或者俩个字节组成一个字符*/
			if (tmpChar < 128)
			{
				charNum=1;/*charNum记录utf8编码的字符数*/
			
			}else
			if (tmpChar < 194)
			{
				printf("未知的数据1  tmpChar=%d\n",(int)tmpChar);
				break;
			}else if (tmpChar < 224)
			{
				charNum=2;
			}else if (tmpChar < 240)
			{
				charNum=3;
			}else
			{
				printf("未知的数据2  tmpChar=%d\n",(int)tmpChar);
				break;
			}
/*			printf("find char %x  charNum=%d\n",tmpChar,charNum);*/
		}
		else
			over=0;
		/*将长度信息保存*/
		if (ObjRecognizeCount  < DataFlowProcessObjNum_MAX   && over !=0)
		{
			if (ObjRecognizeCount == 0)
			{
				strBegin=p;
			}		
		
			ObjRecognizeLen[ObjRecognizeCount]=charNum;//记录识别出的字符长度
			ObjRecognizeCount++;
			p=p+charNum;
		}

		if (ObjRecognizeCount == (DataFlowProcessObjNum_MAX )  ||  over == 0)
		{

			countOfWord=ObjRecognizeCount;
			
			(DataFlow)=(void **)malloc(sizeof(void *)*countOfWord);
			(dataKind)=(nero_s32int *)malloc(sizeof(nero_s32int 			   					*)*countOfWord);	
					
			for (k=0;k<countOfWord;k++)
			{
				DataFlow[k]=(void *)malloc((sizeof( char)*3));
				linc=(char *)DataFlow[k];
			
				memset(linc,0,3);
				memcpy(linc,strBegin,ObjRecognizeLen[k]);

				dataKind[k]=NeuronNode_ForChCharacter;
				strBegin+=ObjRecognizeLen[k];
/*				printf("%s",linc);*/
				
				#ifdef  Nero_DeBuging14_01_14_
				char * ttmp[10];
				memset(ttmp,0,10);
				memcpy(ttmp,DataFlow[k],3);
				printf("字符是：%s   len=%d\n",ttmp,ObjRecognizeLen[k]);
				#endif				
				
			}			
/*			printf("\n");*/
			/*现在开始准备发送消息了*/
			dataNum=countOfWord;	
			arg2.dataNum=dataNum;
			arg2.dataKind=dataKind;
			arg2.conf=&neroConf;
			arg2.DataFlow=DataFlow;
/*			printf("dataNum=%d.\n",dataNum);*/
			memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
			mymsg.type =MsgId_Nero_DataFlowProcess ;
			msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);

			/*现在*/
			
			/*测试*/
			#ifdef  Nero_DeBuging14_01_14_
			char * str=malloc(sizeof(char)*ObjRecognizeCount*4);
			sum=0;		
			for (i=0;i<ObjRecognizeCount;i++)
			{
				sum+=ObjRecognizeLen[i];
			}
			memcpy(str,strBegin,sum);
			str[sum]=0;
/*			printf("sum=%d  ObjRecognizeCount=%d over=%d\n",sum,ObjRecognizeCount,over);*/
			printf("%s\n",str);
			free(str);
			#endif
			/*结束*/
			ObjRecognizeCount=0;
		}		
		
		
/*		p++;*/
	}
	
	
/*			isZh=IO_IfIsChineseChar(zhWord)*/
	close(fd);
	munmap(mapped_mem, flength);
	
	printf("IO_InputDataToSys  end\n");
	return nero_msg_ok;	
	
}
/*打印某个类别下面的所有的衍生类*/
nero_s32int Log_printAllKindOf(void * obj_,void *str_)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind,ObjectKind2,ii,iii;
	nero_8int  strLinshi[500];
	NeuronObject * obj;
	NeuronObject * tmp;
	NeuronObject * BaseObi;
	NerveFiber  *  curFiber;
	NerveFiber  *  childcurFiber;
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (str == NULL || str_==NULL)
	{
		return nero_msg_ParameterError;
	}
	if (GodNero == NULL )
	{

			printf(" Log_printAllKindOf                      GodNero=%x\n\n\n\n\n\n",GodNero);
			exit(0);
		return nero_msg_ParameterError;
	}


		ObjectKind=*((nero_s32int *)(str_));
		curFiber=GodNero->outputListHead;
		for (;curFiber !=NULL;curFiber=curFiber->next)
		{
			BaseObi=curFiber->obj;
			ObjectKind2=nero_GetNeroKind(BaseObi);
			if (ObjectKind2 == ObjectKind)
			{
				break;
			}
		}		

		if(ObjectKind2 != ObjectKind)
		{
			sprintf(str,"Log_printAllKindOf(undefault):%s		there is  no ObjectKind=%d的对象\n",asctime(timenow),ObjectKind);
			addLineToFile(AllKindOfFile,str);
			return nero_msg_ok;			
		}

		// printf("Log_printAllKindOf  start:  Object Kind=%d,adress:%x\n",ObjectKind,BaseObi);	
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
/*			sprintf(str,"Log_printAllKindOf:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),obj,ObjectKind);*/
/*			addLineToFile(logFile,str);	*/
			break;	
		case NeuronNode_ForChCharacter:
/*		case NeuronNode_ForChWord :*/
			curFiber=BaseObi->outputListHead;
			sprintf(str,"Log_printAllKindOf(undefault):%s	add:%x	ObjectKind=%d的所有对象为：\n",asctime(timenow),BaseObi,ObjectKind);
			addLineToFile(AllKindOfFile,str);
			while(curFiber)
			{
				tmp=curFiber->obj;

					ObjectKind2=nero_GetNeroKind(tmp);
					switch(ObjectKind2)
					{
						case NeuronNode_ForChCharacter:
							IO_getZhInNero(str,tmp);
							break;
						case NeuronNode_ForChWord:
							IO_getWordsInNero(str,tmp);
							break;
						default:
					
					                str[0]=0;
							break;
					
					}
					sprintf(strLinshi,"		地址 %x  <%s>\n",(int)tmp,str);
					addLineToFile(AllKindOfFile,strLinshi);

				curFiber=curFiber->next;
			}
				
			break;
		default:
		
			// if(BaseObi->inputListHead->obj)
			// 	sprintf(str,"Log_printAllKindOf(default):%s		ObjectKind=%d ,name=%x的所有对象为(%d):\n",asctime(timenow),ObjectKind,BaseObi->inputListHead->obj,BaseObi->x);
			
			// else
			sprintf(strLinshi,"Log_printAllKindOf(default):%s		ObjectKind=%d add:%x,所有对象num为(%d):\n",asctime(timenow),ObjectKind,BaseObi,BaseObi->x);
			addLineToFile(AllKindOfFile,strLinshi);

			if(ObjectKind  >NeuronNode_ForComplexDerivative   &&    BaseObi->inputListHead->obj != NULL)
			{
				sprintf(strLinshi,"(%d%d%d)所有child对象kind为\n",BaseObi->inputListHead->obj->x,BaseObi->inputListHead->obj->y,BaseObi->inputListHead->obj->z);
				addLineToFile(AllKindOfFile,strLinshi);

			}
			// else
			// 	sprintf(strLinshi,"(NULL)所有child对象kind为\n");

			curFiber=BaseObi->inputListHead;

			if(curFiber == NULL)
			{
				sprintf(strLinshi,"  inputListHead =NULL        n");
				addLineToFile(AllKindOfFile,strLinshi);

			}
			while(curFiber)
			{
				tmp=curFiber->obj;


				sprintf(strLinshi,"          kind:%d,getFiberPointToObjNum=%d\n",nero_GetNeroKind(tmp),getFiberPointToObjNum(curFiber));
				addLineToFile(AllKindOfFile,strLinshi);
				curFiber=curFiber->next;
			}
			curFiber=BaseObi->outputListHead;
			while(curFiber)
			{
					//tmp  is the obj you wangt to search
				tmp=curFiber->obj;
				
				sprintf(strLinshi,"		地址%x,DataNUm2=%d,[",(int)tmp,nero_getObjDataNum(tmp));
				addLineToFile(AllKindOfFile,strLinshi);
				childcurFiber=tmp->inputListHead;
				iii=nero_getObjDataNum(tmp);
				ObjectKind2=nero_GetNeroKind(childcurFiber->obj);
				for(ii=0;ii <iii   &&   childcurFiber != NULL;ii++,childcurFiber=childcurFiber->next )
				{
					tmp=childcurFiber->obj;
					switch(ObjectKind2)
					{

						case NeuronNode_ForData:
							sprintf(str,"%d%d%d,",tmp->x,tmp->y,tmp->z);

							break;		
						case NeuronNode_ForChCharacter:
							IO_getZhInNero(str,tmp);

							break;
						case NeuronNode_ForChWord:
							IO_getWordsInNero(str,tmp);

							break;		
						default:	

								if( nero_GetNeroKind ( tmp->inputListHead->obj)   ==  NeuronNode_ForChCharacter )	
									IO_getZhInNero(str,tmp->inputListHead->obj);
								else 	
					                str[0]=0;
								break;
					
					}

					sprintf(strLinshi,"%s,",str);
					addLineToFile(AllKindOfFile,strLinshi);


				}
				sprintf(strLinshi,"]\n");

				addLineToFile(AllKindOfFile,strLinshi);

				curFiber=curFiber->next;
			}		
		
		
		        break;
		}

		
	// printf("Log_printAllKindOf  done\n");
	return nero_msg_ok;
}

// 打印obj 输出列表得信息
nero_s32int Log_printNeroObjLinkTree(void * arg)
{
		nero_8int  *str=strTmp;
	nero_s32int ObjectKind,ObjectKind2,ObjectKind3;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	NerveFiber  *  curFiber;
	
	nero_us32int isbaseFlag;																		


	if(obj)
	{
		curFiber = obj->inputListHead;

		sprintf(str,"Log_printNeroObjLinkTree:		Kind:%d, address:%x,data=", nero_GetNeroKind(obj),obj);
		addLineToFile(logFile,str);	
		while(curFiber != NULL  &&  curFiber->obj != NULL)
		{
			ObjectKind=nero_GetNeroKind(curFiber->obj);
			switch(ObjectKind)
			{
				case NeuronNode_ForChCharacter:
					IO_getZhInNero(strLinshi,curFiber->obj);
					sprintf(str,"%s",strLinshi);
					addLineToFile(logFile,str);	
					 break;	
				// case NeuronNode_ForChWord :
				// 	 break;		
				default:
					sprintf(str,"Log_printNeroObjLinkTree:		Kind:%d, address:%x", nero_GetNeroKind(obj),obj);
					addLineToFile(logFile,str);	
	
					 break;				
			}
			curFiber=curFiber->next;
		}
		sprintf(str,"\n	 outputList:\n" );
		addLineToFile(logFile,str);	
		// sprintf(str,"Log_printNeroObjLinkTree:		Kind:%d, address:%x\n	 outputList:\n", nero_GetNeroKind(obj),obj);
		// addLineToFile(logFile,str);	

		// 尝试打印obj本身的数据,判断数据的kind是否都一样，如果都一样且都是sys自有的kind就可以打印数据

		// switch(ObjectKind)
		// {
		// 		case NeuronNode_ForNone:					
		// 			 break;	


		// 		default:
		// 			sprintf(str,"Log_printNeroObjLinkTree:		Kind:%d, address:%x\n	 outputList:\n", nero_GetNeroKind(obj),obj);
		// 			addLineToFile(logFile,str);	
		// 			 break;	
		// }
		curFiber = obj->outputListHead;
		while(curFiber != NULL  &&  curFiber->obj != NULL)
		{

			ObjectKind=nero_GetNeroKind(curFiber->obj);
			isbaseFlag=nero_isBaseObj(curFiber->obj);


		
			switch(ObjectKind)
			{

				case NeuronNode_ForNone:			
				case NeuronNode_ForData:
				case NeuronNode_ForConnect:
				case NeuronNode_ForLine:
				case NeuronNode_ForImage:
				case NeuronNode_ForComplexDerivative:
				case NeuronNode_ForChSentence:	
					 // sprintf(str,"Log_printNeroObjLink:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),(int)obj,(int)ObjectKind);
					 // addLineToFile(logFile,str);	
					 // break;	
				case NeuronNode_ForGodNero:
				case NeuronNode_ForChCharacter:
				case NeuronNode_ForChWord :
					 break;		
				case NeuronNode_ForLayering :		
					 break;	


				default:
						//
					 sprintf(str," 				upperKind:%d,FiberStrengthen(%d),FiberPointType(%d),isbase(%d)\n",ObjectKind,(curFiber->msg1 & 0x000000ff),getFiberType(curFiber),isbaseFlag);
					 addLineToFile(logFile,str);	
					 break;				
			}


			curFiber=curFiber->next;

		}



	}

	
	return nero_msg_ok;
}
//print all  nero used  msg
nero_s32int Log_printAllNeroMsg(void * arg)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind,ObjectKind2,ObjectKind3,objnums;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	NerveFiber  *  curFiberOfbase;
	NerveFiber  *  curFiberOfObj;

	if(obj)
	{

		// ObjectKind=nero_GetNeroKind(obj);
		// if(ObjectKind == NeuronNode_ForGodNero)

	}
	else
		return  NeroError;
	
	for(curFiberOfbase=obj->outputListHead;curFiberOfbase !=NULL; curFiberOfbase=curFiberOfbase->next)
	{
		obj=curFiberOfbase->obj;
		objnums=0;
		if(obj)
		{	
			ObjectKind=nero_GetNeroKind(obj);

			for(curFiberOfObj=obj->outputListHead;curFiberOfObj !=NULL; curFiberOfObj=curFiberOfObj->next)
			{

				objnums++;
			}
			if(ObjectKind <= NeuronNode_ForComplexDerivative)
			{
				sprintf(str,"Log_printAllNeroMsg:		nerogod:%x,Kind:%d,actual num:%d ,record  num:%d\n",(NeuronObject *)arg, ObjectKind,objnums,obj->x);
				addLineToFile(logFile,str);	
				printf("Kind:%d,actual num:%d ,record  num:%d\n", ObjectKind,objnums,obj->x);
			}
			else
			{
				sprintf(str,"Log_printAllNeroMsg:		kind add=%x,Kind:%d,actual num:%d ,record  num:%d,name adress:%x  \n",curFiberOfbase->obj, ObjectKind,objnums,obj->x,obj->inputListHead->obj);
				addLineToFile(logFile,str);					
				printf("Kind:%d,actual num:%d ,record  num:%d,name adress:%x  \n", ObjectKind,objnums,obj->x,obj->inputListHead->obj);
			}
		}	
	}
}
nero_s32int Log_printNeroObjLink(void * arg)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind,ObjectKind2,ObjectKind3;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	NerveFiber  *  curFiber;
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (obj)
	{
		ObjectKind=nero_GetNeroKind(obj);
			
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
	
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
			sprintf(str,"Log_printNeroObjLink:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),(int)obj,(int)ObjectKind);
			addLineToFile(logFile,str);	
			break;	
		case NeuronNode_ForGodNero:
		case NeuronNode_ForChCharacter:
		case NeuronNode_ForChWord :
			curFiber=obj->outputListHead;
			sprintf(str,"Log_printNeroObjLink:%s		地址：%x的link对象,ObjectKind=%d\n",asctime(timenow),(int)obj,(int)ObjectKind);
			addLineToFile(logFile,str);
			
			if (curFiber == NULL)
			{
			sprintf(strLinshi,"		没有链接对象，结束link寻找\n");
			addLineToFile(logFile,strLinshi);				
			}
			while(curFiber)
			{
				tmp=curFiber->obj;
			
				if (nero_isBaseObj(tmp ) !=1)
				// if (nero_isBaseObj(tmp ) !=0  /*&&   nero_isBaseObj(tmp ) !=2 */)					
				{
					
				
					ObjectKind2=nero_GetNeroKind(tmp);
					switch(ObjectKind2)
					{
						case NeuronNode_ForChCharacter:
							IO_getZhInNero(str,tmp);
							break;
						case NeuronNode_ForChWord:
							IO_getWordsInNero(str,tmp);
							break;
							
						default:
							ObjectKind3=nero_isBaseObj(tmp);
							if (ObjectKind3 ==1)
							{
								sprintf(str,"链接对象为基类");
							}
							break;
					
					}
					sprintf(strLinshi,"		-->%x  <%s>,FiberType=%d\n",(int)tmp,str,getFiberType(curFiber));
					addLineToFile(logFile,strLinshi);
				}	
				else
				{
					sprintf(strLinshi,"		该对象%x为基类,ObjectKind=%d，无需link\n",tmp,nero_GetNeroKind(tmp));
					addLineToFile(logFile,strLinshi);
				
				}		
				curFiber=curFiber->next;
			}
				
			break;
		default:break;

		
		}

			
	}
	else
	{
		sprintf(str,"Log_printNeroObjLink:%s		空对象\n",asctime(timenow));	
		addLineToFile(logFile,str);	
	}

	return nero_msg_ok;
}





nero_s32int Log_printSomeMsgForObj(void * obj_,void *str_)
{
// nero_8int  strTmp[LenOfstrTmp];


	nero_8int  *str=strTmp;
	nero_s32int ObjectKind,linshi,tmpnum,ObjectKind2,ii;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)obj_;
	NeuronObject * tmp;
	NeuronObject * tmp2222;

	NerveFiber * childcurFiber;
nero_us32int msg;/*记录该nero的种类，性质等信息*/
nero_s32int x;/*取值范围-2147483648 ~ 2147483647*/
nero_s32int y;
nero_s32int z;
struct NerveFiber_  * inputListHead;				
struct NerveFiber_   * outputListHead; 	
	NerveFiber  *  curFiber;
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
		// printf("Local   time   is   %s/n",asctime(timenow));

	memset(str,0,LenOfstrTmp);

	if (obj &&  (nero_isBaseObj(obj ) ==0))
	{
		ObjectKind=nero_GetNeroKind(obj);
		msg=obj->msg;
		x=obj->x;
		y=obj->y;
		z=obj->z;
		inputListHead=obj->inputListHead;
		outputListHead=obj->outputListHead;
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
			if (strlen(str_) <LenOfstrTmp)
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,ObjectKind=%d,%s,%d,%d,%d,%d,%x,%x\n",asctime(timenow),(int)obj,(int)ObjectKind,(char *)str_,msg,x,y,z,inputListHead,outputListHead);	
			else
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,ObjectKind=%d,非法的打印信息\n",asctime(timenow),(int)obj,(int)ObjectKind);	
			break;	
		case NeuronNode_ForChCharacter:
				tmp=obj->inputListHead->obj;/*衍生对象的第一个数据*/
				if (strlen(str_) <LenOfstrTmp  && tmp->x !=0 && tmp->y !=0 && tmp->z !=0)
				{
					sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印字符对象(%c%c%c),%s,数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->y,(int)tmp->z,(char *)str_,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
				}
				else if (strlen(str_) <LenOfstrTmp)
				{

					if(tmp->x !=0   )
					{
						sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印字符对象(%c),数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
					}
					else if(tmp->y !=0   )
					{
						sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印字符对象(%c),数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->y,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
					}
					else if(tmp->z !=0   )
					{
						sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印字符对象(%c),数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->z,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
					}

				}
				else
					sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印字符对象《%c%c%c》,非法的打印信息\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
				break;
		case NeuronNode_ForChWord :
			linshi=IO_getWordsInNero(strLinshi,obj);
			if (strlen(str_) <LenOfstrTmp)
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印词组对象《%s》,句子长度为：%d,%s\n",asctime(timenow),(int)obj,strLinshi,linshi,(char *)str_);	
			else
				sprintf(str,"Log_printSomeMsgForObj:%s		地址：%x,打印词组对象《%s》,非法的打印信息\n",asctime(timenow),(int)obj,strLinshi);	
			break;

		default:
			linshi=0;
			tmpnum=nero_getObjDataNum(obj);

			if(nero_GetNeroKind(obj)  >  NeuronNode_ForComplexDerivative)
			{

				sprintf(strLinshi,"Log_printSomeMsgForObj:%s,ObjectKind=%d add:%x,zi对象num为(%d):\n",(char *)str_,nero_GetNeroKind(obj),obj,tmpnum);
				addLineToFile(logFile,strLinshi);

				curFiber=obj->inputListHead;
				tmp=curFiber->obj;
				sprintf(strLinshi,"		地址%x,DataNUm1=%d,[",(int)tmp,nero_getObjDataNum(tmp));
				addLineToFile(logFile,strLinshi);				
				while(curFiber)
				{
						//tmp  is the obj you wangt to search
					tmp=curFiber->obj;
					ObjectKind2=nero_GetNeroKind(tmp);

					childcurFiber=curFiber;
					// for(ii=0;ii < tmpnum &&  childcurFiber != NULL;ii++,childcurFiber=childcurFiber->next )
					{
						tmp=childcurFiber->obj;
						switch(ObjectKind2)
						{
							case NeuronNode_ForChCharacter:
								IO_getZhInNero(str,tmp);

								break;
							case NeuronNode_ForChWord:
								IO_getWordsInNero(str,tmp);

								break;		
							default:	

									if( nero_GetNeroKind ( tmp->inputListHead->obj)   ==  NeuronNode_ForChCharacter )	
										IO_getZhInNero(str,tmp->inputListHead->obj);
									else 	
						                str[0]=0;
									break;				
						}
						tmp2222=tmp->inputListHead->obj;
						sprintf(strLinshi,"%s[%x,kind=%d,%d*%d*%d]",str,tmp,nero_GetNeroKind(tmp),tmp2222->x,tmp2222->y,tmp2222->z);
						addLineToFile(logFile,strLinshi);


					}

					curFiber=curFiber->next;
				}
				sprintf(strLinshi,"]\n");
				addLineToFile(logFile,strLinshi);
			}
			break;		
		}		
	}
	else
		sprintf(str,"Log_printSomeMsgForObj:%s		空对象或者为基类对象,%s,kind=%d,%x\n",asctime(timenow),(char *)str_,nero_GetNeroKind(obj),obj);	

	addLineToFile(logFile,str);
	return nero_msg_ok;
}


//print  word  to  ...
nero_s32int IO_ForOutputWord(void * arg)
{

	nero_s32int res=0;
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	
	memset(strTmp,0,LenOfstrTmp);
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (obj)
	{
		ObjectKind=nero_GetNeroKind(obj);
		if(ObjectKind  ==  NeuronNode_ForOutputWord)
		{

			tmp=obj->inputListHead->obj;/*衍生对象的第一个数据obj*/
			tmp=tmp->inputListHead->obj;/*the data of 数据obj*/

			// printf("kind of  obj=%d\n",nero_GetNeroKind(obj));
			// printf("kind of  data=%d\n",nero_GetNeroKind(tmp));
		    // printf("地址：%x,打印字符对象(%c%c%c),数据是：《%x %x %x》\n",(int)obj,(int)tmp->x,(int)tmp->y,(int)tmp->z,(int)tmp->x,(int)tmp->y,(int)tmp->z);	

			if (tmp->x !=0 && tmp->y !=0 && tmp->z !=0)
			{
				sprintf(str,"IO_ForOutputWord:%s		地址：%x,打印字符对象(%c%c%c),数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->y,(int)tmp->z,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
			}
			else if(tmp->x !=0   )
			{
				sprintf(str,"IO_ForOutputWord1:%s		地址：%x,打印字符对象(%c),数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
			}
			else if(tmp->y !=0   )
			{
				sprintf(str,"IO_ForOutputWord2:%s		地址：%x,打印字符对象(%c),数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->y,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
			}
			else if(tmp->z !=0   )
			{
				sprintf(str,"IO_ForOutputWord3:%s		地址：%x,打印字符对象(%c),数据是：《%x%x%x》\n",asctime(timenow),(int)obj,(int)tmp->z,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
			}
			else
				sprintf(str,"IO_ForOutputWord:%s		地址：%x,打印字符对象《%c%c%c》,非法的打印信息\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->y,(int)tmp->z);	

		}

			
	}
	addLineToFile(logFile,str);
	return nero_msg_ok;
}
/*想窗口发送信息*/
nero_s32int IO_GetNeroObjMsg(void * arg)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (obj)
	{
		ObjectKind=nero_GetNeroKind(obj);
			
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),(int)obj,(int)ObjectKind);	
			break;	
		case NeuronNode_ForChCharacter:
			tmp=obj->inputListHead->obj;/*衍生对象的第一个数据*/
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,打印字符对象《%c%c%c》\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
			break;
		case NeuronNode_ForChWord :
			IO_getWordsInNero(strLinshi,obj);
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,打印词组对象《%s》\n",asctime(timenow),(int)obj,strLinshi);	
			break;
		default:break;

		
		}

			
	}
	else
		sprintf(str,"Log_printNeroObjMsg:%s		空对象\n",asctime(timenow));	

/*	addLineToFile(logFile,str);*/
	return nero_msg_ok;
}

nero_s32int Log_printNeroObjMsg(void * arg)
{
	nero_8int  *str=strTmp;
	nero_s32int ObjectKind;
	nero_8int  strLinshi[500];
	NeuronObject * obj=(NeuronObject *)arg;
	NeuronObject * tmp;
	
	time(&now);//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow   =   localtime(&now);//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
/*		printf("Local   time   is   %s/n",asctime(timenow));*/

	if (obj)
	{
		ObjectKind=nero_GetNeroKind(obj);
			
	/*	printf("str %s  and logFile=%s",str,logFile);*/
		switch(ObjectKind)
		{
		case NeuronNode_ForNone:
		case NeuronNode_ForGodNero:
		case NeuronNode_ForData:
		case NeuronNode_ForConnect:
		case NeuronNode_ForLine:
		case NeuronNode_ForImage:
		case NeuronNode_ForComplexDerivative:
		case NeuronNode_ForChSentence:	
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,ObjectKind=%d\n",asctime(timenow),(int)obj,(int)ObjectKind);	
			break;	
		case NeuronNode_ForChCharacter:
			tmp=obj->inputListHead->obj;/*衍生对象的第一个数据*/
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,打印字符对象《%c%c%c》数据：《%x %x %x》\n",asctime(timenow),(int)obj,(int)tmp->x,(int)tmp->y,(int)tmp->z,(int)tmp->x,(int)tmp->y,(int)tmp->z);	
			break;
		case NeuronNode_ForChWord :
			IO_getWordsInNero(strLinshi,obj);
			sprintf(str,"Log_printNeroObjMsg:%s		地址：%x,打印词组对象《%s》\n",asctime(timenow),(int)obj,strLinshi);	
			break;
		default:break;

		
		}

			
	}
	else
		sprintf(str,"Log_printNeroObjMsg:%s		空对象\n",asctime(timenow));	

	addLineToFile(logFile,str);
	return nero_msg_ok;
}


void *thread_for_IO_Pic(void *arg)
{
	int x=0;
	nero_s32int i = 0;
	long MsgId;
	struct { long MsgId; char text[100]; } IOMsg;
	struct  NeuronObjectMsg_  * NeroArgMsg_st;
	struct  NeuronObjectMsgWithStr_  * NeroWithStrArgMsg_st;
	struct  IODataMsg_ * DataIO_st;
	key_t ipckey;
	int IO_mq_id;

	int received;



	const nero_s32int size_message_map = 
	      sizeof(IO_msg_print_map) / sizeof(struct one_arg_message_entry );   //求得表长
	const nero_s32int size_message_map2 = 
	      sizeof(IO_msg_dataIO_map) / sizeof(struct two_arg_message_entry );   //求得表长	      
	/* Generate the ipc key */

	ipckey = ftok(IO_ipckey , IPCKEY);
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
	printf("Operating_ipckey key is %d\n", ipckey);
/*	*/
	/* Set up the message queue */
	IO_mq_id = msgget(ipckey,0);// IPC_CREAT
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
/*		msgctl(IO_mq_id,IPC_RMID,0);*/
/*	printf("IO_mq_id :    清空队列: %s\n", strerror(errno));*/
/*	IO_mq_id = msgget(ipckey,0);// IPC_CREAT*/
	printf("IO_ipckey Message identifier is %d\n", IO_mq_id);

	while(x == 0)
	{
/*		sleep(1);*/
/*		printf("wait for IO msg......\n");*/
		received = msgrcv(IO_mq_id, &IOMsg, sizeof(IOMsg), 0, MSG_NOERROR);
		if (errno != 0)
		printf("IO msgrcv strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
		if (received<1)
		{
			#ifdef Nero_DeBugInOperating_Pic
			 printf("received fail\n");
			#endif
			continue;
		}
		else
			#ifdef Nero_DeBugInOperating_Pic
			 printf("received  ok:\n");
			#endif
		MsgId=IOMsg.MsgId;
		
		switch(MsgId)
		{
			
		case MsgId_IO_ForOutputWord:

			NeroArgMsg_st=(struct  NeuronObjectMsg_  * )&IOMsg;
			
			for( i = 0; i < size_message_map; i++)
			{
			    if( IO_msg_print_map[i].id == NeroArgMsg_st->fucId )
				 (*(IO_msg_print_map[i].operate) )(NeroArgMsg_st->Obi);
			}				
			
			
			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_IO_CreateNetNet:\n");
			#endif
			break;

		case MsgId_IO_GetObjMsg:

			NeroArgMsg_st=(struct  NeuronObjectMsg_  * )&IOMsg;
			
			for( i = 0; i < size_message_map; i++)
			{
			    if( IO_msg_print_map[i].id == NeroArgMsg_st->fucId )
				 (*(IO_msg_print_map[i].operate) )(NeroArgMsg_st->Obi);
			}				
			
			
			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_IO_CreateNetNet:\n");
			#endif
			break;
			
		case MsgId_IO_dataIO:

			DataIO_st=(struct  IODataMsg_  * )&IOMsg;
			
			for( i = 0; i < size_message_map2; i++)
			{
			    if( IO_msg_dataIO_map[i].id == DataIO_st->fucId )
				 (*(IO_msg_dataIO_map[i].operate) )(&(DataIO_st->operateKind),DataIO_st->str);
			}				
			
			
			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_IO_CreateNetNet:\n");
			#endif
			break;			
	
		default:			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("MsgId_IO_NONE:  \n");
			 printf("IO msg=%s (%d)\n", IOMsg.text, received);	
			#endif	
			break;
		}
		

	}
	printf("end to wait IO msg................................\n");
}


void *thread_for_Log_Pic(void *arg)
{
	nero_s32int x=0;
	long MsgId;
	struct { long MsgId; char text[100]; } LogMsg;
	struct  NeuronObjectMsg_  * NeroArgMsg_st;
	struct  NeuronObjectMsgWithStr_  * NeroWithStrArgMsg_st;
	key_t ipckey;
	nero_s32int Log_mq_id;
	nero_s32int fucId;
	nero_s32int received;
	nero_s32int i = 0;

	const nero_s32int size_message_map = 
	      sizeof(nero_msg_print_map) / sizeof(struct one_arg_message_entry );   //求得表长
	const nero_s32int size_message_map2 = 
	      sizeof(nero_msg_WithStr_map) / sizeof(struct two_arg_message_entry );   //求得表长
	
        printf("i=%d.\n",size_message_map);
        printf("i=%d.\n",size_message_map2);
	 
	 getcwd(file_path_getcwd,FILEPATH_MAX);
	 /*设置日志目录*/
	 sprintf(logFile,"%s/log/log.txt",file_path_getcwd);
/*	 printf("file_path_getcwd: %s  and logFile=%s",file_path_getcwd,logFile);*/
	 createFile(logFile);
	 
	 sprintf(AllKindOfFile,"%s/log/AllKindOfFile.txt",file_path_getcwd);
	 createFile(AllKindOfFile);
	

	/* Generate the ipc key */

	ipckey = ftok(Log_ipckey , IPCKEY);
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
	printf("Log_ipckey key is %d\n", ipckey);
/*	*/
	/* Set up the message queue */
	Log_mq_id = msgget(ipckey,0);// IPC_CREAT
	printf("strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息
/*			msgctl(Log_mq_id,IPC_RMID,0);*/
/*	printf("Log_mq_id :    清空队列: %s\n", strerror(errno));*/
/*	Log_mq_id = msgget(ipckey,0);// IPC_CREAT*/
/*	printf("Log_ipckey Message identifier is %d\n", Log_mq_id);*/
/*	while( msgrcv(Log_mq_id, &LogMsg, sizeof(LogMsg), 0, MSG_NOERROR) >1 );*/
	while(x == 0)
	{
/*		sleep(1);*/
/*		printf("wait for Log msg......\n");*/
		received = msgrcv(Log_mq_id, &LogMsg, sizeof(LogMsg), 0, MSG_NOERROR);
		if (errno != 0)
		{
			printf(" Log msgrcv strerror: %s\n", strerror(errno)); //转换错误码为对应的错误信息			
		}

		if (received<1)
		{
			#ifdef Nero_DeBugInOperating_Pic
			 printf("received fail\n");
			#endif
			continue;
		}
		else
			#ifdef Nero_DeBugInOperating_Pic_
			 printf("thread_for_Log_Pic:received  ok:\n");
			#endif
		MsgId=LogMsg.MsgId;

		switch(MsgId)
		{
		case MsgId_Log_PrintObjMsg:
/*			printf("MsgId_Log_PrintObjMsg \n");*/
/*			fucId=(nero_s32int)  (LogMsg.text[0])*/
			NeroArgMsg_st=(struct  NeuronObjectMsg_  * )&LogMsg;
			
			for( i = 0; i < size_message_map; i++)
			{
			    if( nero_msg_print_map[i].id == NeroArgMsg_st->fucId )
				 (*(nero_msg_print_map[i].operate) )(  NeroArgMsg_st->Obi  );
			}			
			
			
			 
			#ifdef Nero_DeBugInOperating_Pic_
			 printf("thread_for_Log_Pic:MsgId_Nero_CreateNetNet\n");
			#endif
			break;
		case MsgId_Log_PrintObjMsgWithStr:
			NeroWithStrArgMsg_st=(struct  NeuronObjectMsgWithStr_  * )&LogMsg;
			
			for( i = 0; i < size_message_map2; i++)
			{
			    if( nero_msg_WithStr_map[i].id == NeroWithStrArgMsg_st->fucId )
				 (*(nero_msg_WithStr_map[i].operate) )(NeroWithStrArgMsg_st->Obi,NeroWithStrArgMsg_st->str);
			}			
			break;
		default:			
			#ifdef Nero_DeBugInOperating_Pic
			 printf("LogId_Nero_NONE:  \n");
			 printf("Log msg=%s (%d)\n", LogMsg.text, received);	
			#endif	
			break;
		}
		

	}
	printf("end to wait Log msg................................\n");
}


/*从一个数组对象中提前词组，存与str中*/
nero_s32int IO_getWordsInNero(nero_8int str[],NeuronObject * obj)
{
	nero_s32int i,p,count,charLen;
	nero_us32int c=1;	
	nero_s32int ObjectKind;
	NeuronObject * tmp;
	NeuronObject * tmpObi;
/*	NerveFiber  *  curFiber;*/
	NerveFiber  *  tmpFiber1;
	if (str == NULL || obj == NULL)
	{
		return nero_msg_ParameterError;
	}

	count=0;
	tmpObi=obj;/*每一个衍生类*/
	ObjectKind=nero_GetNeroKind(tmpObi);	


	/*只是输出这个词汇有那几个字*/
	p=0;
	if (ObjectKind == NeuronNode_ForChWord )
	{
																	
		tmpFiber1=tmpObi->inputListHead;
		c=1;
		while(tmpFiber1)
		{
				count++;
				tmp=tmpFiber1->obj->inputListHead->obj;
				if ( tmp->y == 0  && tmp->z == 0)
				{
/*					if (tmp->x == 0)*/
/*					{*/
/*						sprintf(str+p,"<0>");*/
/*					}*/
/*					else*/
						sprintf(str+p,"%c",tmp->x);
						charLen=1;
				}
				else if(tmp->y != 0  && tmp->z == 0)
				{
						sprintf(str+p,"%c%c",tmp->x,tmp->y);
						charLen=2;				
				
				}
				else
				{
				sprintf(str+p,"%c%c%c",tmp->x,tmp->y,tmp->z);
				charLen=3;
				}
					
				p+=charLen;
				
				tmpFiber1=tmpFiber1->next;
				c++;
		}
			if (c != 1)
			{
						
			}
			else
			{
				sprintf(str,"错误的词组对象");	
			}
	
	}
	else
	{
		sprintf(str,"错误的词组对象");						
	}

return count;
}

/*从一个字符对象中提取，存与str中*/
nero_s32int IO_getZhInNero(nero_8int str[],NeuronObject * obj)
{
	
	nero_s32int ObjectKind;
	NeuronObject * tmp;
	NeuronObject * tmpObi;
	NerveFiber  *  tmpFiber1;
	if (str == NULL || obj == NULL)
	{
		return nero_msg_ParameterError;
	}


	tmpObi=obj;/*每一个衍生类*/
	ObjectKind=nero_GetNeroKind(tmpObi);	

	if (ObjectKind == NeuronNode_ForChCharacter )
	{
																	
		tmpFiber1=tmpObi->inputListHead;
		tmp=tmpFiber1->obj;
		sprintf(str,"%c%c%c,<%x%x%x>",tmp->x,tmp->y,tmp->z,tmp->x,tmp->y,tmp->z);
	
	}
	else
	{
		sprintf(str,"错误的字符对象");						
	}

return nero_msg_ok;
}
/*判断该字是不是一个汉字，而不是其他字符如标点，字母等*/
int IO_IfIsChineseChar(char src[])
{
	if(src == NULL)
	{
		return 0;
	}
	char res[8];
	int is;
	int ok=UTF8ToGBK(src,res);

	if(strlen(res) >0 && ok== 1)
	{
/*		printf("%s\n", dst.c_str());*/
		is=GetToken(res);
	}	
	else
		is=0;
		
	return is;
}
//编码转换，从UTF8转换为GBK。如果原串不空，返回空，则为转换失败
int UTF8ToGBK(char src[],char res[])
{
/*	string ans;*/
	int len = strlen(src)*2+1;
	char *dst = res;
	if(dst == NULL || res == NULL)
	{
		return 0;
	}
	memset(dst, 0, len);
	char *in = src;
	char *out = dst;
	size_t len_in = strlen(src);
	size_t len_out = len;
	
	iconv_t cd = iconv_open("GBK", "UTF-8");
	if ((iconv_t)-1 == cd)
	{
/*		printf("init iconv_t failed\n"); free(dst);*/
		return ;
	}
	int n = iconv(cd, &in, &len_in, &out, &len_out);
	if(n<0)
	{
/*		printf("iconv failed\n");*/
	}
	else
	{
/*		ans = dst;*/
	}
/*      free(dst);*/
	iconv_close(cd);
	return 1;
}

//将str分隔成一个一个的字符，并判断是否是汉字，并输出编码，包括简体和繁体

/*但是这里修改成只判断str中的第一个字符，哪怕后面还有字符也不管了*/
/*返回1就表面是汉字*/
int  GetToken( char * str)
{
	
	short high, low;
	unsigned int code;
	 char   s[4];
	 int i=0;
	 if (str == NULL)
	 {
	 	return 0;
	 }
	 int len = strlen(str);
	for(; i<len; i++)
	{
		if(str[i]>=0 || i==len-1)
		{
/*			printf("%c >> no\n", str[i]);   //ASCii字符*/
			return 0;
		}
		else
		{
			//计算编码
			high = (short)(str[i] + 256);
			low = (short)(str[i+1]+256);
			code = high*256 + low;
			
			//获取字符
			s[2] = '0';
			s[0] = str[i];
			s[1] = str[i+1];
			i++;
		
/*			printf("%s >> 0x%x", s, code);*/
			if(code>=0xB0A1 && code<=0xF7FE || code>=0x8140 && code<=0xA0FE || code>=0xAA40 && code<=0xFEA0)
			{
/*				printf(" yes\n");*/
				return 	1;
			}
			else
			{
/*				printf(" no\n");*/
				return 0;
			}
		}
	}
	return 0;
}





