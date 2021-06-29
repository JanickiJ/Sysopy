//
// Created by ja on 14.04.2021.
//

#ifndef CW04_HEADER_H
#define CW04_HEADER_H

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void scenario_SIGINFO(void(*handler)(), int signal);
void handler_FPE(int sig_no, siginfo_t* siginfo, void* ucontext);
void handler_SEGV(int sig_no, siginfo_t* siginfo, void* ucontext);
void handler_CHILD(int sig_no, siginfo_t* siginfo, void* ucontext);
void handler_NOCLDSTOP(int signum);
void handler_RESETHEAD(int signum);
void scenario_NOCLDSTOP();
void scenario_RESETHAND();

#endif //CW04_HEADER_H
