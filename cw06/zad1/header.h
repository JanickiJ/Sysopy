//
// Created by ja on 4/26/21.
//


#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

#define MAX_MSG_LEN 512

enum operation{
    LIST =1,
    CONNECT =2,
    DISCONNECT=3,
    STOP=4,
    MESSAGE=5,
    NEW = 6
};

struct msg {
    long                  msg_type;
    char                  msg_spot[MAX_MSG_LEN];
    time_t                msg_stime;  /* czas wyslania tego komunikatu */
    short                 msg_ts;     /* dlugosc wlasciwej tresci komunikatu */
    pid_t                 msg_sender;
    int                   msg_sender_num;
};

struct msg* new_msg(){
    struct msg* msg = malloc(sizeof(struct msg));
    return msg;
}

struct client{
    int client_id;
    pid_t client_pid;
    int client_q;
    int connected_client_id;
};
#define MAX_MESSAGE_SIZE sizeof(struct msg) - sizeof(long)


void print_error(int error){
    fprintf(stderr, "Error: %s\n", strerror(error));
    exit(-1);
}

void send_message(int q_id, struct msg* msg) {
    if (msgsnd(q_id, msg, MAX_MESSAGE_SIZE, 0) == -1) {print_error(errno);}
}

void get_message(int q_id, struct msg* msg) {
    if (msgrcv(q_id, msg, MAX_MESSAGE_SIZE, -10, 0) == -1) {print_error(errno);}
}


#endif