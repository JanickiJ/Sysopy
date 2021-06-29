#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merge.h"


struct block* create_block(int size){
    struct block* block = calloc(1, sizeof(struct block));
    block -> size = size;
    block -> lines = calloc(size, sizeof(char*));
    return block;
}

struct table* create_table(int size){
    struct table* table = calloc(1, sizeof(struct table));
    table -> size = size;
    table -> blocks = calloc(size, sizeof(struct block));
    return table;
}

struct pair* create_pair(){
    struct pair* pair = calloc(1, sizeof (struct pair));
    pair -> path1 = calloc(42,sizeof (char*));
    pair -> path2 = calloc(42,sizeof (char*));
    return pair;
}

struct sequence* create_sequence(int size){
    struct sequence* sequence = calloc(1,sizeof (struct sequence));
    sequence -> size = size;
    sequence -> pairs = calloc(size, sizeof (struct pair));
    return sequence;
}


void remove_block(struct table* table, int block_idx){
    if (table->blocks[block_idx] == NULL) return;
    int size = table->blocks[block_idx]->size;

    for(int i=0;i<size;i++)
        free(table->blocks[block_idx]->lines[i]);

    free(table->blocks[block_idx]->lines);
    free(table->blocks[block_idx]);
    table->blocks[block_idx] =NULL;
}

void remove_line(struct table* table ,int block_idx, int line_idx){
    if (table->blocks[block_idx]->lines[line_idx] == NULL) return;
    free(table->blocks[block_idx]->lines[line_idx]);
    table->blocks[block_idx]->lines[line_idx] = NULL;
}



FILE* merge(struct pair* pair){

    FILE* tmp = tmpfile();
    FILE* file1 = fopen(pair -> path1, "r");
    FILE* file2 = fopen(pair -> path2, "r");

    int fileNum =0;
    int c;
    int flag1 = 1, flag2 =1;

    while (flag1 && flag2) {

        if(fileNum % 2 == 0){
            while (((c = getc(file1)) != EOF) && c != '\n') {
                putc(c,tmp);
            }
            if(c == EOF){flag1 =0;}
        }
        else{
            while (((c = getc(file2)) != EOF) && c != '\n')
            {
                putc(c,tmp);
            }
            if(c == EOF){flag2 =0;}
        }
        fileNum++;
        putc('\n',tmp);

    }

    while(flag1){
        while (((c = getc(file1)) != EOF))
        {
            putc(c,tmp);
        }
        if(c == EOF){flag1 =0;}
    }
    while(flag2){
        while (((c = getc(file2)) != EOF))
        {
            putc(c,tmp);
        }
        if(c == EOF){flag2 =0;}
    }

    fclose(file1);
    fclose(file2);
    rewind(tmp);

    return tmp;
}

FILE** sequence_merge(struct sequence* sequence) {
    FILE** tmp= calloc(sequence->size, sizeof (FILE*));

    for (int i = 0; i < sequence->size; i++) {
        tmp[i] = merge(sequence->pairs[i]);
    }
    return tmp;
}


int add_block(FILE* tmp, struct table* table){

    int size = 0;
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, tmp) >= 0){size++;}
    rewind(tmp);

    struct block *block = create_block(size);
    block->size = size;

    int i =0;
    while(getline(&line, &len, tmp) >= 0){
        block->lines[i] = calloc((len + 1), sizeof(char));
        strcpy(block->lines[i], line);
        i++;
    }
    fclose(tmp);

    for(i=0;i<table->size;i++){
        if(table->blocks[i] == NULL) {
            table->blocks[i] = block;
            break;
        }
    }
    if(i == table->size) return -1;
    return i;
}




int lines_count(struct table *table, int block_idx) {
    struct block *block = table->blocks[block_idx];
    if (block == NULL || block->lines == NULL) return -1;
    int result = 0;
    for (int i = 0; i < block->size; i++){
        if (block->lines[i]) result++;
    }
    return result;
}

void print(struct table *table) {
    struct block *block;
    char* line;
    for(int i = 0; i<table->size; i++) {
        block = table->blocks[i];
        if(block != NULL) {
            for (int j = 0; j < block->size; j++) {
                line = block->lines[j];
                if (line != NULL){
                    printf("%s", line);
                }
            }
            printf("%s","\n");
        }
    }
}

