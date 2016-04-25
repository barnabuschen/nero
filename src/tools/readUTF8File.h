


#ifndef READUTF8File_H
#define READUTF8File_H


#include "../common/type.h"
//#include "../NeuralNetwork/NeuralNetwork.h"
extern ChUTF8 chChar[ChineseCharNum];
extern nero_s32int charCounts;


 typedef struct adkfjao
{
ChUTF8 tmp;
nero_s32int end;

}PrintUtf8;
/*存储一个词的结构体*/
typedef struct Utf8Word_
{
	ChUTF8    *words;
	nero_s32int num;/*数组words中的字符个数，就是该词的汉字个数*/
	struct Utf8Word_ *next;
}Utf8Word;

//extern struct NeuronObject;
/*从文件中读取汉字的utf8编码，存于chChar中，charCounts是存储的个数*/
nero_s32int readUTF8FileData(nero_8int * FileName);


nero_s32int readUTF8FileForWords(nero_8int * FileName ,Utf8Word * wordsHead);


nero_s32int rprintWords(Utf8Word * wordsHead);


nero_s32int nero_log(nero_8int * FileName,nero_8int * msg);
nero_s32int nero_printNeroLink(nero_8int * FileName,void *nero);

















#endif
