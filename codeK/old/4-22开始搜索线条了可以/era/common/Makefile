#common
DeBug = 1
obj = fileOperating
gtkVer=`pkg-config --cflags --libs gtk+-3.0`
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc -Wall
endif

$(obj).o : $(obj).c
	$(cc)  -c $(obj).c   $(gtkVer)
c :
	rm *.o.o

