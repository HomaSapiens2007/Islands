CC = gcc
CFLAGS = -c

all: island

island: functions.o island.o
	$(CC) island.o functions.o -o island

island.o: island.c
	$(CC) $(CFLAGS) island.c -o island.o

functions.o: functions.c
	$(CC) $(CFLAGS) functions.c -o functions.o
