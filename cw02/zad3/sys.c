#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/types/clock_t.h>
#include <sys/times.h>
#include <string.h>


char* pathA = "a.txt";
char* pathB = "b.txt";
char* pathC = "c.txt";
char* pathData = "dane.txt";
char sep[1] ={'\n'};

void sys() {

    int A = open(pathA, O_WRONLY | O_CREAT);
    int even =0;
    int B = open(pathB,O_WRONLY | O_CREAT);
    int C = open(pathC,O_WRONLY | O_CREAT);
    int data = open(pathData, O_RDONLY);

    if(!A || !B || !C || !data){
        printf("Invalid path");
        return;
    }

    char* c = calloc(1,sizeof(char));
    int numberInt=0;
    char num[32];

    while(read(data,c,1) > 0) {
        if (c[0] == '\n') {
            memset(num, '\0', 32);
            sprintf(num,"%d",numberInt);
            if (numberInt % 2 == 0) {
                even++;
            }

            if (((int) (numberInt / 10)) % 10 == 7 || ((int) (numberInt / 10)) % 10 == 0) {
                write(B, num, strlen(num));
                write(B, sep, 1);
            }

            if (((int) sqrt(numberInt)) * ((int) sqrt(numberInt)) == numberInt) {
                write(C, num, strlen(num));
                write(C, sep, 1);
            }
            numberInt = 0;
        } else {
            numberInt = numberInt * 10 + atoi(c);
        }
    }

    write(A,"Liczb parzystych jest \0", 22);
    char e[32];

    sprintf(e,"%d",even);
    write(A, e,strlen(e));

    close(A);
    close(B);
    close(C);
    close(data);
}

double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}


int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_3.txt";
    FILE* raport = fopen(raportPath, "a+");

    struct tms start_tms, end_tms;

    clock_t start_time = times(&start_tms);
    sys();
    clock_t end_time = times(&end_tms);
    fprintf(raport,"System_time  :\n");
    fprintf(raport,"Real_time   : %f, ", calculate_time(start_time, end_time));
    fprintf(raport,"User_time   : %f, ", calculate_time(start_tms.tms_utime, end_tms.tms_utime));
    fprintf(raport,"System_time : %f, \n\n", calculate_time(start_tms.tms_stime, end_tms.tms_stime));
    fclose(raport);
    return -1;
}