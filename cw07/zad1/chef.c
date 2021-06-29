//
// Created by ja on 5/8/21.
//
#include <unistd.h>
#include "shared_memory.h"

int shm_id;
int sem_id;

enum pizza_type new_pizza();
void to_oven(enum pizza_type pizza);
enum pizza_type from_oven();
void to_table(enum pizza_type pizza);

int main(int argc, char* argv[]){
    sem_id = sem_get(ftok(getenv("HOME"), 1));
    shm_id = shm_get(ftok(getenv("HOME"), 2));
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
    sem_use(sem_id,&sem_oven_free_minus,1);
    sem_use(sem_id,&sem_oven_minus,1);

    struct data* data = shm_attach(shm_id);
    data->oven_idx = (data->oven_idx+1)%OVEN_CAPACITY;
    data->oven[data->oven_idx] = pizza_type;
    data->oven_counter++;
    printf("[%d %s C] Dodałem pizze %u Liczba pizz w piecu: %d \n",getpid(),get_time(), pizza_type, data->oven_counter);
    shm_detach(data);

    sem_use(sem_id, &sem_oven_plus,1);
}

enum pizza_type from_oven(){
    sem_use(sem_id, &sem_oven_minus,1);

    struct data* data = shm_attach(shm_id);
    enum pizza_type pizza_type = data->oven[data->oven_idx];
    data->oven_counter--;
    data->oven_idx = data->oven_idx == 0 ? OVEN_CAPACITY-1 : data->oven_idx-1;
    shm_detach(data);

    sem_use(sem_id,&sem_oven_free_plus,1);
    sem_use(sem_id,&sem_oven_plus,1);
    return pizza_type;
}

void to_table(enum pizza_type pizza){
    sem_use(sem_id,&sem_table_minus,1);
    sem_use(sem_id,&sem_table_free_minus,1);

    struct data* data = shm_attach(shm_id);;
    data->table_counter++;
    data->table_idx = (data->table_idx+1)% OVEN_CAPACITY;
    data->table[data->table_idx] = pizza;
    printf("[%d %s C] Liczba pizz w piecu: %d Liczba pizz na stole: %d \n", getpid(), get_time(), data->oven_counter, data->table_counter);
    shm_detach(data);

    sem_use(sem_id,&sem_table_plus,1);
    sem_use(sem_id,&sem_table_counter_plus,1);
}


//Sprawdzic post i wait