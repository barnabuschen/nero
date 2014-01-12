

#AM_CPPFLAGS  = @GTK_CFLAGS@
#LDADD = @GTK_LIBS@

#AM_CFLAGS = $(gtkMain_CFLAGS)
bin_PROGRAMS = gtkMain
gtkMain_SOURCES = gtkMain.c MainWindow.c MainWindow.h common/fileOperating.h common/fileOperating.c  tools/readUTF8File.h tools/readUTF8File.c tools/createDot.h tools/createDot.c tools/Nero_IO.h tools/Nero_IO.c NeuralNetwork/NeuralOperating.h NeuralNetwork/NeuralOperating.c common/type.h NeuralNetwork/NeuralNetwork.h NeuralNetwork/NeuralNetwork.c
gtkMain_LDADD = $(gtkMain_LIBS)

AM_LDFLAGS =$(gtkMain_CFLAGS)

