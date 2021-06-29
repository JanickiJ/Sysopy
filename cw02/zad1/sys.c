#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/types/clock_t.h>
#include <sys/times.h>


void sys(int argc, char** argv) {
    int pathSize = 70;
    int file1;
    int file2;
    char path1[pathSize];
    char path2[pathSize];

    if(argc == 1){
        printf("Enter path\n");
        scanf("%s", path1);
        printf("Enter path\n");
        scanf("%s", path2);
    }
    else if(argc == 2){
        strcpy(path1,argv[1]);
        printf("Enter path\n");
        scanf("%s", path2);
    }
    else if(argc == 3){
        strcpy(path1,argv[1]);
        strcpy(path2,argv[2]);
    }

    if(!(file1 = open(path1, O_RDONLY)) || !(file2 = open(path2, O_RDONLY))){
        printf("Incorrect path\n");
        return;
    }


    int fileNum =0;
    char c[1];
    char sep[] ={'\n'};
    int size1 =1;
    int size2 =1;

    while (size1 !=0 && size2!=0) {
        if(fileNum % 2 == 0){
            while ((size1 = read(file1,c,1) > 0) && c[0] != '\n') {
                write(0, c, 1);
            }
        }
        else{
            while ((size2 = read(file2,c,1) > 0) && c[0] != '\n'){
                write(0, c, 1);
            }
        }
        fileNum++;
        write(0,sep,1);
    }

    while ((size1 = read(file1,c,1)) >0){
        write(0, c, 1);
    }

    while (((size2 = read(file2,c,1) > 0)) >0){
        write(0, c, 1);
    }
    close(file1);
    close(file2);
}

double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_1.txt";
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