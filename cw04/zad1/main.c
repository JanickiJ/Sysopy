//
// Created by ja on 12.04.2021.
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "header.h"

int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr,"Not enough arguments");
        exit(1);
    }

    enum option option = assign_option(argv[1]);
    enum function function = assign_function(argv[2]);
    struct sigaction act;
    act.sa_flags = 0;

    if(option == IGNORE){
        act.sa_handler = SIG_IGN;
        sigemptyset(&act.sa_mask);
        if(sigaction(SIGUSR1, &act, NULL) == -1){
            fprintf(stderr, "sigaction error: %s\n", strerror(errno));
        }
    }
    else if(option == HANDLER){
        act.sa_handler = handler;
        sigemptyset(&act.sa_mask);
        if(sigaction(SIGUSR1, &act, NULL) == -1){
            fprintf(stderr, "sigaction error: %s\n", strerror(errno));
        }
    }
    else{
        sigset_t sig;
        sigemptyset(&sig);
        sigaddset(&sig, SIGUSR1);
        if(sigprocmask(SIG_BLOCK, &sig, NULL) == -1){
            fprintf(stderr, "sigaction error: %s\n", strerror(errno));
        }
    }

    printf("[Parent] Sending SIGUSR1 signal\n");
    raise(SIGUSR1);

    if(option == MASK || option == PENDING){
        sigset_t pending;
        sigpending(&pending);
        printf("[Parent] SIGUSR1 is in pending signals: %s\n", sigismember(&pending, SIGUSR1) ? "true" : " false");
    }

    if(function == FORK) {
        pid_t pid;
        if ((pid = fork()) == 0) {
            if (option != PENDING) {
                printf("[Child]  Sending SIGUSR1 signal\n");
                raise(SIGUSR1);
            }
            if (option == MASK || option == PENDING) {
                sigset_t pending;
                sigpending(&pending);
                printf("[Child]  SIGUSR1 is in pending signals: %s\n", sigismember(&pending, SIGUSR1) ? "true" : " false");
            }
            return 0;
        }
        wait(NULL);
    }
    else{
        execl("./exec", "./exec",argv[1],NULL);
    }
    return 0;
}
