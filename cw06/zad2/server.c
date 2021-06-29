//
// Created by ja on 4/28/21.
//



#include "header.h"
#define CLIENTS_MAX_NUM 6

int SERVER_Q;
struct client* CLIENTS[CLIENTS_MAX_NUM] = {NULL};

void stop_server();
void sigint_handler(int sig);
void stop_message(char* msg);
void connect_message(char* msg);
void list_message(char* msg);
void new_message(char* msg);
void disconnect_message(char* msg);

int main(){
    atexit(stop_server);
    SERVER_Q = create_queue(SERVER_Q_NAME);
    signal(SIGINT, &sigint_handler);
    printf("Server ready\n");

    while (1){
        char *msg = malloc(sizeof(char) * MAX_MSG_LEN);
        unsigned int type;
        get_message(SERVER_Q, msg,&type);

        if(type == STOP){
            stop_message(msg);
        }
        else if (type == CONNECT){
            connect_message(msg);
        }
        else if (type == LIST){
            list_message(msg);
        }
        else if (type == NEW){
            new_message(msg);
        }
        else if (type == DISCONNECT){
            disconnect_message(msg);
        }
    }
}

void stop_message(char* msg){
    int client_id = atoi(msg);
    struct client* client = CLIENTS[client_id];
    CLIENTS[client_id] = NULL;
    free(client);
    printf("Client %d stopped\n", client_id);
}

void connect_message(char* msg){
    int client_id1;
    int client_id2;
    sscanf(msg, "%d %d", &client_id1, &client_id2);
    struct client* client1 = CLIENTS[client_id1];
    struct client* client2 = CLIENTS[client_id2];

    if(client2 != NULL && client2->connected_client_id == -1){
        char n_msg1[8+MAX_Q_NAME_LEN];
        char n_msg2[8+MAX_Q_NAME_LEN];

        sprintf(n_msg1, "%d %s", client_id2, client2->client_q_name);
        sprintf(n_msg2, "%d %s", client_id1, client1->client_q_name);

        send_message(client2->client_q, n_msg2, CONNECT);
        send_message(client1->client_q, n_msg1, CONNECT);

        client1->connected_client_id = client2->client_id;
        client2->connected_client_id = client1->client_id;
        printf("Client %d and %d connected\n", client_id1, client_id2);
    } else{
        printf("Can not connect\n");
        send_message(client1->client_q, "-1", CONNECT);
    }
}

void list_message(char* msg){
    int client_id = atoi(msg);
    printf("Client %d can connect with:\n", client_id);
    for (int i = 0; i < CLIENTS_MAX_NUM; i++) {
        struct client* client = CLIENTS[i];
        if(client != NULL && client->client_id != client_id && client->connected_client_id == -1){
            printf("Client %d\n", client->client_id);
        }
    }
}

void new_message(char* msg){
    int i=0;
    for (i=0; i < CLIENTS_MAX_NUM; i++) {
        if(CLIENTS[i] == NULL){
            struct client* new_client= create_client();
            new_client->client_id = i;
            sscanf(msg, "%s", new_client->client_q_name);
            new_client->connected_client_id = -1;

            if((new_client->client_q = mq_open(new_client->client_q_name, O_RDWR)) == -1){ print_error(errno);}
            CLIENTS[i] =  new_client;

            char client_char_id[3];
            sprintf(client_char_id, "%d", i);
            send_message(new_client->client_q, client_char_id, NEW);

            printf("New client: %d\n", i);
            break;
        }
    }
    if(CLIENTS_MAX_NUM == i){
        printf("Two many clients\n");
        exit(-1);
    }
}

void disconnect_message(char* msg){
    int client_id = atoi(msg);
    struct client* client1 = CLIENTS[client_id];
    if(client1->connected_client_id != -1){
        struct client* client2 = CLIENTS[client1->connected_client_id];
        char* helper = "";
        send_message(client2->client_q,helper,DISCONNECT);

        client2->connected_client_id = -1;
        client1->connected_client_id = -1;
        printf("Clients %d and %d disconnected\n", client1->client_id, client2->client_id);
    } else{
        printf("Is already disconnected\n");
    }
}


void stop_server(){
    for (int i = 0; i < CLIENTS_MAX_NUM; i++) {
        struct client* client = CLIENTS[i];
        if(client != NULL){
            char* helper = "";
            send_message(client->client_q, helper, STOP);
        }
    }
    if (mq_close(SERVER_Q) == -1) { print_error(errno);}
    if (mq_unlink(SERVER_Q_NAME) == -1) {print_error(errno);}
    printf("Server stopped\n");
}

void sigint_handler(int sig){
    exit(0);
}