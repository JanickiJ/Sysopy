CC = gcc -Wall -std=c11 -g

#zad1
all: static shared


static_lib:
	$(CC) -c merge.c -o merge.o
	ar rcs merge.a merge.o

shared_lib:
	$(CC) -c -fPIC merge.c -o merge.o
	$(CC) -shared -fPIC -o merge.so merge.o

clean:
	rm -f *.out *.so *.o *.a

#zad2

main: main.c static_lib
	$(CC) -c main.c -o main.o
	$(CC) main.o merge.a -o main.out

#zad3

static: static_lib
	$(CC) -c main.c -o main.o
	$(CC) main.o merge.a -o main.out

shared: shared_lib
	$(CC) main.c -L. merge.so -I . -o main.out

dynamic: shared_lib
	$(CC) main.c -ldl -D dynamic -o main.out

#test
test:
	make static_test
	make shared_test
	make dynamic_test


run_test_all_libraries_opt: clean_raport3b.txt
	echo "---------" >> raport3b.txt
	echo " OPT = 01">> raport3b.txt
	echo "---------" >> raport3b.txt

	make run_test_all_libraries OPT=-O1

	echo "---------" >> raport3b.txt
	echo " OPT = O2 " >> raport3b.txt
	echo "---------" >> raport3b.txt

	make run_test_all_libraries OPT=-O2

	echo "---------" >> raport3b.txt
	echo " OPT = O3 " >> raport3b.txt
	echo "---------" >> raport3b.txt

	make run_test_all_libraries OPT=-O3

	echo "---------" >> raport3b.txt
	echo " OPT = Os " >> raport3b.txt
	echo "---------" >> raport3b.txt

	make run_test_all_libraries OPT=-Os




clean_raport3b.txt:
	rm -f raport3b.txt
	touch raport3b.txt

static_test: clean static
	echo "\n" >> raport3b.txt
	echo "   STATIC TEST    " >> raport3b.txt
	echo "\n\n" >> raport3b.txt
	make run_tests_all_sizes

shared_test: clean shared
	echo "\n" >> raport3b.txt
	echo "   SHARED TEST    " >> raport3b.txt
	echo "\n\n" >> raport3b.txt
	make run_tests_all_sizes


dynamic_test: clean dynamic
	echo "\n" >> raport3b.txt
	echo "   DYNAMIC TEST    " >> raport3b.txt
	echo "\n\n" >> raport3b.txt
	make run_tests_all_sizes




run_tests_all_sizes:
	make small_small
	make small_medium
	make small_big
	make medium_small
	make medium_medium
	make medium_big
	make big_small
	make big_medium
	make big_big
	make huge_huge


small_small:
	./main.out 1 test small small create_table 30 merge_files 1.txt:1.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

small_medium:
	./main.out 1 test small medium create_table 30 merge_files 2.txt:2.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

small_big:
	./main.out 1 test small big create_table 30 merge_files 3.txt:3.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

medium_small:
	./main.out 1 test medium small create_table 80 merge_files 1.txt:1.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

medium_medium:
	./main.out 1 test medium medium create_table 80 merge_files 2.txt:2.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

medium_big:
	./main.out 1 test medium big create_table 80 merge_files 3.txt:3.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

big_small:
	./main.out 1 test big small create_table 140 merge_files 1.txt:1.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

big_medium:
	./main.out 1 test big medium create_table 140 merge_files 2.txt:2.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

big_big:
	./main.out 1 test big big create_table 140 merge_files 3.txt:3.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

huge_huge:
	./main.out 1 test huge huge create_table 200 merge_files 3.txt:3.txt build_blocks remove_block 2 remove_block 3 remove_row 4 1 end

