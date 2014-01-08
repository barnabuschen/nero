#main
DeBug = 1

obj = gtkMain
gtkVer=`pkg-config --cflags --libs gtk+-3.0`
#opencvKer=`pkg-config opencv --cflags opencv`
subCommon=./common
subGraph=./graph
subCandy=./opcv
subtools=./tools
subNeuralNetwork=./NeuralNetwork
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
fileOperating=common/fileOperating.o
bmpRead=graph/bmpRead.o
searchLine=graph/searchLine.o
candy=opcv/candy.o
nero=common/Neuron.o
tools=tools/readUTF8File.o
dot=tools/createDot.o

NeuNetwork=NeuralNetwork/NeuralNetwork.o
neroOperating=NeuralNetwork/NeuralOperating.o
#$(tools) $(NeuNetwork)  $(dot) $(fileOperating)
$(obj) :gtkMain.o MainWindow.o  
	cd $(subtools) && $(MAKE)
	cd $(subNeuralNetwork) && $(MAKE)
	cd $(subCommon) && $(MAKE)
	$(cc)   gtkMain.o  MainWindow.o  $(NeuNetwork) $(tools) $(dot)  $(fileOperating) $(neroOperating) -o $(obj)  $(gtkVer) 
	sudo ./gtkMain
gtkMain.o : gtkMain.c  	
	$(cc)  -c gtkMain.c $(gtkVer)
MainWindow.o : MainWindow.c MainWindow.h $(NeuNetwork) $(tools) $(dot) $(neroOperating)
		$(cc)  -c MainWindow.c   $(gtkVer)
#sub2:
#	cd $(subGraph) && $(MAKE)

#sub1:
#	cd $(subCommon) && $(MAKE)
#sub3:
#	cd $(subCandy) && $(MAKE)

#c :
#	cd $(subGraph) && $(CLEAN)
#r :
#	cd $(subGraph) && $(RUN)
#d :
#	cd $(subGraph) && $(DEBUG)

c :
	
	cd $(subtools) && $(CLEAN)
	cd $(subNeuralNetwork) && $(CLEAN)
	cd $(subCommon) && $(CLEAN)
	rm gtkMain *.o
r :
	./gtkMain
d :
	gdb $(obj)


