#gu-#
CC = gcc
OBJS = dccthread.o dlist.o
HDRS = dccthread.h dlist.h 

EXE = dccthread

all: $(EXE)

dccthread: $(OBJS)
	$(CC) -g -o dccthread $(OBJS)

dccthread.o: $(HDRS) dccthread.c
	$(CC) -g -o dccthread.o -c dccthread.c

dlist.o: $(HDRS) dlist.c
	$(CC) -g -o dlist.o -c dlist.c
	
clean:
	rm $(EXE) $(OBJS)
#-gu#