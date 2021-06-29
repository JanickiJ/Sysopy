//
// Created by ja on 5/8/21.
//

#include <unistd.h>
#include "shared_memory.h"

int shm_id;
int sem_id;

enum pizza_type from_table();
void to_client(enum pizza_type pizza);


int main(int argc, char* argv[]){
    sem_id = sem_get(ftok(getenv("HOME"), 1));
    shm_id = shm_get(ftok(getenv("HOME"), 2));

    srand(getpid());
    enum pizza_type pizza;
    while (1){
        pizza = from_table();
        sleep(rand()%2 + 4);
        to_client(pizza);
        sleep(rand()%2 + 4);
    }
}

enum pizza_type from_table(){
    sem_use(sem_id,&sem_table_counter_minus,1);
    sem_use(sem_id,&sem_table_minus,1);

    struct data* data = shm_attach(shm_id);
    enum pizza_type pizza = data->table[data->table_idx];
    data->table_idx = data->table_idx == 0 ? TABLE_CAPACITY-1 : data->table_idx-1;
    data->table_counter --;
    printf("[%d %s S] Pobieram pizze: %u Liczba pizz na stole %d\n", getpid(), get_time(), pizza, data->table_counter);
    shm_detach(data);

    sem_use(sem_id,&sem_table_free_plus,1);
    sem_use(sem_id,&sem_table_plus,1);

    return pizza;
}

void to_client(enum pizza_type pizza){
    printf("[%d %s S] Dostarczam pizze: %u\n", getpid(), get_time(), pizza);
}
