#graph
DeBug = 1
gtkVer=`pkg-config --cflags --libs gtk+-3.0`
fileOperating=../common/fileOperating
type=../common/type.h
list=../common/list.h

#subNeuralNetwork=../NeuralNetwork

nero=../NeuralNetwork/NeuralNetwork.h
io=Nero_IO
obj=readUTF8File
dot=createDot
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc 
endif
#Main: $(search).o $(bmp).c  $(search).h $(bmp).h 
#	$(cc)  -c $(bmp).c  $(gtkVer)
$(nero):   $(obj).c  $(obj).h $(dot).c  $(dot).h   $(nero)   $(io).h  $(io).c 
	$(cc)  -c  $(obj).c
	$(cc)   -c  $(dot).c
	$(cc)  -c  $(io).c 
c :
	rm *.o 
r :
	./$(obj)
d :
	gdb $(obj)
