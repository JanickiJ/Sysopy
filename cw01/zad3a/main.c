//
// Created by ja on 14.03.2021.
//
#include "merge.h"
#include "merge.c"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>


const char PATH[50] = "/home/ja/Programy/sysopy/JanickiJakub/cw01/";
const char RAPORTPATH[70] = "/home/ja/Programy/sysopy/JanickiJakub/cw01/zad3a/raport3a.txt";
double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}



int main(int arg, char* arguments[]){
    struct tms start_tms, end_tms;


    clock_t start_time = times(&start_tms);

    struct table* table;
    struct sequence *sequence;
    FILE** tmp;
    int len;

    FILE* raport = stdout;

    int i=1;
    char* operation;

    while(i < arg){
        i++;
        operation = arguments[i];

        if(strncmp(operation, "create_table", strlen("create_table")) == 0){
            i++;
            len = atoi(arguments[i]);
            table = create_table(len);
            sequence = create_sequence(len);
        }
        else if(strncmp(operation, "merge_files", strlen("merge_files")) == 0){
            struct pair* pair;
            i++;

            char* token2 = strtok(arguments[i], ":");
            char* token1 = token2;
            token2 = strtok(NULL, " ");
            for (int j = 0; j < sequence->size; ++j) {
                pair = create_pair();
                strcat(pair->path1, PATH);
                strcat(pair->path2, PATH);
                strcat(pair->path1, token1);
                strcat(pair->path2, token2);
                sequence->pairs[j] = pair;
            }
            tmp = sequence_merge(sequence);
        }

        else if(strncmp(operation, "build_blocks", strlen("build_blocks")) == 0){
            if(tmp == NULL) return -1;
            if(table == NULL) return -1;
            for(int j=0;j<len;j++){
                if(table->blocks[j] == NULL){
                    add_block(tmp[j], table);}
            }
        }
        else if(strncmp(operation, "remove_block", strlen("remove_block")) == 0){
            if(table == NULL) return -1;
            i++;
            remove_block(table,atoi(arguments[i]));
        }
        else if(strncmp(operation, "remove_row", strlen("remove_row")) == 0){
            if(table == NULL) return -1;
            i++;
            int block_index = atoi(arguments[i]);
            i++;
            int row_index = atoi(arguments[i]);

            remove_line(table,block_index, row_index);
        }
        else if(strncmp(operation, "print", strlen("print")) == 0){
            print(table);
        }
        else if(strncmp(operation, "test", strlen("test")) == 0){
            raport = fopen(RAPORTPATH, "a+");
            i++;
            char * number_of_pairs = arguments[i];
            i++;
            char * number_of_rows = arguments[i];

            fprintf(raport,"Number of pairs: %s, Number of rows: %s\n", number_of_pairs,number_of_rows);
        }
        else if(strncmp(operation, "end", strlen("end")) == 0){
            break;
        }

    }


    clock_t end_time = times(&end_tms);
    fprintf(raport,"\nTotal_time  :\n");
    fprintf(raport,"Real_time   : %f, ", calculate_time(start_time, end_time));
    fprintf(raport,"User_time   : %f, ", calculate_time(start_tms.tms_utime, end_tms.tms_utime));
    fprintf(raport,"System_time : %f, \n\n", calculate_time(start_tms.tms_stime, end_tms.tms_stime));

    for (int j = 0; j < len; ++j) {
        remove_block(table,j);
        free(sequence->pairs[j]->path1);
        free(sequence->pairs[j]->path2);
        free(sequence->pairs[j]);
    }
    free(sequence);
    free(table);

    return 0;
}
