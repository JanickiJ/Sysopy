//
// Created by ja on 15.04.2021.
//

#ifndef CW04_HEADER_H
#define CW04_HEADER_H
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>




enum option{
    KILL,
    SIGQUEUE,
    SIGRT
};


int sig_start;
int sig_stop;
int to_send;
pid_t catcher_pid;
int caught_catcher =0;
int caught_sender =0;
enum option option;
void handler(int sig_no, siginfo_t* info, void* ucontext);

enum option assign_option(char* input){
    if(strcmp("kill", input) == 0){
        return KILL;
    }
    else if(strcmp("sigqueue", input) == 0){
        return SIGQUEUE;
    }
    else if(strcmp("sigrt", input) == 0){
        return SIGRT;
    }
    else{
        fprintf(stderr,"Incorrect input: %s", input);
        exit(1);
    }
}

void config(){
    sigset_t mask;
    sigfillset(&mask);
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;

    if (option == SIGRT){
        sig_start = SIGRTMIN + 1;
        sig_stop = SIGRTMIN + 2;
    }
    else{
        sig_start = SIGUSR1;
        sig_stop = SIGUSR2;
    }
    sigdelset(&mask, sig_start);
    sigdelset(&mask, sig_stop);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, sig_stop);
    sigaddset(&act.sa_mask, sig_start);
    sigaction(sig_stop, &act, NULL);
    sigaction(sig_start, &act, NULL);
}


#endif //CW04_HEADER_H
