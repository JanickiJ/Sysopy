CC = gcc -Wall -std=c11 -g

make all:
	make frun


frun:
	$(CC) -o lib.out lib.c
	./lib.out v 1.txt

run:
	$(CC) -o sys.out sys.c
	./sys.out v 1.txt

clean:
	rm -f *.out
