#graph
DeBug = 1

bmp = bmpRead
search=searchLine
gtkVer=`pkg-config --cflags --libs gtk+-3.0`

fileOperating=../common/fileOperating.o
type=../common/type.h
list=../common/list.h

nero=NeuralNetwork

ifeq ($(DeBug),1)
cc = gcc -Wall -g
echo  "cc = gcc -Wall -g\n"
else
cc = gcc 
echo  "cc = gcc \n"
endif
#Main: $(search).o $(bmp).c  $(search).h $(bmp).h 
#	$(cc)  -c $(bmp).c  $(gtkVer)
$(nero) :$(nero).c  $(nero).h 
	$(cc)  -c  $(nero).c $(gtkVer)
c :
	rm *.o 
r :
	./$(nero)
d :
	gdb $(bmp)
