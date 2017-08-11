

#AM_CPPFLAGS  = @GTK_CFLAGS@
#LDADD = @GTK_LIBS@
AUTOMAKE_OPTIONS = subdir-objects
#AM_CFLAGS = $(neroMain_CFLAGS)
bin_PROGRAMS = neroMain
neroMain_SOURCES = neroMain.c neroMain.h common/fileOperating.h common/Neuron.h common/fileOperating.c tools/readUTF8File.h tools/readUTF8File.c tools/createDot.h tools/createDot.c tools/Nero_IO.h tools/Nero_IO.c NeuralNetwork/NeuralOperating.h NeuralNetwork/NeuralOperating.c common/type.h NeuralNetwork/NeuralNetwork.h NeuralNetwork/NeuralNetwork.c tools/Nero_Task.h tools/Nero_Task.c common/sounds.h common/sounds.c



#连接时所需要的库文件,这里表示需要两个库文件的支持，下面会看到这个库文件又是怎么用Makefile.am文件后成的。
#$(top_builddir)全局变量,是Makefile.am系统定义的一个基本路径变量，表示生成目标文件的最上层目录
#$(top_srcdir)，这个表示工程的最顶层目录

neroMain_LDADD = $(neroMain_LIBS) \
	/usr/local/lib/libhiredis.a

#neroMain_LDADD = $(neroMain_LIBS)

#neroMain_LDADD = $(top_builddir)/sx/libsession.la
AM_CPPFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include  

#
AM_LDFLAGS =$(neroMain_CFLAGS)
#SUBDIRS		= common data log NeroConfig NeuralNetwork NeuronData tools
