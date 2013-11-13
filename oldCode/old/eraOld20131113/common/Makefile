#common
DeBug = 1



obj = fileOperating
neroF=Neuron


gtkVer=`pkg-config --cflags --libs gtk+-3.0`
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc -Wall
endif


Main: $(obj).h $(obj).c  $(neroF).h $(neroF).c
	$(cc)  -c $(neroF).c  $(gtkVer)
	$(cc)  -c $(obj).c   $(gtkVer)
c :
	rm *.o
