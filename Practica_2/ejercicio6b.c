#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define NUM_PROC 5
#define SEGUNDOS 40


void manejador (int senal);

int main (void) {
    int pid, counter;
    int retorno_senial;
    
    pid = fork();
    
    if (pid == 0) {
        /*El proceso hijo, al recibir la señal SIGTERM, imprimirá el mensaje “Soy <PID> y he 
        recibido la señal SIGTERM” y finalizará su ejecución.*/
        
        if(signal (SIGTERM, manejador)==SIG_ERR){
            perror("manejador");
            exit(EXIT_FAILURE);
        }
        
        while(1) {
            for (counter = 0; counter < NUM_PROC; counter++){
                printf("%d\n", counter);
                sleep(1);
            }
            sleep(3);
        }
    } else {
        /*El  padre  enviará  la  señal SIGTERM al  proceso  hijo  cuando  
        hayan  pasado  40  segundos de la creación del hijo.*/
        sleep(SEGUNDOS);
        retorno_senial = kill(pid,SIGTERM);
        if (retorno_senial == -1) {
            perror("Error al mandar señal el padre");
            
            exit(EXIT_FAILURE);
        }
            
        /*Cuando el hijo haya finalizado su ejecución, 
        el padre finalizará.*/
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
    
    return 0;
}



void manejador (int senal) {
    printf("Soy <%d> y he recibido la señal SIGTERM\n", getpid());
    fflush(stdout);
    exit(EXIT_SUCCESS);
}