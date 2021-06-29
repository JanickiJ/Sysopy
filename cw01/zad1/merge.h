//
// Created by ja on 13.03.2021.
//

#ifndef PRO_MERGE_H
#define PRO_MERGE_H
#include <stdio.h>
struct block {
    int size;
    char** lines;
};

struct table {
    int size;
    struct block** blocks;
};

struct pair {
    char* path1;
    char* path2;
};

struct sequence {
    int size;
    struct pair** pairs;
};




struct block* create_block(int size);
struct table* create_table(int size);
struct pair* create_pair(char* path1, char* path2);
struct sequence* create_sequence(int size);

void remove_block(struct table* table, int block_idx);
void remove_line(struct table* table ,int block_idx, int line_idx);

FILE* merge(struct pair* pair);
FILE** sequence_merge(struct sequence* sequence);
int add_block(FILE* tmp, struct table* table);
int lines_count(struct table* table, int block_idx);
void print(struct table* table);



#endif //PRO_MERGE_H
