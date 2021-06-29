#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#define FILE_NAME "common.txt"
#define SEM_NAME "/kol_sem"

union semnum{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main(int argc, char** args){

   if(argc !=4){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }

    /**************************************************
    Stworz semafor systemu V
    Ustaw jego wartosc na 1
    ***************************************************/
    struct sembuf a= {0,1,0};
    struct sembuf b= {0,-1,0};

    key_t key = ftok(getenv("HOME"),1);
    int sem = semget(key,1,IPC_CREAT | 0666);
    union semnum arg;
    arg.val =1;
    semctl(sem, 0, SETVAL, arg);

     
     int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC , 0644);
     
     int parentLoopCounter = atoi(args[1]);
     int childLoopCounter = atoi(args[2]);
        
     char buf[50];
     pid_t childPid;
     int max_sleep_time = atoi(args[3]);
     


     if(childPid=fork()){
      int status = 0;
      srand((unsigned)time(0)); 

        while(parentLoopCounter--){
    	    int s = rand()%max_sleep_time+1;
    	    sleep(s);    
            
	    /*****************************************
	    sekcja krytyczna zabezpiecz dostep semaforem
	    **********************************************/
            if(semop(sem,&a, 1) == -1){
                fprintf(stderr,"%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            sprintf(buf, "Wpis rodzica. Petla %d. Spalem %d\n", parentLoopCounter,s);
	    write(fd, buf, strlen(buf));
	    write(1, buf, strlen(buf));
            
	    /*********************************
	    Koniec sekcji krytycznej
	    **********************************/
            if(semop(sem,&b, 1) == -1){
                fprintf(stderr,"%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

        }
        waitpid(childPid,&status,0);
     }
     else{

	srand((unsigned)time(0)); 
        while(childLoopCounter--){

	    int s = rand()%max_sleep_time+1;
            sleep(s);                
            

	    /*****************************************
	    sekcja krytyczna zabezpiecz dostep semaforem
	    **********************************************/
            if(semop(sem,&a, 1) == -1){
                fprintf(stderr,"%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            
            sprintf(buf, "Wpis dziecka. Petla %d. Spalem %d\n", childLoopCounter,s);
            write(fd, buf, strlen(buf));
	    write(1, buf, strlen(buf));

	    /*********************************
	    Koniec sekcji krytycznej
	    **********************************/
            if(semop(sem,&b, 1) == -1){
                fprintf(stderr,"%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

        }
        _exit(0);
     }
     
    /*****************************
    posprzataj semafor
    ******************************/
    if(semctl(sem,0,IPC_RMID,NULL) == -1){
        printf("Semctl error");
    }

     close(fd);
    return 0;
}
     
        
