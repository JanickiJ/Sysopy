CC = gcc -Wall -g

all: main

main: interpreter.c
	$(CC) interpreter.c -o interpreter

test: all
	./interpreter commands.txt

clean:
	rm -f interpreter *.o