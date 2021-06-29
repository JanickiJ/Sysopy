//
// Created by ja on 12.04.2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "header.h"

int main(int argc, char** argv){
    enum option option = assign_option(argv[1]);
    if (option != PENDING) {
        printf("[Child] Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
    }
    if (option == MASK || option == PENDING) {
        sigset_t pending;
        sigpending(&pending);
        printf("[Child]  SIGUSR1 is in pending signals: %s\n",
               sigismember(&pending, SIGUSR1) ? "true" : "false");
    }
    return 0;
}