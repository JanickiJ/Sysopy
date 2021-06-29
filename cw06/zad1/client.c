//
// Created by ja on 4/26/21.
//


#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "header.h"

int CLIENT_ID;
int IS_CONNECTED=0;
int SERVER_Q_ID;
int CLIENT_Q_ID;
int CONNECTED_Q_ID;
int CONNECTED_CLIENT_ID;
int CONNECTED_CLIENT;

void stop();
void connect_to_server();
void parse_input(char* line);
void disconnect();
void send_message_to_sb(char* message);
void sigint_handler(int sig_no);
void signal_handler(int sig_no);

int main(){
    atexit(stop);
    signal(SIGINT, sigint_handler);
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGRTMIN, &act, NULL);

    connect_to_server();
    char input[MAX_MSG_LEN];
    while (1){
        if(fgets(input,MAX_MSG_LEN, stdin)){
            parse_input(input);
        }
    }

}

void parse_input(char* line){
    char* option;
    char* arg;
    option = strtok_r(line, " \n",&arg);
    struct msg msg;
    msg.msg_sender = getpid();
    msg.msg_sender_num = CLIENT_ID;

    if(strcmp(option,"LIST") == 0){
        msg.msg_type = LIST;
        send_message(SERVER_Q_ID, &msg);
    }
    else if(strcmp(option,"DISCONNECT") == 0){
        disconnect();
    }
    else if(strcmp(option,"STOP") == 0){
        exit(0);
    }
    else if(strcmp(option,"MESSAGE") == 0){
        send_message_to_sb(arg);
    }
    else if(strcmp(option,"CONNECT")==0){
        printf("Connecting...");
        if(IS_CONNECTED !=0){printf("You are already connected\n");}
        else {
            msg.msg_type = CONNECT;
            sprintf(msg.msg_spot, "%d", atoi(arg));
            send_message(SERVER_Q_ID, &msg);
        }
    }
    else{
        printf("Incorrect input: %s\n", line);
    }
}


void disconnect(){
    printf("Disconnecting...");
    if(IS_CONNECTED != 0) {
        struct msg* msg = new_msg();
        msg->msg_type = DISCONNECT;
        msg->msg_sender = getppid();
        msg->msg_sender_num = CLIENT_ID;
        send_message(SERVER_Q_ID, msg);
        printf("Disconnected\n");
    } else{
        printf("Is already disconnected\n");
    }
    IS_CONNECTED = 0;
}

void connect_to_server(){
    printf("Connecting to server...");
    key_t key = ftok(getenv("HOME"),1);
    SERVER_Q_ID = msgget(key,0);

    key = ftok(getenv("HOME"), getpid());
    CLIENT_Q_ID =msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    struct msg msg;
    msg.msg_type = NEW;
    msg.msg_sender = getpid();
    sprintf(msg.msg_spot, "%d", CLIENT_Q_ID);
    send_message(SERVER_Q_ID, &msg);
    get_message(CLIENT_Q_ID, &msg);
    CLIENT_ID = atoi(msg.msg_spot);

    printf("Connected successfully, id: %d\n", CLIENT_ID);
}


void stop(){
    struct msg* msg = new_msg();
    msg->msg_type = STOP;
    msg->msg_sender = getpid();
    msg->msg_sender_num = CLIENT_ID;
    disconnect();
    send_message(SERVER_Q_ID, msg);

    if(msgctl(CLIENT_Q_ID, IPC_RMID, NULL) == -1){ print_error(errno);}
    printf("Stopped\n");
}

void send_message_to_sb(char* message){
    if(IS_CONNECTED == 0){
        printf("You are not connected");
        return;
    }
    struct msg msg;
    msg.msg_sender = getpid();
    msg.msg_sender_num = CLIENT_ID;
    msg.msg_type = MESSAGE;
    sprintf(msg.msg_spot, "%s",message);
    send_message(CONNECTED_Q_ID, &msg);
    kill(CONNECTED_CLIENT, SIGRTMIN);
}

void sigint_handler(int sig_no){
    exit(0);
}

void signal_handler(int sig_no){
    struct msg* msg = new_msg();
    get_message(CLIENT_Q_ID,msg);
    if(msg->msg_type == STOP){
        exit(0);
    }
    if(msg->msg_type == CONNECT){
        if(msg->msg_sender_num <0){
            printf("Can not connect\n");
            return;
        }
        CONNECTED_CLIENT_ID = msg->msg_sender_num;
        CONNECTED_CLIENT = msg->msg_sender;
        CONNECTED_Q_ID = atoi(msg->msg_spot);
        IS_CONNECTED = 1;
        printf("Connected with: %d\n", CONNECTED_CLIENT_ID);
    }
    else if(msg->msg_type == DISCONNECT){
        IS_CONNECTED = 0;
        printf("Disconnected\n");
    }
    else if(msg->msg_type == MESSAGE){
        printf("[%d] %s", msg->msg_sender_num, msg->msg_spot);
    }
    else if(msg->msg_type == NEW){
        CLIENT_ID = atoi(msg->msg_spot);
    }
}