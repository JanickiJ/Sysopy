//
// Created by ja on 18.04.2021.
//

#ifndef CW05_HEADER_H
#define CW05_HEADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int MAX_NUM_COMPONENTS = 10;
int MAX_NUM_COMMANDS = 10;
int MAX_NUM_ARGS = 10;
int MAX_COMPONENTS_IN_LINE = 10;
int MAX_LEN_ARG = 10;
int MAX_COMPONENT_NAME = 20;

struct Command{
    char** args;
    int size;
};

struct Component{
    struct Command** commands;
    int size;
    char* name;
};

struct Task{
    struct Component** components;
    int size;
};

struct Task* create_task(){
    struct Task* task = malloc(sizeof(struct Task));
    task->components = malloc(sizeof(struct Component*) * MAX_NUM_COMPONENTS);
    task->size = 0;
    return task;
}

struct Component* create_component(){
    struct Component* component = malloc(sizeof(struct Component));
    component->commands = malloc(sizeof(struct Command*) * MAX_NUM_COMMANDS);
    component->size = 0;
    return component;
}

struct Command* create_command(){
    struct Command* command = malloc(sizeof(struct Command));
    command->args = malloc(sizeof(char*) * MAX_NUM_ARGS);
    for (int i = 0; i < MAX_NUM_ARGS; i++) {
        command->args[i] = NULL;
    }
    command->size = 0;
    return command;
}

struct Command* filled_command(char* line){
    struct Command* command = create_command();
    char* helper = strtok(line, " ");

    while(helper != NULL){
        if(command->size > MAX_NUM_ARGS){
            fprintf(stderr, "Too many arguments\n");
            exit(0);
        }
        command->args[command->size] = strdup(helper);
        command->size++;
        helper = strtok(NULL," ");
    }
    return command;
}

struct Component* filled_component(char* line){
    struct Component* component = create_component();
    char* copy;

    char* command = strtok_r(line, "=", &copy);
    component->name = strdup(command);

    while ((command = strtok_r(NULL, "|", &copy)) != NULL){
        if(component->size > MAX_NUM_COMMANDS){
            fprintf(stderr, "Too many commands\n");
            exit(0);
        }
        char* helper = strdup(command);

        component->commands[component->size] = filled_command(helper);
        component->size++;
        free(helper);
    }
    return component;
}


void add_component(char* line, struct Task* task){
    if(task->size > MAX_NUM_COMPONENTS){
        fprintf(stderr, "Too many commands\n");
        exit(0);
    }
    task->components[task->size] = filled_component(line);
    task->size++;
}

void free_task(struct Task* task){
    struct Command* command;
    struct Component* component;
    for (int i = 0; i < task->size; i++) {
        component = task->components[i];
        for (int j = 0; j < component->size; j++) {
            command = component->commands[j];
            for (int k = 0; k < command->size; k++) {
                free(command->args[k]);
            }
            free(component->commands[j]);
        }
        free(task->components[i]);
    }
    free(task);
}

int fill_component_arr(struct Component** components, struct Task* task, char* line){
    char *component_name  = strtok(line, " |");
    int component_index = 0;
    while (component_name != NULL) {
        int component_idx = -1;
        for (int i = 0; i < task->size; i++) {
            if (!strcmp(component_name, task->components[i]->name)) {
                component_idx = i;
                break;
            }
        }
        if (component_idx == -1) {
            fprintf(stderr, "Incorrect component name\n");
            return -1;
        }
        components[component_index] = task->components[component_idx];
        component_index++;
        component_name = strtok(NULL, " |");
    }
    return component_index;
}

void print_task(struct Task* task){
    printf("Print task\n");
    struct Command* command;
    struct Component* component;
    for (int i = 0; i < task->size; i++) {
        component = task->components[i];
        for (int j = 0; j < component->size; j++) {
            command = component->commands[j];
            for (int k = 0; k < command->size; k++) {
                printf("%s ",command->args[k]);
            }
            printf("|");
        }
        printf("\n");
    }
}



#endif //CW05_HEADER_H
