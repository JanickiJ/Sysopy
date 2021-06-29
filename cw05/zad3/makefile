CC = gcc -Wall

all: main.c producer.c consumer.c
	make clean
	$(CC) main.c -o main
	$(CC) producer.c -o producer
	$(CC) consumer.c -o consumer
	touch consumer.txt

test: all
	./main 3
	cat consumer.txt

clean:
	rm -f main
	rm -f producer
	rm -f consumer
	rm -f pipe
	rm -rf files
	rm -f consumer.txt
