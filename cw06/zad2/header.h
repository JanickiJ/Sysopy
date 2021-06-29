//
// Created by ja on 4/28/21.
//

#ifndef CW06_HEADER_H
#define CW06_HEADER_H


#include <stdio.h>
#include <sys/msg.h>
#include <errno.h>
#include <mqueue.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define MAX_MSG_LEN 512
#define MAX_Q_NAME_LEN 15
#define MAX_NUM_OF_MSG 10

#define SERVER_Q_NAME "/server_q_name"

enum operation{
    LIST =1,
    CONNECT =2,
    DISCONNECT=3,
    STOP=4,
    MESSAGE=5,
    NEW = 6
};

struct client{
    int client_id;
    mqd_t client_q;
    char* client_q_name;
    int connected_client_id;
};

/*struct mq_attr {
    long mq_flags;    *//* sygnalizator kolejki: 0, O_NONBLOCK *//*
    long mq_maxmsg;   *//* maksymalna liczba komunikatów w kolejce *//*
    long mq_msgsize;  *//* maksymalny rozmiar komunikatu (w bajtach)  *//*
    long mq_curmsgs;   *//* liczba komunikatów w kolejce *//*
};*/

struct client* create_client(){
    struct client* new_client= calloc(1, sizeof(struct client));
    new_client->client_q_name = calloc(MAX_Q_NAME_LEN,sizeof(char));
    return new_client;
}

void print_error(int error){
    fprintf(stderr, "Error: %s\n", strerror(error));
    exit(-1);
}

void send_message(mqd_t q_id, char* msg, unsigned int type) {
    if(mq_send(q_id,msg, strlen(msg), type) == -1) {print_error(errno);}
}

void get_message(mqd_t q_id, char* msg, unsigned int* type) {
    if(mq_receive(q_id, msg, MAX_MSG_LEN,type) == -1) {print_error(errno);}
}


char* generate_q_name() {
    const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char* name = calloc(sizeof(char), (MAX_Q_NAME_LEN+1));
    sprintf(name, "/");
    for (int i = 1; i < MAX_Q_NAME_LEN; i++) {
        name[i] = alphabet[rand() % (strlen(alphabet) - 1)];
    }
    name[MAX_Q_NAME_LEN] = '\0';
    return name;
}


mqd_t create_queue(char *name) {
    mqd_t new_q;
    struct mq_attr mq_attr;
    mq_attr.mq_maxmsg = MAX_NUM_OF_MSG;
    mq_attr.mq_curmsgs = 0;
    mq_attr.mq_flags = 0;
    mq_attr.mq_msgsize = MAX_MSG_LEN;
    if ((new_q = mq_open(name, O_RDWR | O_CREAT, 0666, &mq_attr)) == -1) {print_error(errno);}
    return new_q;
}

#endif //CW06_HEADER_H
