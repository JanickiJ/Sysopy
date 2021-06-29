//
// Created by ja on 15.04.2021.
//

#include "header.h"

int main(int argc,char** argv){
    if (argc != 2){
        fprintf(stderr,"Not enough arguments\n");
        exit(0);
    }
    option = assign_option(argv[1]);

    //Mask, start_signal, end_signal and sigaction setup
    config();

    while(1){};

}

void handler(int sig_no, siginfo_t* info, void* ucontext) {

    if (sig_no == sig_start || sig_no == SIGUSR1) {
        caught_catcher++;
        send_signal(info->si_pid, SIGUSR1);

    }else {
        printf("[Catcher] received %d signals and end signal\n", caught_catcher);
        send_signal(info->si_pid, sig_stop);
        exit(0);
    }
}