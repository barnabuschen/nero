//编程记录：3月17：findAllLine函数无法获取正确的行数，请调试找出问题所在

//文件清空
void emptyFile(char * fileName);

//
void createFile(char * fileName);

//const char *msg="here is op \n";
//在文件中搜索字符串，并返回所在行，找不到就返回负数。
int searchStrInFile(char * FileName,char * searchstr);

//将字符串a中的部分子串提取到b中的指定位置startB处（都是从0开始计数）
void putSubStrIntoOther(char * a,char *b,int startA,int startB,int len);

//将数字转化为字符串
char *myitoa(int value,char *string,int radix);


//读取文件中的第xx行，注意@@@@@@@@@@@@@@@这里的num是从一开始算@@@@@@@@@@@@@@@@@@@@@@@@@
 char * getLineInFile(char * FileName,int num);//返回获取的字符串

//找到文件中的每一行的起始位置
int * findAllLine(char * FileName,int * countline);//输出文件中每一行的位置，返回的数组中每俩个数表明
				//了某一行的起始位置，比如：  data【0】，与 data【1】
				//保存了第一行的起始位置，不包括第一行后面的换行符，显然。data【0】一般为0，除非第一行没有
				//符合条件的数据

				//countline为统计的行数
//向文件末尾插入一行
int addLineToFile(char * FileName,char * str);

//删除文件中的某一行，注意，这里的line从0开始算,并返回所删除行的起始位置
int delLine(char * FileName,int line);

//修改某一行的数据,注意，这里的line从0开始算
void updataLine(char * FileName,char * newStr,int line);

//删除数据，如果len大于point开始之后的所以字符，就从point出截断文件
int     delStrInFile(char * FileName,int len,int point);

//读取文件数据
 char * getStrInFile(char * FileName,int len,int point);//返回获取的字符串，point为起始位置,note:最后的字符串结束符是
							//认为添上去的。需要去掉
char * getStrInFile2(char * FileName,int len,int point);
//插入数据，注意这个函数的实际功能是覆盖从point开始的len个字符，而不是从中间插入
 int    putStrInFile(char * FileName,char * str,int len,int point);//在文件特定位置插入一个字符串,point is the  									//location of the str,if 
					// it is zero ,that means put the str at the start of file
void otput();

