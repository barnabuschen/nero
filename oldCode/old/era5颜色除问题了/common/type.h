#ifndef	  FILENAME
#define   FILENAME
char * nameFile="Data/name";
char * originFile="Data/origin";
char * explanationFile="Data/explanation";
char * moreFile="Data/more";
char * configFile="Data/config";
char * titlebmp="../Data/title.bmp";	
char * titlebmp2="Data/title.png";
//char * titlebmp="~/nero/era/Data/title.bmp";			
#endif

//#include <stdint.h>

#define putNumTwoINOne(high,low,type) ( (((type)high) << (sizeof(high)*8)) +low )

#define putNumFourINOne(high2,high1,low2,low1,type) ( (((type)(unsigned char)high2) << (sizeof(high1)*24)) + \
							(((type)(unsigned char)high1) << (sizeof(high1)*16))+\
							(((type)(unsigned char)low2) << (sizeof(high1)*8))+\
										(unsigned char)low1 )	\

#define BYTEOS32 11
#ifdef BYTEOS32
#endif


#ifdef BYTEOS64
#endif
