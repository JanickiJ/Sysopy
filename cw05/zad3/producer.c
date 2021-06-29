//
// Created by ja on 4/22/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv){
    if(argc != 5){
        fprintf(stderr, "Incorrect arguments\n");
        return -1;
    }

    char* producer_no = argv[2];
    int N = atoi(argv[4]);

    FILE* pipe_file = fopen(argv[1], "w+");
    FILE* text_file = fopen(argv[3], "r");

    if(pipe_file == NULL || text_file == NULL){
        fprintf(stderr, "Incorrect file paths\n");
        return -1;
    }

    srand(time(NULL));

    char buffer[N];
    while (fgets(buffer, N, text_file) != NULL){
        sleep(1);
        fprintf(pipe_file, "%s|%s", producer_no, buffer);
    }

    fclose(text_file);
    return 0;
}
