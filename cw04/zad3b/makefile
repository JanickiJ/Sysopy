CC = gcc -Wall -g

all: compile test

compile:
	$(CC) sender.c -o sender
	$(CC) catcher.c -o catcher

test: compile
	./catcher kill&
	./sender `pidof catcher` 100 kill
	./catcher sigqueue&
	./sender `pidof catcher` 100 sigqueue
	./catcher sigrt&
	./sender `pidof catcher` 100 sigrt

clean:
	rm -f *.o *.a catcher sender