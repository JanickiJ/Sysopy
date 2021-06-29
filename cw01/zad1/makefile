CC = gcc -Wall -std=c11 -g


all: static shared

static: 
	$(CC) -c merge.c -o merge.o
	ar rcs merge.a merge.o

shared:
	$(CC) -c -fPIC merge.c -o merge.o
	$(CC) -shared -fPIC -o merge.so merge.o

clean:
	rm -f *.out *.so *.o *.a

