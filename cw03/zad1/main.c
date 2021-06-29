#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if(!argv[1]){
        printf("Incorrect arguments\n");
        return -1;
    }
    printf("PID glownego programu: %d\n", (int)getpid());


    pid_t child_pid;
    int n=atoi(argv[1]);
    int status = 0;

    for (int i = 0; i < n; i++) {
        child_pid = vfork();
        if (child_pid == 0){
            printf("PID dziecka: %d\n", (int)getpid());
            _exit(status);
        }
    }

    return 0;
}
