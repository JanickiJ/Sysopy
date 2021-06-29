//
// Created by ja on 14.04.2021.
//
#include "header.h"


int main(int argc, char** argv){

    printf("---- TEST 1 SIGINFO SIGFPE ----\n");
    scenario_SIGINFO(handler_FPE, SIGFPE);
    if(fork() ==0){
        int zero =0;
        int error = 5/zero;
        printf("%d", error);
    } else{
        wait(NULL);
    }


    printf("\n---- TEST 2 SIGINFO SIGSEGV ----\n");
    scenario_SIGINFO(handler_SEGV, SIGSEGV);
    if(fork() == 0){
        int* mem = (int*) 2222;
        *mem = 22;
    } else{
        wait(NULL);
    }

    printf("\n---- TEST 3 SIGINFO SIGCHLD ----\n");
    scenario_SIGINFO(handler_CHILD, SIGCHLD);
    if(fork() == 0){
        _exit(0);
    } else{
        wait(NULL);
    }


    printf("\n---- TEST 4 NOCLDSTOP SIGCHLD ----\n");
    scenario_NOCLDSTOP();

    printf("\n---- TEST 5 RESETHAND SIGHILD ----\n");
    scenario_RESETHAND();


    return 0;
}

void scenario_SIGINFO(void(*handler)(), int signal){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    if(sigaction(signal,&act,NULL) == -1){
        fprintf(stderr, "Sigaction error: %s", strerror(errno));
    }
}


void handler_FPE(int sig_no, siginfo_t* info, void* ucontext){
    printf("Signal SIGFPE sig_no: %d\n", info->si_signo);
    printf("Parent's pid: %d , Child's id: %d\n", info->si_pid, info->si_uid);
    printf("Due to memory location: %p\n", info->si_addr);

    if(info->si_code == FPE_INTDIV) {
        printf("Signal code: %d - integer divided by zero\n", info->si_code);
    } else{
        printf("Signal code: %d \n", info->si_code);
    }
    _exit(0);
}

void handler_SEGV(int sig_no, siginfo_t* info, void* ucontext){
    printf("Signal SIGFPE sig_no: %d\n", info->si_signo);
    printf("Parent's pid: %d , Child's id: %d\n", info->si_pid, info->si_uid);
    printf("Due to memory location: %p\n", info->si_addr);

    if (info->si_code == SEGV_MAPERR) {
        printf("Signal code: %d - address not mapped\n", info->si_code);
    }else{
        printf("Signal code: %d \n", info->si_code);
    }
    _exit(0);
}

void handler_CHILD(int sig_no, siginfo_t* info, void* ucontext){
    printf("Signal SIGFPE sig_no: %d\n", info->si_signo);
    printf("Parent's pid: %d , Child's id: %d\n", info->si_pid, info->si_uid);
    printf("Due to memory location: %p\n", info->si_addr);

}

void scenario_NOCLDSTOP(){
    printf("-- NOCLDSTOP turn off --\n");
    struct sigaction act;
    act.sa_handler = &handler_NOCLDSTOP;
    sigemptyset(&act.sa_mask);
    if(sigaction(SIGCHLD,&act,NULL) == -1){
        fprintf(stderr, "Sigaction Error: %s", strerror(errno));
    }
    pid_t pid;
    if((pid = fork()) == 0){
        while(1){};
    }
    kill(pid, SIGSTOP);
    sleep(2);
    printf("--- NOCLDSTOP turn on ---\n");
    act.sa_flags = SA_NOCLDSTOP;
    if(sigaction(SIGCHLD,&act,NULL) == -1){
        fprintf(stderr, "Sigaction Error: %s", strerror(errno));
    }
    pid_t pid2;
    if((pid2 = fork()) == 0){
        while(1){};
    }
    kill(pid2, SIGSTOP);
    sleep(1);
}

void scenario_RESETHAND(){
    printf("-- RESETHAND turn off --\n");
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_handler = handler_RESETHEAD;

    if(sigaction(SIGCHLD,&act,NULL) == -1){fprintf(stderr, "Sigaction Error: %s", strerror(errno));}

    printf("Forking two processes\n");
    if (fork() == 0) {_exit(0);}
    wait(NULL);
    if (fork() == 0) {_exit(0);}
    wait(NULL);

    printf("-- RESETHAND turn on --\n");
    act.sa_flags = SA_RESETHAND;

    if(sigaction(SIGCHLD,&act,NULL) == -1){fprintf(stderr, "Sigaction Error: %s", strerror(errno));}

    printf("Forking two processes\n");
    if (fork() == 0) {_exit(0);}
    wait(NULL);
    if (fork() == 0) {_exit(0);}
    wait(NULL);
}


void handler_NOCLDSTOP(int signum){
    printf("Signal SIGCHLD parent pid: %d, child pid: %d\n", getpid(), getppid());
}

void handler_RESETHEAD(int signum){
    printf("Signal SIGCHLD parent pid: %d, child pid: %d\n", getpid(), getppid());
}