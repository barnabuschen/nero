#graph
DeBug = 1

bmp = bmpRead
search=searchLine
gtkVer=`pkg-config --cflags --libs gtk+-3.0`

fileOperating=../common/fileOperating.o
type=../common/type.h
list=../common/list.h



ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc 
endif
Main: $(search).o $(bmp).c  $(search).h $(bmp).h
	

#$(bmp).o : $(bmp).*  
	$(cc)  -c $(bmp).c  $(gtkVer)
$(search).o :   $(search).c  $(list) $(search).h  $(type) $(fileOperating)
	$(cc)  -c  $(search).c  $(gtkVer)
c :
	rm *.o 
r :
	./$(bmp)
d :
	gdb $(bmp)
