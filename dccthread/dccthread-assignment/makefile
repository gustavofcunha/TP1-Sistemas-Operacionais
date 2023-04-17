#gu-#
CC = gcc -Wall
OBJS = dccthread.o dlist.o
HDRS = dccthread.h dlist.h 

EXE = dccthread.out test1.out test2.out test3.out test4.out test5.out test6.out test7.out test8.out

all: $(EXE)

dccthread.out: $(OBJS)
	$(CC) -g -o dccthread.out $(OBJS)

dccthread.o: $(HDRS) dccthread.c
	$(CC) -g -o dccthread.o -c dccthread.c

dlist.o: $(HDRS) dlist.c
	$(CC) -g -o dlist.o -c dlist.c
	
test1.out: $(OBJS)
	$(CC) -g -o test1.out $(OBJS)

test1.o: $(HDRS) test1.c
	$(CC) -g -o test1.o -c test1.c

test2.out: $(OBJS)
	$(CC) -g -o test2.out $(OBJS)

test2.o: $(HDRS) test2.c
	$(CC) -g -o test2.o -c test2.c

test3.out: $(OBJS)
	$(CC) -g -o test3.out $(OBJS)

test3.o: $(HDRS) test3.c
	$(CC) -g -o test3.o -c test3.c

test4.out: $(OBJS)
	$(CC) -g -o test4.out $(OBJS)

test4.o: $(HDRS) test4.c
	$(CC) -g -o test4.o -c test4.c

test5.out: $(OBJS)
	$(CC) -g -o test5.out $(OBJS)

test5.o: $(HDRS) test5.c
	$(CC) -g -o test5.o -c test1.c

test6.out: $(OBJS)
	$(CC) -g -o test6.out $(OBJS)

test6.o: $(HDRS) test6.c
	$(CC) -g -o test6.o -c test6.c

test7.out: $(OBJS)
	$(CC) -g -o test7.out $(OBJS)

test7.o: $(HDRS) test7.c
	$(CC) -g -o test7.o -c test7.c

test8.out: $(OBJS)
	$(CC) -g -o test8.out $(OBJS)

test8.o: $(HDRS) test8.c
	$(CC) -g -o test8.o -c test8.c

clean:
	rm $(EXE) $(OBJS)
#-gu#