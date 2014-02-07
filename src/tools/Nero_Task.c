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
#include <gtk/gtk.h>
#include "../MainWindow.h"
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

void ReadTaskFromTxt()
{

        nero_us32int strlenMax,flength,strlenMin,strLen,pos;
/*        nero_8int s[strlenMax+1];*/
/*        nero_8int * findStr;*/
        nero_8int  fileName[FILEPATH_MAX];
	nero_s32int fd;
	nero_8int *mapped_mem, * p,*end;
        nero_8int       *linStart,*linEnd;
 	getcwd(file_path_getcwd,FILEPATH_MAX);
	sprintf(fileName,"%s/data/taskFile.sh",file_path_getcwd);

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
		        memcpy(tff.str,linStart,linEnd-linStart+1);

                        /*题目信息中的各个字段*/
                        if( (tff.str)[0]  !=  '#')
                        {
  		                printf("%s.\n",tff.str);
                                 getMsgInToTFF(&tff);

                        }




		}

		/*寻找新一行的行开头*/
                while( p <= end &&  (*(p) ==tff.msgSeparator || *(p) ==tff.orderSeparator)  )
			p=p+1;
	}

	close(fd);
	munmap(mapped_mem, flength);



}

void getMsgInToTFF(TFF *  tff)
{

        if (tff == NULL )
        {
                return ;
        }
        nero_s32int  strLen=strlen(tff->str);
        if (strLen < 1  ||  strLen  >= 500)
        {
                return ;
        }

        /*提取tff-<str中被tff.msgSeparator分割符分割的字条*/






}























































































