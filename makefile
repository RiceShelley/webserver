CC=gcc
CFLAGS=-Wall -D_GNU_SOURCE -lsqlite3 -llua

main:
	$(CC) main.c -o serv $(CFLAGS)
