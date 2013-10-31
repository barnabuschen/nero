#opencv
DeBug = 1

#obj = contourFind
obj =candy2
gtkVer=`pkg-config --cflags --libs gtk+-3.0`
opencvKer=`pkg-config opencv --libs --cflags opencv`
subCommon=./common
subGraph=./graph

MAKE=make
RUN=make r
DEBUG=make d
CLEAN=make c
type=common/type.h
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc 
endif

fileOperating=../common/fileOperating.o
type=../common/type.h
list=../common/list.h

#-lpthread
$(obj) :$(obj).c $(obj).h
	$(cc)   $(obj).c  $(fileOperating)  -o  $(obj)  $(opencvKer)
	sudo ./candy2
c :
	rm $(obj) *.o
r :
	sudo ./$(obj)
d :
	gdb $(obj)










