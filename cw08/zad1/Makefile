CC = gcc -g -Wall -std=c11  -D_XOPEN_SOURCE=700

all: main

main: main.c
	$(CC) main.c -o main -pthread -lm

test: main
	./main 4 block pepper.ascii.pgm test_out.pgm

times: main
	./main 1 block pepper.ascii.pgm test_out.pgm
	./main 2 block pepper.ascii.pgm test_out.pgm
	./main 4 block pepper.ascii.pgm test_out.pgm
	./main 8 block pepper.ascii.pgm test_out.pgm
	./main 16 block pepper.ascii.pgm test_out.pgm

	./main 1 numbers pepper.ascii.pgm test_out.pgm
	./main 2 numbers pepper.ascii.pgm test_out.pgm
	./main 4 numbers pepper.ascii.pgm test_out.pgm
	./main 8 numbers pepper.ascii.pgm test_out.pgm
	./main 16 numbers pepper.ascii.pgm test_out.pgm

clean:
	rm -f main out/* test_out.pgm
