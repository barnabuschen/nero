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
#define  Task_Order_CreateKindWithOneCharArg     200  /*创建一个新类，由单个字符组成，新类名称为一个字符串*/
#define  Task_Order_CreateKindWithOneCharArg2     201  /*创建一个新类，由单个字符组成，新类名称为一个字*/
#define  Task_Order_CreateOutputWordKindObj    202      /*创建NeuronNode_ForOutputWord  kind  obj*/




#define  Task_Order_ResetConf     500  /*将conf恢复为默认配置*/

#define  Task_Order_Max     1000


#define TFFDataType_Num   	10	/*数*/
#define TFFDataType_Character   11
#define TFFDataType_String   	12

#define OrderListLen		100  /*最多支持OrderListLen条命令*/
#define OrderListWigth		100	/*每条命令最多100个参数*/




/*命令的格式：数组中每一行对应该类型各个数据的类型*/
/*每一行对于如下：*/
/*1   	2   					3  .....*/
/*命令	该命令后面的数据个数，不包括1,2  	第一个数据*/

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
{0},
{0},
{0},
{0},
};




static nero_8int  file_path_getcwd[FILEPATH_MAX]="/tmp";/*保存当前目录*/



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
// void ReadTaskFromTxt()
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
								//~ printf("ReadTaskFromTxt::%s\n",tff.str);
                                 getMsgInToTFF(&tff);
/*				下面将tff中的信息转化为实际的命令*/
				
				obtainOrderFromTFF(&tff);/*从TFF中分析得到命令后在函数里面直接发送就行了*/

                        }




		}

		/*寻找新一行的行开头*/
                while( p <= end &&  (*(p) ==tff.msgSeparator || *(p) ==tff.orderSeparator)  )
			p=p+1;
	}

	close(fd);
	munmap(mapped_mem, flength);



}

void obtainOrderFromTFF(TFF * tff)/*从TFF中分析得到命令后在函数里面直接发送就行了*/
{
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
        nero_s32int dataNum,k,countOfWord,m,lenOfpar;
        void **DataFlow;
        nero_8int baseobjName[100]="阿拉伯数字";
        struct DataFlowProcessArg arg2;		
        struct { long type; char text[100]; } mymsg;        
        
        
        
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
	if (OrderDataTypeList[orderPos][1] !=  (tff->MsgCount -1))
	{
		printf("obtainOrderFromTFF:  order  参数个数 error,shuld:%d,but:%d\n",OrderDataTypeList[orderPos][1],(tff->MsgCount -1));
		return;
	}
	/*现在开始转化发送命令的参数*/	
        for (i=0;i<=0;i++)
        {
                
        
                countOfWord=tff->MsgCount -1;
                if(countOfWord > 0)
                {
	                DataFlow=(void **)malloc(sizeof(void *)*countOfWord);
					dataKind=(nero_s32int *)malloc(sizeof(nero_s32int *) * countOfWord);
      				
				}
          
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
									memcpy(linc,&(tff->data[k+1]),lenOfpar);
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
						
					default:
						printf("obtainOrderFromTFF: unknow order \n");
						break;
			
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
			switch( tff->order)
			{
				case    Task_Order_CreateObjShu:
				case    Task_Order_CreateObjALBS:
				case    Task_Order_MathNotation:
				case    Task_Order_CreateKindWithOneCharArg:
				case    Task_Order_CreateKindWithOneCharArg2:
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
				        // DataIO_st.operateKind =Process_Create_ForOutputWord;
                        break;
				 default :
                        DataIO_st.operateKind =Conf_Modify_ReSet; 
                        flag=0;
                        break;
			}

            if(flag  == 1)
            {
                memcpy(DataIO_st.str,&neroConf,sizeof(NeroConf));
                ((NeroConf *)DataIO_st.str)->CreateNewBaseObjKind=1;
                msgsnd(Operating_mq_id, &DataIO_st, sizeof(DataIO_st), 0);          
            }

		
			 if(countOfWord > 0)
			{
	/*                printf(" neroConf.CreateNewBaseObjKind=%d.\n", neroConf.CreateNewBaseObjKind);*/
					memcpy(&(mymsg.text),&arg2,sizeof(struct DataFlowProcessArg));
					mymsg.type =MsgId_Nero_DataFlowProcess ;
					msgsnd( Operating_mq_id, &mymsg, sizeof(mymsg), 0);
					
			}
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























































































