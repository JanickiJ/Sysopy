CC = gcc -Wall -g -D_XOPEN_SOURCE=700
all: client server

server: server.c
	$(CC) server.c -o server -lrt

client: client.c
	$(CC) client.c -o client -lrt

clean:
	rm -f server client