# makefile for thread pool
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: buffer.o main.o thread.o
	$(CC) $(CFLAGS) -o main buffer.o main.o thread.o $(PTHREADS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(PTHREADS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)

thread.o: thread.c thread.h
	$(CC) $(CFLAGS) -c thread.c $(PTHREADS)

clean:
	rm -rf *.o
	rm -rf main
