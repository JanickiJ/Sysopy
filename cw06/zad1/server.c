//
// Created by ja on 4/26/21.
//

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include<stdio.h>

#include "header.h"
#define CLIENTS_MAX_NUM 6

int SERVER_Q_ID;
struct client* CLIENTS[CLIENTS_MAX_NUM] = {NULL};

void stop_server();
void sigint_handler(int sig);
void stop_message(struct msg* msg);
void connect_message(struct msg* msg);
void list_message(struct msg* msg);
void new_message(struct msg* msg);
void disconnect_message(struct msg* msg);

int main(){
    atexit(stop_server);
    key_t key = ftok(getenv("HOME"),1);
    if ((SERVER_Q_ID = msgget(key, IPC_CREAT | 0666)) == -1){ print_error(errno);}

    signal(SIGINT, &sigint_handler);
    printf("Server ready\n");

    while (1){
        struct msg msg;
        get_message(SERVER_Q_ID,&msg);
        if(msg.msg_type == STOP){
            stop_message(&msg);
        }
        else if (msg.msg_type == CONNECT){
            connect_message(&msg);
        }
        else if (msg.msg_type == LIST){
            list_message(&msg);
        }
        else if (msg.msg_type == NEW){
            new_message(&msg);
        }
        else if (msg.msg_type == DISCONNECT){
            disconnect_message(&msg);
        }
    }
}

void stop_message(struct msg* msg){
    struct client* client = CLIENTS[msg->msg_sender_num];
    CLIENTS[msg->msg_sender_num] = NULL;
    free(client);
    printf("Client %d stopped\n", msg->msg_sender_num);
}

void connect_message(struct msg* msg){
    struct msg n_msg1;
    struct msg n_msg2;
    struct client* client1 = CLIENTS[msg->msg_sender_num];
    struct client* client2 = CLIENTS[atoi(msg->msg_spot)];

    n_msg1.msg_type = CONNECT;
    n_msg2.msg_type = CONNECT;
    if(client2 != NULL && client2->connected_client_id == -1){

        n_msg1.msg_sender = client2->client_pid;
        n_msg1.msg_sender_num = client2->client_id;
        sprintf(n_msg1.msg_spot, "%d", client2->client_q);

        n_msg2.msg_sender = client1->client_pid;
        n_msg2.msg_sender_num = client1->client_id;
        sprintf(n_msg2.msg_spot, "%d", client1->client_q);

        send_message(client2->client_q, &n_msg2);
        send_message(client1->client_q, &n_msg1);

        client1->connected_client_id = client2->client_id;
        client2->connected_client_id = client1->client_id;

        kill(client1->client_pid, SIGRTMIN);
        kill(client2->client_pid, SIGRTMIN);
    } else{
        printf("Can not connect\n");
        n_msg1.msg_sender_num =-1;
        send_message(client1->client_q, &n_msg1);
        kill(client1->client_pid, SIGRTMIN);
    }
}

void list_message(struct msg* msg){
    printf("Client %d can connect with:\n", msg->msg_sender_num);
    for (int i = 0; i < CLIENTS_MAX_NUM; i++) {
        struct client* client = CLIENTS[i];
        if(client != NULL && client->client_id != msg->msg_sender_num && client->connected_client_id == -1){
            printf("Client %d\n", client->client_id);
        }
    }
}

void new_message(struct msg* msg){
    int i=0;
    for (i=0; i < CLIENTS_MAX_NUM; i++) {
        if(CLIENTS[i] == NULL){
            struct client* new_client= calloc(1, sizeof(struct client));
            new_client->client_pid = msg->msg_sender;
            new_client->client_id = i;
            new_client->client_q = atoi(msg->msg_spot);
            new_client->connected_client_id = -1;
            CLIENTS[i] =  new_client;

            struct msg* n_msq = new_msg();
            sprintf(n_msq->msg_spot,"%d", i);
            n_msq->msg_type = NEW;
            send_message(new_client->client_q, n_msq);
            printf("New client: %d\n", i);
            break;
        }
    }
    if(CLIENTS_MAX_NUM == i){
        printf("Two many clients\n");
        exit(-1);
    }
}

void disconnect_message(struct msg* msg){
    struct client* client1 = CLIENTS[msg->msg_sender_num];
    if(client1->connected_client_id != -1){
        struct client* client2 = CLIENTS[client1->connected_client_id];
        struct msg* n_msq = new_msg();
        n_msq->msg_type = DISCONNECT;
        send_message(client2->client_q, n_msq);
        kill(client2->client_pid, SIGRTMIN);
        CLIENTS[client2->client_id]->connected_client_id =  -1;
        CLIENTS[msg->msg_sender_num]->connected_client_id = -1;
        printf("Clients %d and %d disconnected\n", client1->client_id, client2->client_id);
    } else{
        printf("Is already disconnected\n");
    }
}


void stop_server(){
    struct msg* msg = new_msg();
    msg->msg_type = STOP;
    for (int i = 0; i < CLIENTS_MAX_NUM; i++) {
        struct client* client = CLIENTS[i];
        if(client != NULL){
            send_message(client->client_q,msg);
            kill(client->client_pid, SIGRTMIN);
        }
    }
    if(msgctl(SERVER_Q_ID, IPC_RMID, NULL) == -1){ print_error(errno);}
    printf("Server stopped\n");
}

void sigint_handler(int sig){
    exit(0);
}