#graph
DeBug = 1

bmp = bmpRead
search=searchLine
gtkVer=`pkg-config --cflags --libs gtk+-3.0`

fileOperating=../common/fileOperating.o
type=../common/type.h
list=../common/list.h

nero=NeuralNetwork
obj=readUTF8File
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc 
endif
#Main: $(search).o $(bmp).c  $(search).h $(bmp).h 
#	$(cc)  -c $(bmp).c  $(gtkVer)
$(nero):   $(obj).c  $(obj).h 
	$(cc)  -c  $(obj).c
c :
	rm *.o 
r :
	./$(obj)
d :
	gdb $(obj)
