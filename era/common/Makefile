#common
DeBug = 0



obj = fileOperating
neroF=Neuron


gtkVer=`pkg-config --cflags --libs gtk+-3.0`
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc 
endif


Main: $(obj).h $(obj).c
#	$(cc)  -c $(neroF).c  $(gtkVer)
	$(cc)  -c $(obj).c   $(gtkVer)
c :
	rm *.o
