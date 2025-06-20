CC=gcc
CFLAGS=-Wall -O2
LIBS=-lsqlite3

all: foodrec

foodrec: src/main.c
	$(CC) $(CFLAGS) -o foodrec src/main.c $(LIBS)

clean:
	rm -f foodrec
