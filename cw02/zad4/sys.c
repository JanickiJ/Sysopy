#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/types/clock_t.h>
#include <sys/times.h>


void sys(int argc, char** argv) {
    if(argc != 5) {
        printf("Incorrect arguments");
        return;
    }

    int we;
    int wy;

    if((!(we = open(argv[1], O_RDONLY))) || !(wy = open(argv[2], O_WRONLY|O_CREAT))){
        printf("Incorrect path\n");
        return;
    }

    char* c = calloc(1,sizeof(char));
    char* copy = calloc(1,sizeof(char));
    off_t lineBeginning = 0;
    int common;

    while(read(we,c,1) > 0) {
        common =0;
        lineBeginning=0;
        strcpy(copy,c);
        while (common < strlen(argv[3]) && c[0] == argv[3][common]) {
            common++;
            lineBeginning++;
            read(we,c,1);
        }

        if(common == strlen(argv[3])){
            write(wy, argv[4], strlen(argv[4]));
            lineBeginning=0;
            write(wy, c, 1);
        } else{
            lseek(we,-lineBeginning, SEEK_CUR);
            write(wy, c, 1);
        }
    }
    free(c);
    free(copy);
    close(we);
    close(wy);
}

double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_4.txt";
    FILE* raport = fopen(raportPath, "a+");

    struct tms start_tms, end_tms;

    clock_t start_time = times(&start_tms);
    sys(argc,argv);
    clock_t end_time = times(&end_tms);
    fprintf(raport,"System_time  :\n");
    fprintf(raport,"Real_time   : %f, ", calculate_time(start_time, end_time));
    fprintf(raport,"User_time   : %f, ", calculate_time(start_tms.tms_utime, end_tms.tms_utime));
    fprintf(raport,"System_time : %f, \n\n", calculate_time(start_tms.tms_stime, end_tms.tms_stime));
    fclose(raport);
    return -1;
}