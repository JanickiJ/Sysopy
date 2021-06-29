#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

void lib(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stdout,"Incorrect arguments");
        return;
    }

    FILE* we;
    FILE* wy;

    if(!(we = fopen(argv[1], "r")) || !(wy = fopen(argv[2], "w"))){
        fprintf(stdout,"Incorrect path\n");
        return;
    }

    char c[1];
    char sep = {'\n'};
    int counter =0;
    while(fread(c,sizeof(char),1,we) > 0) {
        counter++;
        if(c[0] =='\n'){
            counter =0;
        }
        if(counter == 50){
            fwrite(&sep, sizeof(char), 1, wy);
            counter =0;
        }
        fwrite(c, sizeof(char), 1, wy);
    }

    fclose(we);
    fclose(wy);
}


double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_5.txt";
    FILE* raport = fopen(raportPath, "a+");

    struct tms start_tms, end_tms;

    clock_t start_time = times(&start_tms);
    lib(argc,argv);
    clock_t end_time = times(&end_tms);
    fprintf(raport,"Library_time  :\n");
    fprintf(raport,"Real_time   : %f, ", calculate_time(start_time, end_time));
    fprintf(raport,"User_time   : %f, ", calculate_time(start_tms.tms_utime, end_tms.tms_utime));
    fprintf(raport,"System_time : %f, \n\n", calculate_time(start_tms.tms_stime, end_tms.tms_stime));
    fclose(raport);
    return -1;
}