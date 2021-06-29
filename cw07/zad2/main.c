//
// Created by ja on 5/8/21.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shared_memory.h"

void create_supplier(int i);
void create_chef(int i);
void handler(int sig);

pid_t* suppliers;
pid_t* chefs;
int N;
int M;

int main(int argc, char* argv[]){
    signal(SIGINT, handler);

    if(argc != 3){
        fprintf(stderr,"Incorrect input\n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);

    suppliers = calloc(M,sizeof(pid_t));
    chefs = calloc(N, sizeof(pid_t));

    create_sem(SEM_OVEN, 1);
    create_sem(SEM_OVEN_FREE, OVEN_CAPACITY);
    create_sem(SEM_TABLE, 1);
    create_sem(SEM_TABLE_COUNTER, 0);
    create_sem(SEM_TABLE_FREE, TABLE_CAPACITY);
    int shm_fd = create_shm();
    struct data* data = attach_memo(shm_fd);
    data->table_idx = 0;
    data->oven_idx = 0;
    detach_memo(data);

    for (int i = 0; i < N; i++) {
        create_chef(i);
    }

    for (int i = 0; i < M; i++) {
        create_supplier(i);
    }

    for (int i = 0; i < M + N; i++) {wait(NULL);}
    return 0;
}

void create_chef(int i){
    pid_t pid;
    if((pid = fork()) == 0){
        chefs[i] = getpid();
        if(execlp("./chef", "./chef", NULL) == -1){
            fprintf(stderr,"Error %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

void create_supplier(int i){
    pid_t pid;
    if((pid = fork()) == 0){
        suppliers[i] = getpid();
        if(execlp("./supplier", "./supplier", NULL) == -1){
            fprintf(stderr,"Error %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

void handler(int sig){
    for (int i = 0; i < N; i++) {
        kill(SIGKILL, chefs[i]);
    }
    for (int i = 0; i < M; i++) {
        kill(SIGKILL, suppliers[i]);
    }
    unlink_sem(SEM_OVEN_FREE);
    unlink_sem(SEM_OVEN);
    unlink_sem(SEM_TABLE);
    unlink_sem(SEM_TABLE_FREE);
    unlink_sem(SEM_TABLE_COUNTER);
    unlink_shm();
    free(suppliers);
    free(chefs);
    exit(EXIT_SUCCESS);
}