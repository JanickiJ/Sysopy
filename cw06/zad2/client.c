//
// Created by ja on 4/28/21.
//

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "header.h"

int CLIENT_ID;
char* CLIENT_Q_NAME;
int IS_CONNECTED=0;
mqd_t CLIENT_Q;
mqd_t SERVER_Q;
mqd_t CONNECTED_Q;
int CONNECTED_ID;


void stop();
void connect_to_server();
void parse_input(char* line);
void send_message_to_sb(char* message);
void sigint_handler(int sig_no);
void signal_handler(int sig_no);
void disconnect(char* client_char_id);
void notification_set();

int main(){
    atexit(stop);
    srand(time(NULL));

    signal(SIGINT, sigint_handler);
    struct sigaction act;
    act.sa_flags = 0;
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

    char char_client_id[3];
    sprintf(char_client_id,"%d", CLIENT_ID);

    if(strcmp(option,"LIST") == 0){
        send_message(SERVER_Q,char_client_id,LIST);
    }
    else if(strcmp(option,"DISCONNECT") == 0){
        disconnect(char_client_id);
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
            char connect_to[8];
            sprintf(connect_to, "%d %d",CLIENT_ID, atoi(arg));
            send_message(SERVER_Q,connect_to,CONNECT);
        }
    }
    else{
        printf("Incorrect input: %s\n", line);
    }
}


void disconnect(char* client_char_id){
    printf("Disconnecting...");
    if(IS_CONNECTED != 0) {
        send_message(SERVER_Q,client_char_id,DISCONNECT);
        printf("Disconnected\n");
    } else{
        printf("Is already disconnected\n");
    }
    IS_CONNECTED = 0;
}

void notification_set(){
    struct sigevent s_sigevent;
    s_sigevent.sigev_signo = SIGRTMIN;
    s_sigevent.sigev_notify = SIGEV_SIGNAL;
    if (mq_notify(CLIENT_Q, &s_sigevent) == -1) { print_error(errno);}
}

void connect_to_server(){
    printf("Connecting to server...");

    CLIENT_Q_NAME = generate_q_name();
    CLIENT_Q = create_queue(CLIENT_Q_NAME);
    if ((SERVER_Q = mq_open(SERVER_Q_NAME, O_RDWR)) == -1){ print_error(errno);}

    unsigned int type = NEW;
    char msg[3];
    send_message(SERVER_Q,CLIENT_Q_NAME, type);
    get_message(CLIENT_Q,msg, &type);
    sscanf(msg, "%d", &CLIENT_ID);
    notification_set();
    printf("Connected successfully, id: %d\n", CLIENT_ID);
}


void stop(){
    char client_char_id[3];
    sprintf(client_char_id,"%d",CLIENT_ID);
    disconnect(client_char_id);

    send_message(SERVER_Q,client_char_id,STOP);

    if(mq_close(CLIENT_Q)){ print_error(errno);}
    if(mq_close(SERVER_Q)){ print_error(errno);}
    if(mq_unlink(CLIENT_Q_NAME)){ print_error(errno);}

    printf("Stopped\n");
}

void send_message_to_sb(char* message){
    if(IS_CONNECTED == 0){
        printf("You are not connected");
        return;
    }
    char helper[strlen(message)];
    sprintf(helper, "%s", message);
    send_message(CONNECTED_Q, message,MESSAGE);
}

void sigint_handler(int sig_no){
    exit(0);
}

void signal_handler(int sig_no){
    char* msg = calloc(MAX_MSG_LEN + 10, sizeof(char));
    unsigned int type;

    get_message(CLIENT_Q, msg, &type);

    if(type == STOP){
        exit(0);
    }
    if(type == CONNECT){
        if(atoi(msg) <0){
            printf("Can not connect\n");
            return;
        }
        char connected_char_name[20];
        int connected;
        sscanf(msg, "%d %s", &connected, connected_char_name);
        CONNECTED_ID = connected;
        if((CONNECTED_Q = mq_open(connected_char_name, O_WRONLY)) == -1){ print_error(errno);}
        IS_CONNECTED = 1;

        printf("Connected with: %d\n", CONNECTED_ID);
    }
    else if(type == DISCONNECT){
        if(IS_CONNECTED != 0) {
            if(mq_close(CONNECTED_Q) == -1){ print_error(errno);}
            printf("Disconnected\n");
        } else{
            printf("Is already disconnected\n");
        }
        IS_CONNECTED = 0;
    }
    else if(type == MESSAGE){
        char text[MAX_MSG_LEN];
        sscanf(msg, "%s", text);
        printf("[%d] %s\n", CONNECTED_ID, text);
    }
    notification_set();
}