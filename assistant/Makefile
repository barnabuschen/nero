#main
DeBug = 1

obj = picShow
#gcc -o hello hello.c -L   /usr/lib/x86_64-linux-gnu  -lcairo -I /usr/include/cairo 
ifeq ($(DeBug),1)
cc = gcc -Wall -g  
else
cc = gcc    -
endif
# 
$(obj) :$(obj).c fileOperating.c
	$(cc) $(obj).c  fileOperating.c -o $(obj)  -L /usr/lib/x86_64-linux-gnu  -lcairo  -lhiredis -lasound -I /usr/include/cairo
	# gcc -o  hiredisTest hiredisTest.c  -lhiredis 
	# ./$(obj)
c :
	rm $(obj) *.o *.png

r :
	./$(obj)
d :
	gdb $(obj)