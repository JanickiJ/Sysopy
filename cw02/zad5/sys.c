#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/types/clock_t.h>
#include <sys/times.h>

void sys(int argc, char** argv) {
    if(argc != 3) {
        printf("Incorrect arguments");
        return;
    }

    int we;
    int wy;

    if((!(we = open(argv[1], O_RDONLY))) || !(wy = open(argv[2], O_WRONLY|O_CREAT))){
        printf("Incorrect path\n");
        return;
    }

    char c[1];
    char sep = {'\n'};
    int counter =0;
    while(read(we,c,1) > 0) {
        counter++;
        if(c[0] =='\n'){
            counter =0;
        }
        if(counter == 50){
            write(wy, &sep, 1);
            counter =0;
        }
        write(wy, c, 1);
    }

    close(we);
    close(wy);
}


double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_5.txt";
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