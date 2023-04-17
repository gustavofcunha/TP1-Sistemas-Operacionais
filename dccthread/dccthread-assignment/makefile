all: dccthread

dccthread:	test1.o	dccthread.o	dlist.o
	gcc -o	dccthread	test1.o	dccthread.o	dlist.o

test1.o:	test1.o	dccthread.h
	gcc -o	test1.o 	test1.c 	-c -W -Wall -ansi -pedantic

dccthread.o:	dccthread.c	dccthread.h	dlist.h
	gcc -o	dccthread.o	dccthread.c	-c -W -Wall -ansi -pedantic

dlist.o:	dlist.c	dlist.h
	gcc -o	dlist.o	dlist.c -c -W -Wall -ansi -pedantic

clean:
	rm -rf *.o *~ dccthread
