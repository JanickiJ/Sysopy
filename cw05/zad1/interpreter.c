#include "header.h"

#define exec execvp(component->commands[j]->args[0], component->commands[j]->args)


int main(int argc, char** argv){
    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments\n");
        return -1;
    }
    char* file_name = argv[1];

    FILE* file_commands;
    if(!(file_commands = fopen(file_name, "r"))){
        fprintf(stderr,"Incorrect path\n");
        return -1;
    }

    struct Task* task = create_task();
    size_t line_len=0;
    char* line;
    struct Component* components[MAX_COMPONENTS_IN_LINE];
    int components_index;
    int curr[2], prev[2];
    struct Component* component;
    int processes;


    while(getline(&line, &line_len, file_commands) != -1) {
        printf("\nCommand: %s", line);
        line = strtok(line, "\n");

        if (line == NULL) {
            printf("Empty line\n");
            continue;
        } else if (strchr(line, '=') != NULL) {
            add_component(line, task);
            printf("Inserted to structure\n");
        } else {
            components_index = fill_component_arr(components,task,line);

            processes = 0;
            int j;
            for (int i = 0; i < components_index; i++) {
                component = components[i];

                for (j = 0; j < component->size; j++) {

                    if(pipe(curr) == -1){
                        perror("Pipe error");
                        exit(0);
                    }

                    if (!(i == components_index - 1 && j == component->size - 1)){
                            if (fork() == 0) {
                                if (!(i == 0 && j == 0)) {
                                    dup2(prev[0], STDIN_FILENO);
                                    close(prev[1]);
                                }
                                dup2(curr[1], STDOUT_FILENO);

                                if (exec == -1) { exit(0); }
                            }

                            close(curr[1]);
                            prev[0] = curr[0];
                            prev[1] = curr[1];

                    }else{
                        if (fork() == 0) {
                            close(prev[1]);
                            dup2(prev[0], STDIN_FILENO);

                            if (exec == -1) {exit(0);}
                        }
                    }
                    processes++;
                }
            }
            for (int i = 0; i < processes; i++) {
                wait(NULL);
            }
        }
    }
    fclose(file_commands);
    free_task(task);
    printf("\n");
    return 0;
}

