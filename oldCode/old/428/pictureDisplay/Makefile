#main
DeBug = 1

obj = gtkMain
gtkVer=`pkg-config --cflags --libs gtk+-3.0`

MAKE=make
RUN=make r
DEBUG=make d
CLEAN=make c

fileOperating=common/fileOperating.o
bmpRead=graph/bmpRead.o
searchLine=graph/searchLine.o
subCommon=./common
subGraph=./graph
bmpRead=graph/bmpRead.o
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc -Wall
endif

$(obj) :gtkMain.o MainWindow.o  sub1  sub2
	$(cc)  gtkMain.o  MainWindow.o $(bmpRead) $(fileOperating) -o $(obj)  $(gtkVer)
	sudo ./gtkMain
gtkMain.o : gtkMain.c  	
	$(cc)  -c gtkMain.c $(gtkVer)
MainWindow.o : MainWindow.c MainWindow.h 
		$(cc)  -c MainWindow.c   $(gtkVer)
sub2:
	cd $(subGraph) && $(MAKE)

sub1:
	cd $(subCommon) && $(MAKE)

#c :
#	cd $(subGraph) && $(CLEAN)
#r :
#	cd $(subGraph) && $(RUN)
#d :
#	cd $(subGraph) && $(DEBUG)

c :
	rm gtkMain *.o
	cd $(subGraph) && $(CLEAN)
r :
	./gtkMain
d :
	gdb $(obj)


