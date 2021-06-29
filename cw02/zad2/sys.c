#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/types/clock_t.h>
#include <sys/times.h>


void sys(int argc, char** argv) {
    int pathSize = 50;

    int file;
    char sep[] ={'\n'};
    char path[pathSize];
    char character;

    if(argc == 3) {
        strcpy(path,argv[2]);
        character = *argv[1];
    }
    else{
        printf("Incorrect arguments");
        return;
    }


    if(!(file = open(path, O_RDONLY))){
        printf("Incorrect path\n");
        return;
    }

    int flag = 0;
    char c[1];
    off_t lineBeginning = 0;

    while((read(file,c,1) > 0)) {
        lineBeginning++;
        if (flag) {
            lseek(file,-lineBeginning, SEEK_CUR);
            while ((read(file,c,1) > 0) && c[0] != '\n') {
                write(0, c, 1);
            }
            write(0, sep, 1);

            flag = 0;
        }
        if(c[0]=='\n'){
            lineBeginning =0;
        }
        if(c[0] == character){
            flag = 1;
        }
    }

    close(file);
}

double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_2.txt";
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