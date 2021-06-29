#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/times.h>


char* pathA = "a.txt";
char* pathB = "b.txt";
char* pathC = "c.txt";
char* pathData = "dane.txt";


void lib() {

    FILE* A = fopen(pathA,"w");
    int even=0;
    FILE* B = fopen(pathB,"w");
    FILE* C = fopen(pathC,"w");
    FILE* data = fopen(pathData,"r");

    if(!A || !B || !C || !data){
        printf("Invalid path");
        return;
    }

    char* c = calloc(1,sizeof(char));
    int numberInt=0;
    while(fread(c,sizeof(char),1,data) > 0) {
        if(c[0] == '\n'){
            if(numberInt%2 ==0){
                even++;
            }

            if(((int)(numberInt/10))%10 == 7 || ((int)(numberInt/10))%10 == 0){
                fprintf(B,"%d\n",numberInt);
            }

            if(((int)sqrt(numberInt))*((int)sqrt(numberInt)) ==numberInt){
                fprintf(C,"%d\n",numberInt);
            }
            numberInt=0;
        }
        numberInt = numberInt*10+atoi(c);
    }
    fprintf(A,"Liczb parzystych jest: %d",even);

    fclose(A);
    fclose(B);
    fclose(C);
    fclose(data);
}

double calculate_time(clock_t start, clock_t end) {
    return ((double ) (end - start)) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    char raportPath[50] = "pomiar_zad_3.txt";
    FILE* raport = fopen(raportPath, "a+");

    struct tms start_tms, end_tms;

    clock_t start_time = times(&start_tms);
    lib();
    clock_t end_time = times(&end_tms);
    fprintf(raport,"Library_time  :\n");
    fprintf(raport,"Real_time   : %f, ", calculate_time(start_time, end_time));
    fprintf(raport,"User_time   : %f, ", calculate_time(start_tms.tms_utime, end_tms.tms_utime));
    fprintf(raport,"System_time : %f, \n\n", calculate_time(start_tms.tms_stime, end_tms.tms_stime));
    fclose(raport);
    return -1;
}