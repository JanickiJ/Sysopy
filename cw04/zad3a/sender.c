#include "header.h"

void send();

void handler(int sig_no, siginfo_t* info, void* ucontext);

int main(int argc, char** argv){
    if (argc != 4){
        fprintf(stderr,"Not enough arguments");
        exit(1);
    }

    to_send = atoi(argv[2]);
    option = assign_option(argv[3]);
    catcher_pid = atoi(argv[1]);

    //Mask, start_signal, end_signal and sigaction setup
    config(handler);

    printf("\n---- TEST -- %s -- %s -- %s ----\n", argv[1], argv[2], argv[3]);
    printf("[Sender] starts sending\n");
    send();
    while(1){};

}

void send(){
    if (option == KILL || option == SIGRT) {
        for (int i = 0; i < to_send; i++) {
            kill(catcher_pid, sig_start);
        }
        kill(catcher_pid, sig_stop);
    } else {
        union sigval sigval;
        for (int i = 0; i < to_send; i++) {
            sigval.sival_int = i;
            sigqueue(catcher_pid, sig_start, sigval);
        }
        sigval.sival_int = to_send;
        sigqueue(catcher_pid, sig_stop, sigval);
    }
    printf("[Sender] end sending %d signals and end signal\n", to_send);
}

void handler(int sig_no, siginfo_t* info, void* ucontext){
    if(sig_no == sig_start){
        caught_sender ++;
        if(option == SIGQUEUE){
            printf("I receive signal (idx: %d)\n", info->si_value.sival_int);
        }
    } else{
        printf("[Sender] received %d signals and end signal.\n\n", caught_sender);
        exit(0);
    }
}

