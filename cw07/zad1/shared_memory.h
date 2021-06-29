//
// Created by ja on 5/8/21.
//

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>

#define SEM_TABLE 0
#define SEM_TABLE_FREE 1
#define SEM_OVEN 2
#define SEM_OVEN_FREE 3
#define SEM_TABLE_COUNTER 4


#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5
struct timespec start;

struct sembuf sem_oven_plus = {SEM_OVEN, 1, 0};
struct sembuf sem_oven_minus = {SEM_OVEN, -1, 0};

struct sembuf sem_table_plus = {SEM_TABLE, 1, 0};
struct sembuf sem_table_minus = {SEM_TABLE, -1, 0};

struct sembuf sem_table_free_plus = {SEM_TABLE_FREE, 1, 0};
struct sembuf sem_table_free_minus = {SEM_TABLE_FREE, -1, 0};

struct sembuf sem_oven_free_plus = {SEM_OVEN_FREE, 1, 0};
struct sembuf sem_oven_free_minus = {SEM_OVEN_FREE, -1, 0};

struct sembuf sem_table_counter_plus = {SEM_TABLE_COUNTER, 1, 0};
struct sembuf sem_table_counter_minus = {SEM_TABLE_COUNTER, -1, 0};

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

enum pizza_type{
    Margherita = 0,
    Diavola = 1,
    Capricciosa = 2,
    Marinara = 3,
    Frutti_di_mare = 4,
    Boscaiola = 5,
    Vegetariana = 6,
    Quattro_fromaggi = 7,
    Quattro_stagioni = 8,
    Prosciutto_e_fungi = 9
};


struct data{
    int oven_counter;
    int table_counter;
    int oven_idx;
    int table_idx;

    enum pizza_type table[TABLE_CAPACITY];
    enum pizza_type oven[OVEN_CAPACITY];
};

void sem_use(int sem_id, struct sembuf *sops, unsigned nsops){
    if(semop(sem_id, sops, nsops) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void sem_remove(int sem_id){
    if(semctl(sem_id, 0, IPC_RMID, NULL) == -1){
        fprintf(stderr,"sem_remove %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int sem_get(key_t sem_key){
    int sem_id;
    if((sem_id =semget(sem_key,5,0)) == -1){
        fprintf(stderr,"sem_get %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sem_id;
}

int sem_create(key_t sem_key, int num){
    int sem_id;
    if((sem_id = semget(sem_key, num, IPC_CREAT | 0666)) == -1){
        fprintf(stderr,"sem_create %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sem_id;
}

void sem_set_val(int sem_id, int sem_num, int val){
    union semun arg;
    arg.val = val;
    printf("%d %d %d\n",sem_id,sem_num,arg.val);
    if(semctl(sem_id, sem_num, SETVAL, arg) == -1){
        fprintf(stderr,"sem_set_val %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}


int shm_create(key_t shk_key){
    int shm_id;
    if((shm_id = shmget(shk_key, sizeof(struct data), IPC_CREAT | IPC_EXCL | 0666)) == -1){
        fprintf(stderr,"shm_create %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return shm_id;
}


void shm_remove(int shm_id){
    if(shmctl(shm_id, IPC_RMID, NULL) == -1){
        fprintf(stderr,"shm_remove %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int shm_get(key_t shm_key){
    int shm_id;
    if((shm_id = shmget(shm_key,sizeof(struct data), 0)) == -1){
        fprintf(stderr,"shm_get %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return shm_id;
}

struct data* shm_attach(int shm_id){
    struct data* data;
    if((data = shmat(shm_id, NULL, 0) ) == NULL){
        fprintf(stderr,"shm_attach %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return data;
}

void shm_detach(struct data* data){
    if(shmdt(data) == -1){
        fprintf(stderr,"shm_detach %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}



char* get_time(){
    char hms[10];
    struct timeval time;
    gettimeofday(&time, NULL);
    char* res = calloc(20,sizeof(char));
    localtime(&time.tv_sec);
    strftime(hms,10,"%T",localtime(&time.tv_sec));
    sprintf(res, "%s:%ld",hms,time.tv_usec);
    return res;
}

#endif //SHARED_MEMORY_H
