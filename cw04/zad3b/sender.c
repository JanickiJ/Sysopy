//
// Created by ja on 15.04.2021.
//

#include "header.h"

void send_start_signal();

int main(int argc, char** argv){
    if (argc != 4){
        fprintf(stderr,"Not enough arguments");
        exit(1);
    }

    to_send = atoi(argv[2]);
    option = assign_option(argv[3]);
    catcher_pid = atoi(argv[1]);

    //Mask, start_signal, end_signal and sigaction setup
    config();

    printf("\n---- TEST -- %s -- %s -- %s ----\n", argv[1], argv[2], argv[3]);
    printf("[Sender] starts sending\n");
    send_signal(catcher_pid, sig_start);
    while(1){};

}


void handler(int sig_no, siginfo_t* info, void* ucontext){
    if(sig_no == sig_start || sig_no == SIGUSR1){
        if(caught_sender <= to_send){
            caught_sender ++;
            if (caught_sender == to_send){
                send_signal(catcher_pid, sig_stop);
            } else{
                send_signal(catcher_pid, sig_start);
            }
        }
    } else{
        printf("[Sender] received %d signals and end signal.\n\n", caught_sender);
        exit(0);
    }
}

