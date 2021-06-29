#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

void find(char* path, char* pattern, int depth){
    if(depth < 0){
        return;
    }

    DIR* dir = opendir(path);
    struct dirent *d;

    while ((d = readdir(dir)) != NULL) {
        if (strcmp(d->d_name, ".") == 0) continue;
        if (strcmp(d->d_name, "..") == 0) continue;

        char *new_path = calloc(PATH_MAX, sizeof(char));
        snprintf(new_path, PATH_MAX, "%s/%s", path, d->d_name);

        if (d->d_type == DT_DIR) {
            if (fork() == 0){
                find(new_path, pattern, depth-1);
                exit(0);
            } else{
                wait(NULL);
            }

        } else{
            char* grep = "grep -c ";
            int max_len = strlen(grep) + strlen(" \"") + strlen(pattern) + strlen("\" ") + strlen(new_path);
            char *command = malloc(max_len * sizeof(char));
            snprintf(command, max_len, "%s\"%s\" %s",grep, pattern, new_path);
            FILE *cmd=popen(command, "r");

            char c[1];
            if((fread(c, sizeof(char), 1, cmd)>0)) {
                if(atoi(c) >0){
                    printf("%s --> (PID: %d)\n", new_path, getpid());
                }
            }
        }
    }
}

int main(int argc, char** argv) {
   if (!argv[1] || !argv[2] || ! argv[3]){
        printf("Incorrect arguments");
        exit(0);
    }

    find(argv[1], argv[2], atoi(argv[3]));
}
