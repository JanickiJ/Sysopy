#include <stdio.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define ELF_NUM 10
#define REINDEER_NUM 10
#define ELF_NUM_TO_WAKE_UP 3
#define REINDEER_NUM_TO_WAKE_UP 3
#define JOBS_TO_DONE 10
#define DELIVERS_TO_DELIVER 3

int delivers_done =0;
int reindeer_waiting=0;
int jobs_done =0;
int is_santa_sleeping =1;
int elves_waiting =0;
int elves_waiting_id[3];

pthread_mutex_t reindeer_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_sleeping_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_sleeping_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_can_help = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_can_deliver = PTHREAD_COND_INITIALIZER;
pthread_cond_t need_santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_waiting_cond = PTHREAD_COND_INITIALIZER;


void santa_fun();
void elf_fun(int* id);
void clean_cond();
void clean_mutexes();
void reindeer_fun(int* id);

int main() {

    pthread_t santa;
    pthread_t* elves = malloc(ELF_NUM * sizeof(pthread_t));
    pthread_t* reindeer = malloc(ELF_NUM * sizeof(pthread_t));
    int* reindeer_id = malloc(sizeof(int)*REINDEER_NUM);
    int* elf_id = malloc(sizeof(int)*ELF_NUM);


    pthread_create(&santa, NULL, (void *(*)(void *))santa_fun, NULL);
    for (int i = 0; i < ELF_NUM; i++) {
        elf_id[i] = i;
        pthread_create(&elves[i], NULL, (void *(*)(void *)) elf_fun, &elf_id[i]);
    }
    for (int i = 0; i < REINDEER_NUM; i++) {
        reindeer_id[i] = i;
        pthread_create(&reindeer[i], NULL, (void *(*)(void *)) reindeer_fun, &reindeer_id[i]);
    }


    for (int i = 0; i < ELF_NUM; i++) {
        pthread_join(elves[i], NULL);
    }
    for (int i = 0; i < REINDEER_NUM; i++) {
        pthread_join(reindeer[i], NULL);
    }
    pthread_join(santa, NULL);


    free(reindeer);
    free(elves);
    free(elf_id);
    free(reindeer_id);
    clean_cond();
    clean_mutexes();

    return 0;
}


void santa_fun(){
    srand(getpid());
    while (delivers_done < DELIVERS_TO_DELIVER || jobs_done < JOBS_TO_DONE ){
        pthread_mutex_lock(&santa_sleeping_mutex);
        is_santa_sleeping = 1;
        printf("Miko??aj: zasypiam\n");
        pthread_cond_broadcast(&santa_sleeping_cond);
        pthread_cond_wait(&need_santa_cond,&santa_sleeping_mutex);
        printf("Miko??aj: budz?? si??\n");
        is_santa_sleeping =0;
        pthread_mutex_unlock(&santa_sleeping_mutex);

        pthread_mutex_lock(&reindeer_waiting_mutex);
        if(reindeer_waiting >= REINDEER_NUM_TO_WAKE_UP){
            pthread_cond_broadcast(&santa_can_deliver);
            pthread_mutex_unlock(&reindeer_waiting_mutex);
            printf("Mikolaj: dostarczam zabawki \n");
            sleep(rand()%3 +2);
            delivers_done++;
        }
        else{
            pthread_mutex_unlock(&reindeer_waiting_mutex);
            pthread_mutex_lock(&elves_waiting_mutex);
            if (ELF_NUM_TO_WAKE_UP <= elves_waiting) {
                pthread_cond_broadcast(&santa_can_help);
                jobs_done++;
                printf("Miko??aj: rozwi??zuje problemy elf??w %d %d %d\n", elves_waiting_id[0],
                                                                               elves_waiting_id[1],
                                                                               elves_waiting_id[2]);
                sleep(rand() % 2 + 1);
                elves_waiting = 0;
                pthread_cond_broadcast(&elves_waiting_cond);
                pthread_mutex_unlock(&elves_waiting_mutex);
            }
        }
    }

    printf("Rozwiazano %d problem??w\n",JOBS_TO_DONE);
    printf("Dostarczono %d dostaw\n",DELIVERS_TO_DELIVER);
    printf("KONIEC PRACY MIKO??AJA\n");

    pthread_exit((void *)0);
}


void reindeer_fun(int* id){
    srand(*id);
    while(delivers_done < DELIVERS_TO_DELIVER){
        sleep((rand() % 6) +5);
        pthread_mutex_lock(&reindeer_waiting_mutex);
        reindeer_waiting++;
        printf("[%d]Renifer: czeka %d reniferow na Mikolaja \n", *id, reindeer_waiting);
        if(reindeer_waiting == REINDEER_NUM_TO_WAKE_UP){
            pthread_mutex_unlock(&reindeer_waiting_mutex);
            pthread_mutex_lock(&santa_sleeping_mutex);
            while (is_santa_sleeping == 0) pthread_cond_wait(&santa_sleeping_cond, &santa_sleeping_mutex);

            printf("[%d] Renifer: wybudzam Mikolaja\n", *id);
            pthread_cond_broadcast(&need_santa_cond);
            pthread_mutex_unlock(&santa_sleeping_mutex);
            pthread_mutex_lock(&reindeer_waiting_mutex);
        }
        pthread_cond_wait(&santa_can_deliver, &reindeer_waiting_mutex);
        reindeer_waiting--;
        pthread_mutex_unlock(&reindeer_waiting_mutex);
        sleep(rand()%3 +2);
    }
}

void elf_fun(int* id){
    srand(*id);
    while (jobs_done < JOBS_TO_DONE){
        sleep(rand()%4 + 2);
        pthread_mutex_lock(&elves_waiting_mutex);
        while(elves_waiting >=3 ){
            printf("[%d] Elf: czeka na powr??t elf??w\n",*id);
            pthread_cond_wait(&elves_waiting_cond,&elves_waiting_mutex);
        }
        if(jobs_done == JOBS_TO_DONE){
            pthread_mutex_unlock(&elves_waiting_mutex);
            pthread_exit((void*)0);
        }
        if(elves_waiting<3){
            elves_waiting_id[elves_waiting] = *id;
            elves_waiting++;
            printf("[%d] Elf: czeka %d elf??w na Miko??aja\n",*id, elves_waiting);
            if(elves_waiting == 3){
                pthread_mutex_unlock(&elves_waiting_mutex);

                pthread_mutex_lock(&santa_sleeping_mutex);
                pthread_mutex_lock(&reindeer_waiting_mutex);
                while(is_santa_sleeping == 0 || reindeer_waiting == REINDEER_NUM_TO_WAKE_UP){
                    pthread_mutex_unlock(&reindeer_waiting_mutex);
                    pthread_cond_wait(&santa_sleeping_cond, &santa_sleeping_mutex);
                    pthread_mutex_lock(&reindeer_waiting_mutex);
                }
                printf("[%d] Elf: wybudzam Miko??aja\n", *id);
                pthread_cond_broadcast(&need_santa_cond);
                pthread_mutex_lock(&elves_waiting_mutex);
                pthread_mutex_unlock(&santa_sleeping_mutex);
                pthread_mutex_unlock(&reindeer_waiting_mutex);
            }
            pthread_cond_wait(&santa_can_help, &elves_waiting_mutex);
            pthread_mutex_unlock(&elves_waiting_mutex);
        }
    }
    pthread_exit((void*) 0);
}


void clean_cond(){
    pthread_cond_destroy(&need_santa_cond);
    pthread_cond_destroy(&elves_waiting_cond);
    pthread_cond_destroy(&santa_can_help);
    pthread_cond_destroy(&santa_sleeping_cond);
    pthread_cond_destroy(&santa_can_deliver);
}

void clean_mutexes(){
    pthread_mutex_destroy(&elves_waiting_mutex);
    pthread_mutex_destroy(&santa_sleeping_mutex);
    pthread_mutex_destroy(&reindeer_waiting_mutex);
}