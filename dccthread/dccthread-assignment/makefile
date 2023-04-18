#gu-#
CC = gcc -Wall
OBJS = dccthread.o dlist.o 
HDRS = dccthread.h dlist.h

EXE = dccthread.o dlist.o 

all: $(EXE)

dccthread.o: $(HDRS) dccthread.c
	$(CC) -g -o dccthread.o -c dccthread.c

dlist.o: $(HDRS) dlist.c
	$(CC) -g -o dlist.o -c dlist.c

clean:
	rm $(EXE) $(OBJS)
#-gu#
