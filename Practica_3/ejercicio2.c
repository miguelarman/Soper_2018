#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>

#include "semaforos.h"
#include "aleat_num.h"

#define FILEKEY "/bin/bash"
#define KEY 1300

typedef struct info{
    char nombre[80];
    int id;
} Estructura;


int main(int argc, char **argv){
    int n_proc;
    int i;
    if (argc < 2){
        printf("Not enough parameters, %d", argc);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    
    /* Key to shared memory */
    /*key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error with key \n");
        return -1;
    }
    */
    
    n_proc = atoi(argv[1]);
    for (i = 0; i<n_proc; i++){
        if (fork()==0){
            /*Código del hijo*/
            
            
            
            
            
            exit(EXIT_SUCCES);
        }else{
            /*Código del Padre*/
            continue;
        }
    }
    /*El padre*/
    while(wait(NULL)>0);
}