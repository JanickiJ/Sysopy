CC = gcc -Wall

all: test

main:
	$(CC) main.c -o main.o

exec:
	$(CC) exec.c -o exec

clean:
	rm -f *.out exec *.o


test: main exec
	./main.o ignore fork
	./main.o handler fork
	./main.o mask fork
	./main.o pending fork
	./main.o ignore exec
	./main.o mask exec
	./main.o pending exec



