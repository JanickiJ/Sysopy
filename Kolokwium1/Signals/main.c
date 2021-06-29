#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void sighandler(int sig, siginfo_t* info, void* ucontext){
    printf("SIG: %d VAL: %d\n", sig, info->si_value.sival_int);
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;

    //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2
    //zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal
    //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci

    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGUSR1, &action, NULL) == -1){
        fprintf(stderr, "%s\n", strerror(errno));
        exit(-1);
    }
    if(sigaction(SIGUSR2, &action, NULL) == -1){
        fprintf(stderr, "%s\n", strerror(errno));
        exit(-1);
    }

    sigset_t set;
    sigfillset(&set);
    sigdelset(&set,SIGUSR1);
    sigdelset(&set,SIGUSR2);
    if(sigprocmask(SIG_SETMASK,&set,NULL)==-1){
        fprintf(stderr, "%s\n", strerror(errno));
        exit(-1);
    }



    int child = fork();
    if(child == 0) {
        sleep(1);
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        union sigval sval;
        sval.sival_int = atoi(argv[1]);
        if(sigqueue(child, atoi(argv[2]), sval) == -1){
            fprintf(stderr, "%s\n", strerror(errno));
            exit(-1);
        }
    }

    return 0;
}
