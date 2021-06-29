CC = gcc -Wall -std=c11 -g

#zad1
all: static shared


static:
	$(CC) -c merge.c -o merge.o
	ar rcs merge.a merge.o

shared:
	$(CC) -c -fPIC merge.c -o merge.o
	$(CC) -shared -fPIC -o merge.so merge.o

clean:
	rm -f *.out *.so *.o *.a

#zad2

test: main.c static
	$(CC) -c main.c -o main.o
	$(CC) main.o merge.a -o main.out
	./main.out
