//
// Created by ja on 5/17/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#define min(a, b) ((a < b) ? a : b)
unsigned char** image;
unsigned char** image_result;

int width;
int height;
int n_threads;
void load_image(FILE* file);
void free_image(unsigned char** img);
unsigned char** image_init();
void * parse_option(char*);
double* numbers(const int* idx);
double* block(int* i);
void write_output(FILE* file);

int main(int argc, char** argv){
    if(argc != 5){
        fprintf(stderr,"Incorrenct imput\n");
        return EXIT_FAILURE;
    }
    n_threads = atoi(argv[1]);
    double* (*f)(int *);
    f = parse_option(argv[2]);
    FILE* file_in = fopen(argv[3], "r");
    FILE* file_out = fopen(argv[4], "w");

    if(file_in == NULL || file_out == NULL){
        fprintf(stderr,"Incorrenct TTimput\n");
        return EXIT_FAILURE;
    }
    load_image(file_in);

    pthread_t *threads = malloc(n_threads* sizeof(pthread_t));
    int* args = malloc(n_threads * sizeof(int));

    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < n_threads; i++) {
        args[i] = i;
        if ((pthread_create(&threads[i], NULL, (void *(*)(void *)) f, &args[i])) == -1){
            fprintf(stderr, "Error: %s", strerror(errno));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < n_threads; i++) {
        double* time = NULL;
        if((pthread_join(threads[i], (void **) &time)) == -1){
            fprintf(stderr, "Error: %s", strerror(errno));
            return EXIT_FAILURE;
        }
        printf("[%d]     took [%f mics]\n", i, *time);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time = (end.tv_sec - start.tv_sec)* 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
    printf("[total] took [%f mics]\n\n", time);
    write_output(file_out);
    fclose(file_in);
    fclose(file_out);
    free_image(image);
    free_image(image_result);
}

void * parse_option(char* arg){
    double* (*f)(int *);
    if(strcmp(arg,"block") == 0){
        f = (double *(*)(int *)) block;
    }
    else if(strcmp(arg,"numbers") == 0){
        f = (double *(*)(int *)) numbers;
    } else{
        fprintf(stderr, "Incorrect option");
        exit(EXIT_FAILURE);
    }
    return f;
}

void load_image(FILE* file){
    int M;
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, file);
    getline(&line, &len, file);

    fscanf(file, "%d %d\n%d\n", &width,&height,&M);
    if(M>255){
        fprintf(stderr,"Incorrenct M");
        exit(EXIT_FAILURE);
    }

    image = image_init();
    image_result = image_init();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file,"%hhu",&image[i][j]);
        }
    }
}

double* block(int* idx){
    int i_thread = idx[0];
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int round = width % n_threads == 0 ? 0 : 1;
    int down = i_thread * (height / n_threads) + round;
    int up = (i_thread + 1) * (height / n_threads)+ round;
    for (int i = down; i < min(up,height); i++) {
        for (int j = 0; j < width; j++) {
            image_result[i][j] = 255 - image[i][j];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double* time = calloc(1,sizeof(double ));
    *time = (end.tv_sec - start.tv_sec)* 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
    return time;
}

double* numbers(const int* idx){
    int i_thread = *idx;
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int round = 256 % n_threads == 0 ? 0 : 1;
    int down = i_thread * (256 / n_threads) + round;
    int up = (i_thread + 1) * (256 / n_threads) + round;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if(image[i][j] >= down && image[i][j] <= up){
                image_result[i][j] = 255 - image[i][j];
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double* time = calloc(1,sizeof(double ));
    *time = (end.tv_sec - start.tv_sec)* 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
    return time;
}
unsigned char** image_init(){
    unsigned char** new_image = malloc(height * sizeof(unsigned char *));
    for (int i = 0; i < height; i++) {
        new_image[i] = calloc(width, sizeof(unsigned char));
    }
    return new_image;
}

void free_image(unsigned char** img){
    for (int i = height-1; i > 0; i--) {
        free(img[i]);
    }
    free(img);
}

void write_output(FILE* file){
    fprintf(file,"P2\n#negative\n%d %d\n255\n",width,height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file,"%d", image_result[i][j]);
            if(j != width-1){
                fprintf(file," ");
            }
        }
        if(i != height-1){
            fprintf(file,"\n");
        }
    }
}