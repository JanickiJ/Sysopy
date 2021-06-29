CC = gcc -Wall -std=c11 -g

make all:
	make run
	make frun


frun:
	$(CC) -o lib.out lib.c -lm
	./lib.out

run:
	$(CC) -o sys.out sys.c -lm
	./sys.out

clean:
	rm -f *.out
