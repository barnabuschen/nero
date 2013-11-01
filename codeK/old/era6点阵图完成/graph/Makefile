#graph
DeBug = 1
obj = bmpRead
gtkVer=`pkg-config --cflags --libs gtk+-3.0`

fileOperating=../common/fileOperating.o
type=../common/type.h
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc -Wall
endif

$(obj).o : $(obj).c 
	$(cc)  -c $(obj).c  $(gtkVer)
c :
	rm *.o $(obj)
r :
	./$(obj)
d :
	gdb $(obj)
