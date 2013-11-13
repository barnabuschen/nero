#main
DeBug = 1

obj = gtkMain
gtkVer=`pkg-config --cflags --libs gtk+-3.0`
subCommon=./common
subGraph=./graph

MAKE=make
RUN=make r
DEBUG=make d
CLEAN=make c
type=common/type.h
ifeq ($(DeBug),1)
cc = gcc -Wall -g
else
cc = gcc -Wall
endif
fileOperating=common/fileOperating.o
bmpRead=graph/bmpRead.o
$(obj) :gtkMain.o MainWindow.o  sub1 $(type) sub2
	$(cc)  gtkMain.o  MainWindow.o $(bmpRead) $(fileOperating) -o $(obj)  $(gtkVer)
	./gtkMain
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


