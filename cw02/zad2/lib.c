#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>



void lib(int argc, char** argv) {
    int pathSize = 70;

    FILE* file;
    char sep[] ={'\n'};
    char path[pathSize];
    char character;
    if(argc == 3) {
        strcpy(path,argv[2]);
        character = *argv[1];
    }
    else{
        fprintf(stdout,"Incorrect arguments");
        return;
    }


    if(!(file = fopen(path, "r"))){
        fprintf(stdout,"Incorrect path\n");
        return;
    }

    int flag = 0;
    char c[1];
    fpos_t lineBeginning;
    fgetpos(file,&lineBeginning);

    while(fread(c,sizeof(char),1,file) > 0) {

        if (flag) {
            fsetpos(file,&lineBeginning);
            while ((fread(c, sizeof(char), 1, file) > 0) && c[0] != '\n') {
                fwrite(c,sizeof(char),1,stdout);
            }
            fwrite(sep,sizeof(char),1,stdout);

            flag = 0;
        }
        if(c[0]=='\n'){
            fgetpos(file,&lineBeginning);
        }
        if(c[0] == character){
            flag = 1;
        }
    }
    fclose(file);
}

double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_2.txt";
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