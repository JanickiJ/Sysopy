//
// Created by ja on 13.04.2021.
//

#ifndef CW04_HADER_H
#define CW04_HADER_H

#include <string.h>

void handler(int signum){
    printf("Signal SIGUSR1 received by handler\n");
}

enum option{
    IGNORE,
    HANDLER,
    MASK,
    PENDING
};
enum function{
    FORK,
    EXEC
};

enum option assign_option(char* input){
    if(strcmp("ignore", input) == 0){
        return IGNORE;
    }
    else if(strcmp("handler", input) == 0){
        return HANDLER;
    }
    else if(strcmp("mask", input) == 0){
        return MASK;
    }
    else if(strcmp("pending", input) == 0){
        return PENDING;
    } else{
        fprintf(stderr,"Incorrect input: %s", input);
        exit(1);
    }
};


enum function assign_function(char* input){
    if(strcmp("fork", input) == 0){
        return FORK;
    }
    else if(strcmp("exec", input) == 0){
        return EXEC;
    }else{
        fprintf(stderr,"Incorrect input: %s", input);
        exit(1);
    }
}


#endif //CW04_HADER_H
