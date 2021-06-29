#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

void lib(int argc, char** argv) {
    int pathSize = 50;
    FILE* file1;
    FILE* file2;
    char path1[pathSize];
    char path2[pathSize];

    if(argc == 1){
        fprintf(stdout,"Enter path\n");
        fscanf(stdin,"%s", path1);
        fprintf(stdout,"Enter path\n");
        fscanf(stdin,"%s", path2);
    }
    else if(argc == 2){
        strcpy(path1,argv[1]);
        fprintf(stdout,"Enter path\n");
        fscanf(stdin,"%s", path2);
    }
    else if(argc == 3){
        strcpy(path1,argv[1]);
        strcpy(path2,argv[2]);
    }

    if(!(file1 = fopen(path1, "r")) || !(file2 = fopen(path2, "r"))){
        fprintf(stdout,"Incorrect path\n");
        return;
    }


    int fileNum =0;
    char c[1];
    char sep[] ={'\n'};
    size_t size1 =1;
    size_t size2 =1;

    while (size1 !=0 && size2!=0) {

        if(fileNum % 2 == 0){
            while ((size1 = fread(c,sizeof(char),1,file1) > 0) && c[0] != '\n') {
                fwrite(c,sizeof(char),1,stdout);
            }
        }
        else{
            while ((size2 = fread(c,sizeof(char),1,file2)> 0) && c[0] != '\n')
            {
                fwrite(c,sizeof(char),1,stdout);
            }
        }
        fileNum++;
        fwrite(sep, sizeof(char), 1, stdout);
    }

    while (((size1 = fread(c,sizeof(char),1,file1)) >0))
    {
        fwrite(c,sizeof(char),1,stdout);
    }

    while (((size2 = fread(c,sizeof(char),1,file2)) >0))
    {
        fwrite(c,sizeof(char),1,stdout);
    }


    fclose(file1);
    fclose(file2);
}

double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_1.txt";
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