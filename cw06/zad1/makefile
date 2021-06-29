CC = gcc -Wall -g -D_XOPEN_SOURCE=700

all: server client

server: server.c
	$(CC) server.c -o server

client: client.c
	$(CC) client.c -o client

clean:
	rm -f server client