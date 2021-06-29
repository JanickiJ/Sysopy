CC = gcc -Wall -std=c11 -g -D_XOPEN_SOURCE=500

all: main

main: main.c
	$(CC) main.c -o main -pthread

test: main
	./main

clean:
	rm -f main
