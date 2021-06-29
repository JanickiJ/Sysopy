#include "header.h"

int main(int argc,char** argv){
    if (argc != 2){
        fprintf(stderr,"Not enough arguments\n");
        exit(0);
    }

    option = assign_option(argv[1]);

    config();

    while(1){};

}

void handler(int sig_no, siginfo_t* info, void* ucontext){
    if(sig_no == sig_start){
        caught_catcher++;
    } else {
        printf("[Catcher] received %d signals and end signal. Beginning sending back\n", caught_catcher);
        pid_t sender_pid = info->si_pid;

        if (option == KILL || option == SIGRT) {
            for (int i = caught_catcher; i > 0; i--) {
                kill(sender_pid, sig_start);
            }
            kill(sender_pid, sig_stop);
        } else {
            union sigval sigval;
            for (int i = 0; i < caught_catcher; i++) {
                sigval.sival_int = i;
                sigqueue(sender_pid, sig_start, sigval);
            }
            sigval.sival_int = caught_catcher;
            sigqueue(sender_pid, sig_stop, sigval);
        }
        printf("[Catcher] end sending %d signals and end signal\n", caught_catcher);
        exit(0);
    }
}