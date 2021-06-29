//
// Created by ja on 4/22/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int MAX_NUM_OF_PROD =4;
int CHARS_TO_READ =5;
int MAX_FILE_NAME = 30;


void run_producer(char* file_name, int producer_num);
void run_customer(char* file_name);

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments\n");
        return -1;
    }

    mkfifo("pipe", S_IRUSR | S_IWUSR);
    int producers = atoi(argv[1]);
    if(producers > MAX_NUM_OF_PROD || producers < 1){
        fprintf(stderr, "Incorrect num of producers\n");
        return -1;
    }

    run_customer("consumer.txt");
    for (int i = 0; i < producers; i++) {
        char file_name[MAX_FILE_NAME];
        sprintf(file_name,"prod_%d.txt", i);
        run_producer(file_name,i);
    }


    for (int i = 0; i < producers+1; ++i) {
        wait(NULL);
    }
    return 0;
}


void run_producer(char* file_name, int producer_num){
    char producer_char[2];
    char chars_to_read_char[2];
    sprintf(producer_char, "%d", producer_num);
    sprintf(chars_to_read_char, "%d", CHARS_TO_READ);
    if(fork() == 0){
        if(execl("./producer","./producer", "pipe", producer_char, file_name, chars_to_read_char, NULL)==-1){
            fprintf(stderr, "Producer exec error: %s\n", strerror(errno));
        }
    }
}

void run_customer(char* file_name){
    char chars_to_read_char[2];
    sprintf(chars_to_read_char, "%d", CHARS_TO_READ);
    if(fork() == 0){
        if(execl("./consumer","./consumer", "pipe", file_name,chars_to_read_char,NULL) == -1){
            fprintf(stderr, "Customer exec error: %s\n", strerror(errno));
        }
    }
}