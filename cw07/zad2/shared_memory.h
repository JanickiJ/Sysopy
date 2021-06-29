//
// Created by ja on 5/8/21.
//

#ifndef JANICKIJAKUB_SHARED_MEMORY_H
#define JANICKIJAKUB_SHARED_MEMORY_H

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

#define SEM_TABLE "/sem_table"
#define SEM_TABLE_FREE "/sem_table_free"
#define SEM_OVEN "/sem_oven"
#define SEM_OVEN_FREE "/sem_oven_free"
#define SEM_TABLE_COUNTER "/sem_table_counter"

#define SHARED_MEMO "/shared_memo"
#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5
struct timespec start;


enum pizza_type{
    Margherita = 0,
    Diavola = 1,
    Capricciosa = 2,
    Marinara = 3,
    Frutti_di_mare = 4,
    Boscaiola = 5,
    Vegetariana = 6,
    Quattro_fromaggi = 7,
    Quatro_stagioni = 8,
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


sem_t* open_sem(char* sem){
    sem_t* result;
    if((result = sem_open(sem, O_RDWR)) == SEM_FAILED){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}

void close_sem(sem_t* sem){
    if(sem_close(sem) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void unlink_sem(char* sem){
    if(sem_unlink(sem) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void create_sem(char* name, int val){
    sem_t* new_sem;
    if((new_sem = sem_open(name, O_CREAT | O_RDWR, 0666, val)) == SEM_FAILED){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void wait_sem(sem_t* sem){
    if(sem_wait(sem) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void post_sem(sem_t* sem){
    if(sem_post(sem) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

struct data* attach_memo(int fd){
    struct data* data;
    if((data = mmap(NULL, sizeof(struct data), PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return data;
}

void detach_memo(struct data* data){
    if(munmap(data,sizeof(struct data)) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int open_shm(){
    int fd;
    if((fd = shm_open(SHARED_MEMO, O_RDWR, 0666)) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}

int create_shm(){
    int fd;
    if((fd = shm_open(SHARED_MEMO, O_CREAT | O_RDWR, 0666)) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(ftruncate(fd,sizeof(struct data)) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}

void unlink_shm(){
    if(shm_unlink(SHARED_MEMO) == -1){
        fprintf(stderr,"%s\n", strerror(errno));
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

#endif //JANICKIJAKUB_SHARED_MEMORY_H
