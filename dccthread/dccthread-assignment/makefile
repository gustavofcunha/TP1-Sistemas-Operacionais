#gu-#
CC = gcc
OBJS = dccthread.o dlist.o
HDRS = dccthread.h dlist.h 

EXE = dccthread test1 test2 test3 test4 test5 test6

all: $(EXE)

dccthread: $(OBJS)
	$(CC) -g -o dccthread $(OBJS)

dccthread.o: $(HDRS) dccthread.c
	$(CC) -g -o dccthread.o -c dccthread.c

dlist.o: $(HDRS) dlist.c
	$(CC) -g -o dlist.o -c dlist.c
	
test1: $(OBJS)
	$(CC) -g -o test1 $(OBJS)

test1.o: $(HDRS) test1.c
	$(CC) -g -o test1.o -c test1.c

test2: $(OBJS)
	$(CC) -g -o test2 $(OBJS)

test2.o: $(HDRS) test2.c
	$(CC) -g -o test2.o -c test2.c

test3: $(OBJS)
	$(CC) -g -o test3 $(OBJS)

test3.o: $(HDRS) test3.c
	$(CC) -g -o test3.o -c test3.c

test4: $(OBJS)
	$(CC) -g -o test4 $(OBJS)

test4.o: $(HDRS) test4.c
	$(CC) -g -o test4.o -c test4.c

test5: $(OBJS)
	$(CC) -g -o test5 $(OBJS)

test5.o: $(HDRS) test5.c
	$(CC) -g -o test5.o -c test1.c

test6: $(OBJS)
	$(CC) -g -o test6 $(OBJS)

test6.o: $(HDRS) test6.c
	$(CC) -g -o test6.o -c test6.c

clean:
	rm $(EXE) $(OBJS)
#-gu#