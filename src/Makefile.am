

#AM_CPPFLAGS  = @GTK_CFLAGS@
#LDADD = @GTK_LIBS@
AUTOMAKE_OPTIONS = subdir-objects
#AM_CFLAGS = $(neroMain_CFLAGS)
bin_PROGRAMS = neroMain
neroMain_SOURCES = neroMain.c neroMain.h common/fileOperating.h common/Neuron.h common/fileOperating.c tools/readUTF8File.h tools/readUTF8File.c tools/createDot.h tools/createDot.c tools/Nero_IO.h tools/Nero_IO.c NeuralNetwork/NeuralOperating.h NeuralNetwork/NeuralOperating.c common/type.h NeuralNetwork/NeuralNetwork.h NeuralNetwork/NeuralNetwork.c tools/Nero_Task.h tools/Nero_Task.c common/sounds.h common/sounds.c
neroMain_LDADD = $(neroMain_LIBS)

AM_LDFLAGS =$(neroMain_CFLAGS)
#SUBDIRS		= common data log NeroConfig NeuralNetwork NeuronData tools
