//
// Created by ja on 5/8/21.
//
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include "shared_memory.h"

int shm_fd;
sem_t* sem_oven;
sem_t* sem_oven_free;
sem_t* sem_table;
sem_t* sem_table_free;
sem_t* sem_table_counter;


enum pizza_type new_pizza();
void to_oven(enum pizza_type pizza);
enum pizza_type from_oven();
void to_table(enum pizza_type pizza);
void handler(int sig);

int main(int argc, char* argv[]){
    signal(SIGINT, handler);
    shm_fd = open_shm();
    sem_oven = open_sem(SEM_OVEN);
    sem_oven_free = open_sem(SEM_OVEN_FREE);
    sem_table = open_sem(SEM_TABLE);
    sem_table_free = open_sem(SEM_TABLE_FREE);
    sem_table_counter = open_sem(SEM_TABLE_COUNTER);

    srand(getpid());

    enum pizza_type pizza;
    while (1){
        pizza = new_pizza();
        sleep(rand()%2 + 1);
        to_oven(pizza);
        sleep(rand()%2 + 4);
        pizza = from_oven();
        to_table(pizza);
    }
}

enum pizza_type new_pizza(){
    enum pizza_type pizza = rand()%10;
    printf("[%d %s C] Przygotowuje pizze: %u\n", getpid(), get_time(), pizza);
    return pizza;
}

void to_oven(enum pizza_type pizza_type){
    wait_sem(sem_oven_free);
    wait_sem(sem_oven);
    struct data* data = attach_memo(shm_fd);
    data->oven_idx = (data->oven_idx+1)%OVEN_CAPACITY;
    data->oven[data->oven_idx] = pizza_type;
    data->oven_counter++;
    printf("[%d %s C] Dodałem pizze %u Liczba pizz w piecu: %d \n",getpid(),get_time(), pizza_type, data->oven_counter);
    post_sem(sem_oven);
    detach_memo(data);
}

enum pizza_type from_oven(){
    wait_sem(sem_oven);
    struct data* data = attach_memo(shm_fd);
    enum pizza_type pizza_type = data->oven[data->oven_idx];
    data->oven_counter--;
    data->oven_idx = data->oven_idx == 0 ? OVEN_CAPACITY-1 : data->oven_idx-1;
    detach_memo(data);

    post_sem(sem_oven_free);
    post_sem(sem_oven);
    return pizza_type;
}

void to_table(enum pizza_type pizza){
    wait_sem(sem_table);
    wait_sem(sem_table_free);

    struct data* data = attach_memo(shm_fd);
    data->table_counter++;
    data->table_idx = (data->table_idx+1)% OVEN_CAPACITY;
    data->table[data->table_idx] = pizza;
    printf("[%d %s C] Liczba pizz w piecu: %d Liczba pizz na stole: %d \n", getpid(), get_time(), data->oven_counter, data->table_counter);
    post_sem(sem_table_counter);
    post_sem(sem_table);
    detach_memo(data);
}

void handler(int sig){
    close_sem(sem_oven);
    close_sem(sem_table);
    close_sem(sem_oven_free);
    close_sem(sem_table_free);
    close_sem(sem_table_counter);
    exit(EXIT_SUCCESS);
}

//Sprawdzic post i wait