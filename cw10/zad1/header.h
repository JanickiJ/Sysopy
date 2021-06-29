//
// Created by ja on 5/30/21.
//

#ifndef CW10_HEADER_H
#define CW10_HEADER_H
#define MAX_LOGIN_LEN 50
#define MAX_MESSAGE_LEN 200
#define MAX_ARGS_LEN 50

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
#include <signal.h>


void print_error(char* name, int error){
    fprintf(stderr, "%s error: %s\n", name, strerror(error));
    exit(-1);
}


typedef struct client{
    char login[MAX_LOGIN_LEN];
    int fd;
    int opponent_fd;
    int opponent_id;
    int has_opponent;
    char mark;
    int is_responding;
    struct pollfd client_pollfd;
}client;

typedef enum msg{
    CONNECT = 1,
    DISCONNECT = 2,
    PING = 3,
    LOOSE = 4,
    DRAW =5,
    SAVE_CLIENT =7,
    SAVE_RESPONSE =8,
    NAME_TAKEN =9,
    SERVER_FULL = 10,
    MAKE_MARK =11,
    PASS = 13,
    MOVE = 14,
    WIN = 15,
    EXIT = 16,
}msg;


#endif //CW10_HEADER_H
