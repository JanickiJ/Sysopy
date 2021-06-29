//
// Created by ja on 14.03.2021.
//

#include "merge.h"
#include "merge.c"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <ctype.h>
#include <unistd.h>


const char PATH[50] = "/home/ja/Programy/sysopy/JanickiJakub/cw01/";
const char RAPORTPATH[70] = "/home/ja/Programy/sysopy/JanickiJakub/cw01/zad2/raport2.txt";
double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}



int main(){
    struct tms start_tms, end_tms;
    struct tms start_tms_cmd, end_tms_cmd;

    clock_t start_cmd, end_cmd;

    clock_t start_time = times(&start_tms);

    struct table* table;
    struct sequence *sequence;
    FILE** tmp;
    int len;

    char line[256];
    fgets(line, sizeof(line), stdin);

    FILE* raport = stdout;

    while(strcmp(line, "end") != 0){

        start_cmd = times(&start_tms_cmd);

        if(strncmp(line, "create_table", strlen("create_table")) == 0){
            len = atoi(line + strlen("create_table"));
            table = create_table(len);
            sequence = create_sequence(len);
        }
        else if(strncmp(line, "merge_files", strlen("merge_files")) == 0){
            char * token;
            token = strtok(line, " ");
            struct pair* pair;

            for(int i=0;i<len;i++) {
                pair = create_pair();

                strcat(pair->path1, PATH);
                strcat(pair->path2, PATH);
                token = strtok(NULL, ":");

                strcat(pair->path1,token);
                token = strtok(NULL, " ");

                strcat(pair->path2,token);
                sequence->pairs[i] = pair;
            }
            int diff = strlen(sequence->pairs[len-1]->path2)-1;
            sequence->pairs[len-1]->path2[diff] ='\0';

            tmp = sequence_merge(sequence);

        }
        else if(strncmp(line, "build_blocks", strlen("build_blocks")) == 0){
            if(tmp == NULL) return -1;
            if(table == NULL) return -1;
            for(int i=0;i<len;i++){
                if(table->blocks[i] == NULL){
                add_block(tmp[i], table);}
            }
        }
        else if(strncmp(line, "remove_block", strlen("remove_block")) == 0){
            if(table == NULL) return -1;
            remove_block(table,atoi(line+strlen("remove_block")));
        }
        else if(strncmp(line, "remove_row", strlen("remove_row")) == 0){
            if(table == NULL) return -1;
            char * token = strtok(line, " ");
            token = strtok(NULL, " ");
            int block_index = atoi(token);
            token = strtok(NULL, " ");
            int row_index = atoi(token);

            remove_line(table,block_index, row_index);
        }
        else if(strncmp(line, "print", strlen("print")) == 0){
            print(table);
        }
        else if(strncmp(line, "test", strlen("test")) == 0){
            raport = fopen(RAPORTPATH, "a+");

            char * number_of_pairs = strtok(line, " ");
            number_of_pairs = strtok(NULL, " ");
            char * number_of_rows = strtok(NULL, " ");


            fprintf(raport,"Number of pairs: %s, Number of rows: %s\n", number_of_pairs,number_of_rows);
        }
        else if(strncmp(line, "end", strlen("end")) == 0){
            break;
        }

        end_cmd = times(&end_tms_cmd);
        fprintf(raport,"%s", line);
        fprintf(raport,"Real_time   : %f, ", calculate_time(start_cmd, end_cmd));
        fprintf(raport,"User_time   : %f, ", calculate_time(start_tms_cmd.tms_utime, end_tms_cmd.tms_utime));
        fprintf(raport,"System_time : %f\n", calculate_time(start_tms_cmd.tms_stime, end_tms_cmd.tms_stime));

        fgets(line, sizeof(line), stdin);
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
