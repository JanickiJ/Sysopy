CC = gcc -Wall -std=c11

make all:
	make run
	make frun


frun:
	$(CC) -o lib.out lib.c
	./lib.out 1.txt 2.txt

run:
	$(CC) -o sys.out sys.c
	./sys.out 1.txt 2.txt

clean:
	rm -f *.out
