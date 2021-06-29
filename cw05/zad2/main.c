//
// Created by ja on 19.04.2021.
//
#define _XOPEN_SOURCE 700
#include <string.h>
#include <stdio.h>

#define MAX_COMMAND_LEN 256

enum mode {
    SENDER,
    DATE,
    SEND
};
enum mode assign_mode(int argc, char** argv);


int main(int argc, char** argv){
    enum mode mode = assign_mode(argc,argv);
    FILE* file;
    char copy[MAX_COMMAND_LEN];
    char* addressee;
    char* title;
    char* body;
    char* commands;
    if(mode == SEND){
        addressee = argv[1];
        title = argv[2];
        body = argv[3];
    }


    if(mode == DATE){
        commands = strdup("mail | sed '1d;$d' | sort -k5M -k6 -k7");
    }
    else if(mode == SENDER){
        commands = strdup("mail | sed '1d;$d' | sort -k3");

    } else{
        sprintf(copy, "echo '%s' | mail -s '%s' %s", body, title, addressee);
        commands = strdup(copy);
    }

    printf("%s\n", commands);
    if((file = popen(commands, "w")) == NULL){
        fprintf(stderr, "Popen error\n");
        return 1;
    }
    pclose(file);
    return 0;
}

enum mode assign_mode(int argc, char** argv){
    enum mode mode;
    int error =1;
    if(argc == 2) {
        if (!strcmp(argv[1], "data")) {
            error =0;
            mode = DATE;
        }
        else if(!strcmp(argv[1],"nadawca")){
            error=0;
            mode = SENDER;
        }
    }
    else if(argc == 4) {
        error =0;
        mode = SEND;
    }
    if(error){
        fprintf(stderr,"Incorrect mode");
        return 1;
    }

    return mode;
}
