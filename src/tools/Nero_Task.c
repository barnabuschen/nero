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


#include<time.h>
#include "../NeuralNetwork/NeuralNetwork.h"
#include "../NeuralNetwork/NeuralOperating.h"
#include "../tools/readUTF8File.h"
#include "Nero_IO.h"
#include "Nero_Task.h"
/*#include "../common/error.h"*/
#include "../common/fileOperating.h"
// #include <gtk/gtk.h>
#include "../common/type.h"
#include "../common/Neuron.h"
#include "neroMain.h"






//TFF  中  order的定义
#define  Task_Order_Min     1
#define  Task_Order_CreateObjShu     100	/*数*/
#define  Task_Order_CreateObjALBS     101  /*阿拉伯数字*/
#define  Task_Order_MathNotation     102  /*数学符号*/
#define  Task_Order_CreateKindWithOneCharArg     200    /*创建一个新类，由单个字符组成，新类名称为一个字符串*/
#define  Task_Order_CreateKindWithOneCharArg2     201   /*创建一个新类，由单个字符组成，新类名称为一个字*/
#define  Task_Order_CreateOutputWordKindObj    202      /*创建NeuronNode_ForOutputWord  kind  obj*/
#define  Task_Order_CreateChWordKindObj    203          /*创建NeuronNode_ForChCharacter  kind  obj*/

#define  Task_Order_CreateKindWithMultipleWord   219    /*创建一个新类，由多个字符串组成，新类名称为一个字符串*/
#define  Task_Order_CreateKindWithEnglishWord   220     /*创建一个新类，由多个char   组成，新类名称为一个字符串*/


#define  Task_Order_DataInput    300      /* just  input some  data  into  sys,  its parameter  is just a  string */
#define  Task_Order_MutiDataInput    301      /* just  input some  data  into  sys,  its parameter is several  string */
                                                // firat string   give out  type of  all  the  data(就是给出了这个数据流表表示得对象类别)
#define  Task_Order_DataSteamInput    310      /* just  input some  data  into  sys,  its parameter  is several string */
                                                // the kind of  all  steam  in  unknow (就是mei you 给出这数据流表示得对象类别) 
                                                


#define  Task_Order_ResetConf     500  /*将conf恢复为默认配置*/

#define  Task_Order_Max     1000


#define TFFDataType_Num   	10	/*数*/
#define TFFDataType_Character   11
#define TFFDataType_String   	12
#define TFFDataType_unknow      100   //该类型为sys自动生成，不知道具体id，需要自动查询确定



#define OrderListLen		100  /*最多支持OrderListLen条命令*/
#define OrderListWigth		100	/*每条命令最多100个参数*/
#define OrderListWigthMax      70 /*每条命令最多100个参数*/




/*命令的格式：数组中每一行对应该类型各个数据的类型*/
/*每一行对于如下：*/
/*1   	2   					3  .....*/
/*命令	该命令后面的数据个数，不包括1,2  	第一个数据*/


// 这个机制无法处理参数为可变的情况,所以必须做出特别规定  for spectial case 
            //
            // 参数个数=OrderListWigthMax,mean: 参数个数 is  Variable
            // TFFDataType_unknow  mean   you need  to  get  obj kind  by  youself
nero_us32int OrderDataTypeList[OrderListLen][OrderListWigth]={
/*创建"数字"		  参数个数 新类名	     	新类的第一个数据*/
{Task_Order_CreateObjShu, 2	,  TFFDataType_Character,TFFDataType_Character},
/*创建"阿拉伯数字"	  参数个数 新类名	     	新类的第一个数据*/
{Task_Order_CreateObjALBS,2,TFFDataType_String,	TFFDataType_Character},
//~ 将conf恢复为默认配置   参数个数 新类名	     	新类的第一个数据
{Task_Order_ResetConf,   0}, 
/*创建"数学符号"	  参数个数 新类名	     	新类的第一个数据*/
{Task_Order_MathNotation,2,TFFDataType_String,	TFFDataType_Character},
/*创建"new  kind"	  参数个数 新类名	     	新类的第一个数据*/
{Task_Order_CreateKindWithOneCharArg,2,TFFDataType_String,	TFFDataType_Character},
/*创建"new  kind"	  参数个数 新类名	     	新类的第一个数据*/
{Task_Order_CreateKindWithOneCharArg2,2,TFFDataType_Character,	TFFDataType_Character},
/*创建"new  kind"                 参数个数    第一个数据*/
{Task_Order_CreateOutputWordKindObj,1,TFFDataType_Character},
/*创建"new  kind"     参数个数   第一个数据*/
{Task_Order_DataInput,1,TFFDataType_String},
/*创建"new  kind"     参数个数   第一个数据*/
{Task_Order_CreateKindWithMultipleWord,5,TFFDataType_String,TFFDataType_unknow,TFFDataType_unknow,TFFDataType_unknow,TFFDataType_unknow},
/*创建"new  kind"     参数个数   第一个数据*/
{Task_Order_CreateKindWithEnglishWord,OrderListWigthMax,TFFDataType_String,TFFDataType_Character},
/*创建"new  kind"     参数个数   第一个数据*/
{Task_Order_CreateChWordKindObj,1,TFFDataType_String},
/*创建"new  kind"     参数个数   第一个数据*/
{Task_Order_MutiDataInput,OrderListWigthMax,TFFDataType_String,TFFDataType_unknow},
/*创建"new  kind"             参数个数            新类名                 第一个数据*/
{Task_Order_DataSteamInput,OrderListWigthMax,TFFDataType_unknow,TFFDataType_unknow},
{0},
{0},
};


// nero_us32int OrderDataTypeMapping[OrderListLen][2]={



static struct  NeuronObjectMsgWithStr_    neroObjMsgWithStr_st;





static nero_8int  file_path_getcwd[FILEPATH_MAX]="/tmp";/*保存当前目录*/



// 读取英文词库，一行一个词,，并在sys中生成这些词得概念
void ReadEnglishWordsFromTxt(nero_8int  * fileNameInpt)
{

        nero_us32int strlenMax,flength,strlenMin,strLen,pos,tmpII,tmpJJ;
/*        nero_8int s[strlenMax+1];*/
/*        nero_8int * findStr;*/
    nero_8int  fileName[FILEPATH_MAX];
    nero_s32int fd;
    nero_8int *mapped_mem, * p,*end;
        nero_8int       *linStart,*linEnd;
    getcwd(file_path_getcwd,FILEPATH_MAX);

    // sprintf(fileName,"%s/data/taskFile.sh",file_path_getcwd);
    sprintf(fileName,"%s%s",file_path_getcwd,fileNameInpt);
    void * start_addr = 0;
    fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    flength = lseek(fd, 1, SEEK_END);
//  write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
    lseek(fd, 0, SEEK_SET);
/*  write(fd, "7", 1);*/
    mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
    MAP_PRIVATE,       //不允许其它进程访问此内存区域
    fd, 0);


    p=mapped_mem;
    end=p+flength-1;
/*        start=p;*/

    strlenMin=5;

    TFF tff;
    tff.order=0;
    tff.msgSeparator=' ';
    tff.orderSeparator=0x0a;/*换行符号*/
/*  tff.data=NULL;  */

    /*首先确保q不是指向分割符号*/
        while( p <= end &&  (*(p) ==tff.msgSeparator || *(p) ==tff.orderSeparator)  )
            p=p+1;

    while(1)
    {
        if( p >= end /*|| *(p) ==0x0a*/)
        {
/*          NerReportMsgError(nero_error_Id);*/
            break;
        }

        linStart=linEnd=p;
                while(*(p) !=tff.orderSeparator /*&& *(q) !=0x09*/)/*换行符或者tab*/
            p=p+1;
        linEnd=p-1;
        /*找到行末尾*/
/*      if (linEnd  > linStart )*/
        {
        memset(tff.str,0,500);
        for(tmpII=0; tmpII<TFFDataWidth; tmpII++)
            memset(tff.data[tmpII], 0, sizeof(nero_us8int ) * TFFDataLength);
        }

        memcpy(tff.str,linStart,linEnd-linStart+1);

        /*提取信息中的各个字段*/
        if( (tff.str)[0]  !=  '#')
        {
                 // printf("ReadTaskFromTxt::%s\n",tff.str);
                 getMsgInToTFF(&tff);//  the data in tff.str is  l  line  in file
/*              下面将tff中的信息转化为实际的命令*/

                // obtainOrderFromTFF(&tff);/*从TFF中分析得到命令后在函数里面直接发送就行了*/

        }

        

        /*寻找新一行的行开头*/
                while( p <= end &&  (*(p) ==tff.msgSeparator || *(p) ==tff.orderSeparator)  )
            p=p+1;
    }

    close(fd);
    munmap(mapped_mem, flength);



}








// static int nero_error_Id=0;

void ModifyBaseKindOfShu()
{
/*生成  数   概念*/
 	nero_s32int *dataKind;
        char *linc;
        nero_s32int dataNum,k,countOfWord,m,i;
        void **DataFlow;
        nero_8int baseobjName[100]="数";
        
        nero_8int nums[]={'0','1','2','3','4','5','6','7','8','9'};
        struct DataFlowProcessArg arg2;		
        struct { long type; char text[100]; } mymsg;
/*        printf("qqqqqqi=%d.\n",neroConf.CreateNewBaseObjKind);*/
        for (i=0;i<=0;i++)
        {
                
        
                countOfWord=3;
                DataFlow=(void **)malloc(sizeof(void *)*countOfWord);
                dataKind=(nero_s32int *)malloc(sizeof(nero_s32int *) * countOfWord);
                
                for (k=0;k<countOfWord;k++)
	        {

                        if (k == 0)
                        {
                                 DataFlow[k]=(void *)malloc((sizeof( char)*3));
                                linc=(char *)DataFlow[k];
                                memset(linc,0,3);
                                memcpy(linc,baseobjName,3);
                                dataKind[k]=NeuronNode_ForChCharacter;
                        }
                        else
                        {
                        
                                 DataFlow[k]=(void *)malloc((sizeof( char)*3));
                                linc=(char *)DataFlow[k];
                                memset(linc,0,3);
                                memcpy(linc,&nums[k],1);
                                dataKind[k]=NeuronNode_ForChCharacter;
                        }

	        }
                /*现在开始准备发送消息了*/
                dataNum=countOfWord;
                arg2.dataNum=dataNum;
                arg2.dataKind=dataKind;
                arg2.conf=&neroConf;
                arg2.DataFlow=DataFlow;
                
                /*必须通过发送消息来修改conf*/
        struct  IODataMsg_  DataIO_st; 
	DataIO_st.MsgId = MsgId_Nero_ConfModify;
	DataIO_st.fucId = 1;
	DataIO_st.operateKind =Conf_Modify_CreateNewBaseObjKind;
	memcpy(DataIO_st.str,&neroConf,sizeof(NeroConf));
	((NeroConf *)DataIO_st.str)->CreateNewBaseObjKind=1;
	msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);
	
	
	
/*                printf(" neroConf.CreateNewBaseObjKind=%d.\n", neroConf.CreateNewBaseObjKind);*/
                memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
                mymsg.type =MsgId_Nero_DataFlowProcess ;
                msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
        }




}
void CreateBaseKindOfShu()
{
/*生成  数   概念*/
 	nero_s32int *dataKind;
        char *linc;
        nero_s32int dataNum,k,countOfWord,m,i;
        void **DataFlow;
        nero_8int baseobjName[100]="数";
        
        nero_8int nums[]={'0','1','2','3','4','5','6','7','8','9'};
        struct DataFlowProcessArg arg2;		
        struct { long type; char text[100]; } mymsg;
/*        printf("qqqqqqi=%d.\n",neroConf.CreateNewBaseObjKind);*/
        for (i=0;i<=9;i++)
        {
                
        
                countOfWord=2;
                DataFlow=(void **)malloc(sizeof(void *)*countOfWord);
                dataKind=(nero_s32int *)malloc(sizeof(nero_s32int *) * countOfWord);
                
                for (k=0;k<countOfWord;k++)
	        {

                        if (k == 0)
                        {
                                 DataFlow[k]=(void *)malloc((sizeof( char)*3));
                                linc=(char *)DataFlow[k];
                                memset(linc,0,3);
                                memcpy(linc,baseobjName,3);
                                dataKind[k]=NeuronNode_ForChCharacter;
                        }
                        else
                        {
                        
                                 DataFlow[k]=(void *)malloc((sizeof( char)*3));
                                linc=(char *)DataFlow[k];
                                memset(linc,0,3);
                                memcpy(linc,&nums[i],1);
                                dataKind[k]=NeuronNode_ForChCharacter;
                        }

	        }
                /*现在开始准备发送消息了*/
                dataNum=countOfWord;
                arg2.dataNum=dataNum;
                arg2.dataKind=dataKind;
                arg2.conf=&neroConf;
                arg2.DataFlow=DataFlow;
                
                /*必须通过发送消息来修改conf*/
				struct  IODataMsg_  DataIO_st; 
				DataIO_st.MsgId = MsgId_Nero_ConfModify;
				DataIO_st.fucId = 1;
				DataIO_st.operateKind =Conf_Modify_CreateNewBaseObjKind;
				memcpy(DataIO_st.str,&neroConf,sizeof(NeroConf));
				((NeroConf *)DataIO_st.str)->CreateNewBaseObjKind=1;
				msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);
				
	
	
/*                printf(" neroConf.CreateNewBaseObjKind=%d.\n", neroConf.CreateNewBaseObjKind);*/
                memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
                mymsg.type =MsgId_Nero_DataFlowProcess ;
                msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
        }
}
//never  use it
void JustDoTask()
{
/*生成  数   和  阿拉伯数字 的概念*/
 	nero_s32int *dataKind;
        char *linc;
        nero_s32int dataNum,k,countOfWord,m,i;
        void **DataFlow;
        nero_8int baseobjName[100]="阿拉伯数字";
        nero_8int nums[100]={'0','1','2','3','4','5','6','7','8','9'};
        struct DataFlowProcessArg arg2;		
        struct { long type; char text[100]; } mymsg;
/*        printf("qqqqqqi=%x.\n",neroConf);*/
        for (i=0;i<=9;i++)
        {
                
        
                countOfWord=2;
                DataFlow=(void **)malloc(sizeof(void *)*countOfWord);
                dataKind=(nero_s32int *)malloc(sizeof(nero_s32int *) * countOfWord);
                
                for (k=0;k<countOfWord;k++)
	        {

                        if (k == 0)
                        {
                                DataFlow[k]=(void *)malloc( (strlen( baseobjName)) +1 );
                                 linc=(char *)DataFlow[k];
                                 memset(linc,0,(strlen( baseobjName)) +1);
                                memcpy(linc,baseobjName,(strlen( baseobjName)) +1);
                                dataKind[k]=NeuronNode_ForChWord;
/*                                strBegin+=ObjRecognizeLen[k];                                */
                        }
                        else
                        {
                        
                                 DataFlow[k]=(void *)malloc((sizeof( char)*3));
                                linc=(char *)DataFlow[k];
                                memset(linc,0,3);
                                memcpy(linc,&nums[i],1);
                                dataKind[k]=NeuronNode_ForChCharacter;
                        }

	        }
                /*现在开始准备发送消息了*/
                dataNum=countOfWord;
                arg2.dataNum=dataNum;
                arg2.dataKind=dataKind;
                arg2.conf=&neroConf;
                arg2.DataFlow=DataFlow;
                
                /*必须通过发送消息来修改conf*/
        struct  IODataMsg_  DataIO_st; 
	DataIO_st.MsgId = MsgId_Nero_ConfModify;
	DataIO_st.fucId = 1;
	DataIO_st.operateKind =Conf_Modify_CreateNewBaseObjKind;
	memcpy(DataIO_st.str,&neroConf,sizeof(NeroConf));
	((NeroConf *)DataIO_st.str)->CreateNewBaseObjKind=1;
	msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);
	
	
	
/*                printf(" neroConf.CreateNewBaseObjKind=%d.\n", neroConf.CreateNewBaseObjKind);*/
                memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
                mymsg.type =MsgId_Nero_DataFlowProcess ;
                msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
        }
}


void ReadTaskFromTxt(nero_8int  * fileNameInpt)
{

        nero_us32int strlenMax,flength,strlenMin,strLen,pos,tmpII,tmpJJ;
/*        nero_8int s[strlenMax+1];*/
/*        nero_8int * findStr;*/
    nero_8int  fileName[FILEPATH_MAX];
	nero_s32int fd;
	nero_8int *mapped_mem, * p,*end;
        nero_8int       *linStart,*linEnd;
 	getcwd(file_path_getcwd,FILEPATH_MAX);

	// sprintf(fileName,"%s/data/taskFile.sh",file_path_getcwd);
    sprintf(fileName,"%s%s",file_path_getcwd,fileNameInpt);
 	void * start_addr = 0;
	fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	flength = lseek(fd, 1, SEEK_END);
//	write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
	lseek(fd, 0, SEEK_SET);
/*	write(fd, "7", 1);*/
	mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
	MAP_PRIVATE,       //不允许其它进程访问此内存区域
	fd, 0);


	p=mapped_mem;
	end=p+flength-1;
/*        start=p;*/

	strlenMin=5;

	TFF tff;
	tff.order=0;
	tff.msgSeparator=' ';
	tff.orderSeparator=0x0a;/*换行符号*/
/*	tff.data=NULL;	*/

	/*首先确保q不是指向分割符号*/
        while( p <= end &&  (*(p) ==tff.msgSeparator || *(p) ==tff.orderSeparator)  )
			p=p+1;

	while(1)
	{
 		if( p >= end /*|| *(p) ==0x0a*/)
		{
/*			NerReportMsgError(nero_error_Id);*/
			break;
		}

		linStart=linEnd=p;
                while(*(p) !=tff.orderSeparator /*&& *(q) !=0x09*/)/*换行符或者tab*/
			p=p+1;
		linEnd=p-1;
		/*找到行末尾*/
/*		if (linEnd  > linStart )*/
		{
		        memset(tff.str,0,500);
                for(tmpII=0; tmpII<TFFDataWidth; tmpII++)
                {
                    memset(tff.data[tmpII], 0, sizeof(nero_us8int ) * TFFDataLength);
                }

		        memcpy(tff.str,linStart,linEnd-linStart+1);

                        /*提取信息中的各个字段*/
                        if( (tff.str)[0]  !=  '#')
                        {
								 // printf("ReadTaskFromTxt::%s\n",tff.str);
                                 getMsgInToTFF(&tff);//  the data in tff.str is  l  line  in file
/*				下面将tff中的信息转化为实际的命令*/
				
				                obtainOrderFromTFF(&tff);/*从TFF中分析得到命令后在函数里面直接发送就行了*/

                        }




		}
        usleep(500);

		/*寻找新一行的行开头*/
                while( p <= end &&  (*(p) ==tff.msgSeparator || *(p) ==tff.orderSeparator)  )
			p=p+1;
	}

	close(fd);
	munmap(mapped_mem, flength);



}

void obtainOrderFromTFF(TFF * tff)/*从TFF中分析得到命令后在函数里面直接发送就行了*/
{
        static nero_us8int  coutOfRun=0;
        if (tff == NULL  ||  tff->MsgCount <1  )
        {
                return ;
        }
        tff->order=atoi(  (tff->data)[0]   );
        if (tff->order <= Task_Order_Min  ||  tff->order >= Task_Order_Max  )
        {
        	printf("obtainOrderFromTFF: wrong data  tff->order=%d\n",tff->order);
                return ;
        }
 	      nero_s32int *dataKind;
        nero_us8int *linc;
        nero_s32int dataNum,k,countOfWord,m,lenOfpar,tmpCount,searchForUnknowKind,FailTosearchForUnknowKind;
        void **DataFlow;
        nero_8int baseobjName[100]="阿拉伯数字";
        struct DataFlowProcessArg arg2;		
        struct { long type; char text[100]; } mymsg;        
        
        NeuronObject  * tmpobj;

        nero_8int  kindname1[]={"整数"};
     
        
	nero_s32int i,orderPos,flag;
	/*先转化为可以发送命令的参数*/
	/*1 找到参数类型列表项*/
	orderPos=-1;
	for (i=0;i<OrderListLen;i++)
	{
/*		printf("OrderDataTypeList[%d][0]=%d.\n",i,OrderDataTypeList[i][0]);*/
		if (OrderDataTypeList[i][0] ==  tff->order)
		{
			orderPos=i;
/*			printf("orderPos=%d.\n",orderPos);*/
		}
	}
	/*命令合法性判断*/
	if (orderPos <0  ||  orderPos >=OrderListLen)
	{
		printf("obtainOrderFromTFF:  cannot find order  order=%d\n",tff->order);
		return;
	}
    if (OrderDataTypeList[orderPos][1] ==  OrderListWigthMax )
    { 
    }
    else if (OrderDataTypeList[orderPos][1] !=  (tff->MsgCount -1))
	{
		printf("obtainOrderFromTFF:  order  参数个数 error,shuld:%d,but:%d\n",OrderDataTypeList[orderPos][1],(tff->MsgCount -1));
		return;
	}
	/*现在开始转化发送命令的参数*/	
    // printf("obtainOrderFromTFF:tff->order=%d\n",tff->order);
    

    countOfWord=tff->MsgCount -1;
    if(countOfWord > 0)
    {
        DataFlow=(void **)malloc(sizeof(void *)*countOfWord);
		dataKind=(nero_s32int *)malloc(sizeof(nero_s32int *) * countOfWord);
			
	}
    // printf("obtainOrderFromTFF:  order ,num:%d,but:tff->MsgCount -1=%d\n",OrderDataTypeList[orderPos][1],(tff->MsgCount -1));


    // /一般参数个数为OrderListWigthMax，表示参数个数可变
    // OrderListWigthMax
    if(  OrderDataTypeList[orderPos][1]  ==  OrderListWigthMax)
    {

        for(tmpCount=4;tmpCount <  OrderListWigthMax ;tmpCount++)
        {

            OrderDataTypeList[orderPos][tmpCount]=OrderDataTypeList[orderPos][3];
        }

    }
    FailTosearchForUnknowKind=0;


    #ifdef Nero_DeBuging14_01_14_
        // printf  msg  by  obj
        neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
        neroObjMsgWithStr_st.fucId = 1;//打印某个具体obj得信息  Log_printSomeMsgForObj
        neroObjMsgWithStr_st.Obi = NULL;
        sprintf(neroObjMsgWithStr_st.str,"obtainOrderFromTFF 0: order=%d  countOfWord=%d",OrderDataTypeList[orderPos][0],countOfWord);
        msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);         
    #endif


    /*命令    该命令后面的数据个数，不包括1,2   第一个数据*/
    // 219 setosa 51 35 14 2
    for (k=0;k<countOfWord;k++)
   {
	    switch(OrderDataTypeList[orderPos][k+2])
	    {
		case TFFDataType_Character:
/*					printf("obtainOrderFromTFF: CreateObjShu order \n");*/
			             lenOfpar=strlen( tff->data[k+1]);
						DataFlow[k]=(void *)malloc((sizeof( char)*3));
						linc=(char *)DataFlow[k];
						memset(linc,0,3);

                        //lenOfpar=1  or 3,so in fact  you do not need to  copy 3 char,
                        //but  when you want to  use this data to create a  NeuronNode_ForChCharacter
                        //  obj  ,you  need  the len of DataFlow[k] is three  
                        //                                  DataFlow[k][0]=   tff->data[k+1]    
                        //                                  DataFlow[k][1]=   tff->data[k+2]  =0    (if  the data is a num)
                        //                                  DataFlow[k][2]=   tff->data[k+3]    =0   (if  the data is a num)                   

						memcpy(linc,&(tff->data[k+1]),3);



                        // printf("code is =%c ,%d lenOfpar=%d\n",linc,linc[0],lenOfpar);
						dataKind[k]=NeuronNode_ForChCharacter;					
			break;
		case TFFDataType_String:
/*					printf("obtainOrderFromTFF: CreateObjShu order \n");*/
			             lenOfpar=strlen( tff->data[k+1]);
						DataFlow[k]=(void *)malloc( (lenOfpar) +1 );
						 linc=(char *)DataFlow[k];
						 memset(linc,0,(lenOfpar) +1);
						memcpy(linc,tff->data[k+1],(lenOfpar) +1);
						dataKind[k]=NeuronNode_ForChWord;
			break;					
        case TFFDataType_unknow:
                //dataKind   need  to  search
                //一般参数个数为OrderListWigthMax，表示参数个数可变，而数据类型不指定时需要自己临时确认

                //  可不可以说通过描述来确定数据得类型，比如说在类型上
                // 220 setosa 51 35 14 2
                // 220 versicolor 61 29 47 14
                // 220 virginica 65 30 55 18
                // 为例：
                //     51 35 14 2 一排数据都是整数，
                //     但是你不知道整数这个类别得kind  id
                // 那么，你在类别栏里直接填写‘整数’，让sys自动查询整数得kind       
                //             searchForUnknowKind=NeuronNode_ForNone;


            lenOfpar=strlen( tff->data[k+1]);
            DataFlow[k]=(void *)malloc( (lenOfpar) +1 );
            linc=(char *)DataFlow[k];
            memset(linc,0,(lenOfpar) +1);
            memcpy(linc,tff->data[k+1],(lenOfpar) +1);

            #ifdef Nero_DeBuging14_01_14_
                // printf  msg  by  obj
                neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
                neroObjMsgWithStr_st.fucId = 1;//打印某个具体obj得信息  Log_printSomeMsgForObj
                neroObjMsgWithStr_st.Obi = NULL;
                sprintf(neroObjMsgWithStr_st.str,"obtainOrderFromTFF1:TFFDataType_unknow start find,countOfWord=%d ",countOfWord);
                msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);         
            #endif
            // dataKind[k]=NeuronNode_ForChWord;     
            // printf("obtainOrderFromTFF: begin  ---nero_getObjKindByName=%d \n",dataKind[k]);
      
            switch( tff->order)
            {

                case  Task_Order_DataSteamInput:
                    //往往是一串单纯得data stream,不知道这些数据属于什么类型的对象实例，所以只能进行简单得data类型判断
                    // 考虑到无论是什么数据类型，最基础得类型一定是char或者string
                    // 如果lenOfpar》1首先就排除了char类型

                    if(lenOfpar == 1)
                        dataKind[k] = NeuronNode_ForChCharacter;
                    else
                        dataKind[k] = NeuronNode_ForChWord;

                    break;
                case  Task_Order_CreateKindWithMultipleWord:                   
                    sleep(1);
                    dataKind[k]=nero_getObjKindByName((void *)kindname1,GodNero);    
                    // printf("obtainOrderFromTFF: nero_getObjKindByName=%d \n",dataKind[k]);
 
                    #ifdef Nero_DeBuging14_01_14_
                        // printf  msg  by  obj
                        neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
                        neroObjMsgWithStr_st.fucId = 1;//打印某个具体obj得信息  Log_printSomeMsgForObj
                        neroObjMsgWithStr_st.Obi = NULL;
                        sprintf(neroObjMsgWithStr_st.str,"obtainOrderFromTFF2: find dataKind=%d,tff->order=%d",dataKind[k],tff->order);
                        msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);         
                    #endif


                    if(dataKind[k]   <  NeuronNode_ForComplexDerivative)
                        FailTosearchForUnknowKind=1;
                                  
                    break;
                case  Task_Order_MutiDataInput:

                    // sleep(1);
                    // the num of data  is  countOfWord-2,

                    dataKind[k]=nero_getObjKindByName((void *)( tff->data[1]),GodNero);    
                    //now dataKind[k]  is the kind  of  all  the data together,but  you  just need  
                    // the kind of tff->data[k+1] 
                    if(dataKind[k]  !=  NeuronNode_ForNone) 
                    {
                        tmpobj=nero_getBaseObjByKind(dataKind[k],GodNero);
                        if(dataKind[k]  > NeuronNode_ForComplexDerivative)
                        {
                            dataKind[k]=nero_getChildKind(tmpobj,k);
                        }
                        else
                        {
                            dataKind[k]=nero_getChildKind(tmpobj,k-1);
                        }

                    }
                    // else



                    // printf("obtainOrderFromTFF: nero_getObjKindByName=%d \n",dataKind[k]);
 
                    #ifdef Nero_DeBuging14_01_14_
                        // printf  msg  by  obj
                        neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
                        neroObjMsgWithStr_st.fucId = 1;//打印某个具体obj得信息  Log_printSomeMsgForObj
                        neroObjMsgWithStr_st.Obi = NULL;
                        sprintf(neroObjMsgWithStr_st.str,"obtainOrderFromTFF22: find dataKind=%d,tff->order=%d,baseobj=%x",dataKind[k],tff->order,tmpobj);
                    msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);         
                    #endif


                    if(dataKind[k]   <  NeuronNode_ForComplexDerivative)
                        FailTosearchForUnknowKind=1;
                    break;
                default:
                     #ifdef Nero_DeBuging14_01_14_
                        // printf  msg  by  obj
                        neroObjMsgWithStr_st.MsgId = MsgId_Log_PrintObjMsgWithStr;
                        neroObjMsgWithStr_st.fucId = 1;//打印某个具体obj得信息  Log_printSomeMsgForObj
                        neroObjMsgWithStr_st.Obi = NULL;
                        sprintf(neroObjMsgWithStr_st.str,"obtainOrderFromTFF2:tff->order error,tff->order=%d",tff->order);
                        msgsnd( Log_mq_id, &neroObjMsgWithStr_st, sizeof(neroObjMsgWithStr_st), 0);         
                    #endif                
                    break;
            }
            break;                  
		default:
			printf("obtainOrderFromTFF: unknow order \n");
			break;

	    }			
   }

    if(FailTosearchForUnknowKind ==  1)
    {
        printf("obtainOrderFromTFF: 内存泄漏   tff->order=%d,dataKind[k]=%d,k=%d,countOfWord=%d \n",tff->order,dataKind[k],k,countOfWord);
        return;
    }

    /*现在开始准备发送消息了*/
    dataNum=countOfWord;
    arg2.dataNum=dataNum;
    arg2.dataKind=dataKind;
    arg2.conf=&neroConf;
    arg2.DataFlow=DataFlow;
    
        /*必须通过发送消息来修改conf*/
    struct  IODataMsg_  DataIO_st; 
    DataIO_st.MsgId = MsgId_Nero_ConfModify;
    DataIO_st.fucId = 1;
    switch( tff->order)
    {
    	case    Task_Order_CreateObjShu:
    	case    Task_Order_CreateObjALBS:
    	case    Task_Order_MathNotation:
    	case    Task_Order_CreateKindWithOneCharArg:
    	case    Task_Order_CreateKindWithOneCharArg2:
        case    Task_Order_CreateKindWithEnglishWord:
        case    Task_Order_CreateKindWithMultipleWord:
       
        
    	        DataIO_st.operateKind =Conf_Modify_CreateNewBaseObjKind;
                flag=1;
                break;			
    	case    Task_Order_ResetConf:
    	        DataIO_st.operateKind =Conf_Modify_ReSet;
                flag=1;
                break;
        case    Task_Order_CreateOutputWordKindObj:
                //it  is  different form  up cases.
                //if  you  wangt  to  created  obj  by  data,the  kind is  depending  on   tff->order  ,rather than  the  type of data
                //
                dataKind[0]=NeuronNode_ForOutputWord;
                flag=0;
    	        // DataIO_st.operateKind =Process_Create_ForOutputWord;
                break;

                //dataKind   need  to  search
        case    Task_Order_CreateChWordKindObj:
                dataKind[0]=NeuronNode_ForChWord;
                flag=0;                              
                break;
        case    Task_Order_MutiDataInput:
                dataKind[0]=NeuronNode_ForChWord;
                // ((NeroConf *)DataIO_st.str)->addLevelObjAlways=1;
                // arg2.conf->addLevelObjAlways=1;
                DataIO_st.operateKind =Conf_Modify_addLevelObjAlways;
                flag=2;
                break; 
         case    Task_Order_DataSteamInput:
                // dataKind[0]=NeuronNode_ForChWord;
                // ((NeroConf *)DataIO_st.str)->addLevelObjAlways=1;
                // arg2.conf->addLevelObjAlways=1;
                // DataIO_st.operateKind =Conf_Modify_addLevelObjAlways;
                flag=0;
                break;                
    	 default :
                DataIO_st.operateKind =Conf_Modify_ReSet; 
                flag=0;
                break;
    }
    if(flag  == 2)
    {
        memcpy(DataIO_st.str,&neroConf,sizeof(NeroConf));
        ((NeroConf *)DataIO_st.str)->addLevelObjAlways=1;
        // ((NeroConf *)DataIO_st.str)->CreateNewBaseObjKind=0;
        msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);          
    }
    if(flag  == 1)
    {
        memcpy(DataIO_st.str,&neroConf,sizeof(NeroConf));
        ((NeroConf *)DataIO_st.str)->CreateNewBaseObjKind=1;
        msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);          
    }

    // printf(" obtainOrderFromTFF:coutOfRun=%d  %s.%s\n", coutOfRun,arg2.DataFlow[0],arg2.DataFlow[1]);

     if(countOfWord > 0)
    {

        #ifdef Nero_DeBuging14_01_14_
        for(i=0;i<countOfWord;i++)
        {

            printf(" obtainOrderFromTFF:%d.%s\n",dataKind[i], DataFlow[i]);
        }
                    printf(" \n");


         
        #endif
        coutOfRun++;
		memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
		mymsg.type =MsgId_Nero_DataFlowProcess ;
		msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
    		
    }







}
void getMsgInToTFF(TFF *  tff)
{
        

        if (tff == NULL )
        {
                return ;
        }
	nero_us8int * str =tff->str; 
	nero_us8int  split[2] ;
	
        nero_s32int  strLen=strlen(tff->str);
        if (strLen < 1  ||  strLen  >= 500)
        {
                return ;
        }

        /*提取tff-<str中被tff.msgSeparator分割符分割的字条*/


	split[0]=tff->msgSeparator; 
	split[1]=0; 
	tff->MsgCount=0; 
	
	nero_us8int * p; 
	p = strsep (&str,split); 
	while(p!=NULL   &&  tff->MsgCount < TFFDataWidth   &&  strlen(p) < TFFDataLength) 
	{ 
/*		(tff->data )[tff->MsgCount][ ]*/
		memcpy((tff->data )[tff->MsgCount], p, strlen(p) );/*把每条子信息条码以字符串的形式存储下来*/
		
		
		
/*		printf ("%s\n",(tff->data )[tff->MsgCount]);  */
		tff->MsgCount=tff->MsgCount +1; 
		p = strsep(&str, split);
	} 	
	

}



//every line  in  file is a  data string ,every char in str  is  a  char  obj
void ReadTaskFromTxtByline(nero_8int  * FileName)
{

    ChUTF8      * chCharInTask;
    nero_s32int  charCountsInTask;
    nero_s32int *dataKind;
    nero_us8int *linc;
    nero_s32int dataNum,k,countOfWord,m,lenOfpar;
    void **DataFlow;
    nero_8int baseobjName[100]="阿拉伯数字";
    struct DataFlowProcessArg arg2;     
    struct { long type; char text[100]; } mymsg;  
     nero_8int  fileName[FILEPATH_MAX];

    struct adkfjao  testsadkfjao;
    nero_s32int fd;
    nero_s8int *mapped_mem, * p,* end;
    nero_s32int flength = 1024;
    nero_us8int tmp;
    nero_s32int i,objNUm,x,y,z;
    nero_s32int charLength=0;//该字节所占位数
    //存储utf编码，chChar中是以utf8编码的汉字，chCharUnicode16Code中则是对应的UnicodeCode
    charCounts=0;
    
/*  nero_us32int chCharUnicodeCode[ChineseCharNum];*/
/*  Unicode16 chCharUnicode16Code[ChineseCharNum];*/
    getcwd(file_path_getcwd,FILEPATH_MAX);
    sprintf(fileName,"%s%s",file_path_getcwd,FileName);

    void * start_addr = 0;
    fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    flength = lseek(fd, 1, SEEK_END);
//  write(fd, "7", 1); /* 在文件最后添加一个空字符，以便下面printf正常工作 */
    lseek(fd, 0, SEEK_SET);
/*  write(fd, "7", 1);*/
    mapped_mem = mmap(start_addr, flength, PROT_READ,        //允许读
    MAP_PRIVATE,       //不允许其它进程访问此内存区域
    fd, 0);
    
    

    /* 使用映射区域. 现在开始解析UTF8File*/ 
    #ifdef Nero_DeBuging10_01_14_
    printf("flength=%d.\n",flength);
    printf("FileName=%s.\n",fileName);
    printf("mapped_mem=%x.\n",mapped_mem);
    printf("start  \n");
    #endif
    p=mapped_mem;

    end=mapped_mem+flength-1;
    charCountsInTask=0;

     chCharInTask=chChar;
     objNUm=0;
    charCountsInTask=0;

    while( charCountsInTask <=  flength)
    {
        // printf("start   %d <  %d\n",charCountsInTask,flength);
        tmp=*p;
        charLength=0;
        //这里需要判断tmp的最高位是0，还是110还是1110
        
        // 一个数与   1000 0000做与运算,等于0说明最高位为0
        // 一个数与   0010 0000做与运算,等于0说明第3位为0
        

        if( *p ==13  && *(p+1) ==10)
        {
          // NerReportMsgError(nero_error_Id);
            break;
        }



        // nero_error_Id++;
        // the  highest  len of uft8  code  for chinese is 3
        if( (tmp & 0x80 ) ==0)
        {
            charLength=1;
        }
        else 
        {   if((tmp & 0x20 ) ==0)
            {
                charLength=2;
            
            }
            else
            {
                charLength=3;
            }
        }
        if( *p == 10)
        {
            charLength=0;
        }

        //
        // nero_error_Id++;
        #ifdef Nero_DeBuging2_
        printf("charLength=%d.\n",charLength);
        printf("tmp=%d.\n",tmp);
        #endif      
        
        // if(charLength >3  ||  charLength< 1 )
        // {
        //     // printf("p=%x.\n",p);
        //     // NerReportMsgError(nero_error_Id);
        //     return NeroError;
        
        // }
        chCharInTask[objNUm].first=0;//低位
        chCharInTask[objNUm].second=0;
        chCharInTask[objNUm].third=0;
        for(i=1;i<=charLength;i++)
        {
           
            switch(i)
            {
                case 1: chCharInTask[objNUm].first=*p;p++;charCountsInTask++;break;//低位
                case 2: chCharInTask[objNUm].second=*p;p++;charCountsInTask++;break;
                case 3: chCharInTask[objNUm].third=*p;p++;charCountsInTask++;break;
                case 4: chCharInTask[objNUm].fourth=*p;p++;charCountsInTask++;break;
                default:
                        // printf("p=%x.\n",p);
                        // NerReportMsgError(nero_error_Id);
                        return NeroError;
            }
            
        }

        // if(*(p) ==10)
        //     p++;
        if(charLength >0 /* &&  *(p) ==10 */)
        {

            
             #ifdef Nero_DeBuging2_
            // charCountsInTask++; 
            x=chCharInTask[objNUm].first;
            y=chCharInTask[objNUm].second;
            z=chCharInTask[objNUm].third;
            if (x !=0 && y !=0 &&  z !=0)
            {
                printf("打印字符对象1《%c%c%c》charLength=%d\n",x,y,z,charLength); 
            }
            else if(x !=0   )
            {
                printf("打印字符对象2《%c》charLength=%d\n",x,charLength); 
            }
            else if(y !=0   )
            {
                printf("打印字符对象3《%c》charLength=%d\n",y,charLength); 
            }
            else if(z !=0   )
            {
                printf("打印字符对象4《%c》charLength=%d\n",z,charLength); 
            }

            #endif  

             objNUm=objNUm+1;
        }


        if(*p != 0x0a)
        {
            // printf("*p != 0x0a\n"); 
            continue;
        }
        else
        {
            
            if(objNUm > 0)
            {
                DataFlow=(void **)malloc(sizeof(void *)*objNUm);
                dataKind=(nero_s32int *)malloc(sizeof(nero_s32int *) * objNUm);
                    
            


                for (k=0;k<objNUm;k++)
                {
                            // printf("%d:%d \n",charCounts,res);

                    DataFlow[k]=(void *)malloc((sizeof( char)*3));
                    linc=(char *)DataFlow[k];
                    memset(linc,0,3);

                    // linc[0]=chCharInTask[objNUm]
                    //len Of char =1  or 3,so in fact  you do not need to  copy 3 char,
                    //but  when you want to  use this data to create a  NeuronNode_ForChCharacter
                    //  obj  ,you  need  the len of DataFlow[k] is three  
                    //                                  DataFlow[k][0]=   tff->data[k+1]    
                    //                                  DataFlow[k][1]=   tff->data[k+2]  =0    (if  the data is a num)
                    //                                  DataFlow[k][2]=   tff->data[k+3]    =0   (if  the data is a num)                   

                    memcpy(linc,&(chCharInTask[k]),3);
                    dataKind[k]=NeuronNode_ForChCharacter;
            

                }
                if(objNUm > 0)
                {

                    // 现在开始准备发送消息了
                    arg2.dataNum=objNUm;
                    arg2.dataKind=dataKind;
                    arg2.conf=&neroConf;
                    arg2.DataFlow=DataFlow;


                
                    memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
                    mymsg.type =MsgId_Nero_DataFlowProcess ;
                    msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
                    
                }   



            }

        }
        
        //打印utf8编码数据：
        #ifdef Nero_DeBuging10_01_14_
        if(charCounts >6700)
        {
            testsadkfjao.tmp=chChar[charCounts];
            testsadkfjao.end=0;
            printf("%s\n",(nero_s8int *)&testsadkfjao);
        }
        #endif

        p++;     
        charCountsInTask++;  
        // printf("%d:%d \n",charCounts,res);
        objNUm=0;

    }
    NerOkMsg;
    close(fd);
    munmap(mapped_mem, flength);

}




















































































