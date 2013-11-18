


#ifndef READUTF8File_H
#define READUTF8File_H


#include "../common/type.h"

extern ChUTF8 chChar[ChineseCharNum];
extern nero_s32int charCounts;


/*从文件中读取汉字的utf8编码，存于chChar中，charCounts是存储的个数*/
nero_s32int readUTF8FileData(nero_8int * FileName);






















#endif
