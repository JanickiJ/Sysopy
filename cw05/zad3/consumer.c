//
// Created by ja on 4/22/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <errno.h>

void write_output(FILE* text_file, int N, char* buffer);

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Incorrect arguments\n");
        return -1;
    }

    int N = atoi(argv[3]);

    FILE* pipe_file = fopen(argv[1], "r");
    FILE* text_file = fopen(argv[2], "w+");

    if(pipe_file == NULL || text_file == NULL){
        fprintf(stderr, "Incorrect file paths\n");
        return -1;
    }

    fputs("\n\n\n\n\n\n", text_file);
    char* buffer = malloc((N + 3)*sizeof(char));
    buffer[N+2] ='\0';
    while(fgets(buffer, N+2, pipe_file) != NULL){
        if ((flock(fileno(text_file), LOCK_EX)) < 0){
            fprintf(stderr, "Flock error: %s\n", strerror(errno));
            return -1;
        }

        write_output(text_file,N,buffer);

        if ((flock(fileno(text_file), LOCK_UN)) < 0){
            fprintf(stderr, "Flock error: %s\n", strerror(errno));
            return -1;
        }
    }
    free(buffer);
    fclose(text_file);
}

void write_output(FILE* text_file,int N, char* buffer){

    int insert_line = atoi(&buffer[0]);
    char* text = calloc(N+1, sizeof(char));
    text = buffer +2;

    rewind(text_file);

    int lines = 1;
    char c;
    char* file_copy = malloc(32 * sizeof(char));
    size_t length = 0;
    size_t buffer_size = 32;
    while ((c = fgetc(text_file)) != EOF) {
        if (buffer_size == length) {
            buffer_size *= 2;
            file_copy = realloc(file_copy, buffer_size);
        }
        if(c == '\n') {
            lines++;
        }
        file_copy[length++] = c;
    }

    rewind(text_file);


    int curr_line = 0;
    for (int i = 0; i < length; i++) {
        c = file_copy[i];
        if(c == '\n'){
            if(insert_line == curr_line){
                fputs(text, text_file);
            }
            curr_line++;
        }
        fputc(c,text_file);
    }

    rewind(text_file);
}
