#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define NUM_PROC 5

#define SEGUNDOS 40


int main (void) {
    int pid, counter;
    sigset_t set, oset;
    int error;
    
    
    pid = fork();
    
    if (pid == 0) {
        
        /*El hijo, justo después de ser creado, establecerá una alarma para ser recibida  dentro de 40 segundos*/
        alarm(SEGUNDOS);
        
        while(1) {
            
            /*Justo antes de comenzar cada bucle de imprimir números en un proceso hijo, las
            señales SIGUSR1, SIGUSR2 y SIGALRM deben quedar bloqueadas*/

            sigemptyset(&set);
            sigaddset(&set, SIGUSR1); /* Máscara que bloqueará la señal USR1*/
            sigaddset(&set, SIGUSR2); /* Añade a la máscara el bloqueo por USR2*/
            sigaddset(&set, SIGALRM); /* Añade a la máscara el bloqueo por ALRM*/
    
    
            error = sigprocmask(SIG_BLOCK, &set,&oset); /*Bloquea la recepción de las
                señales SIGUSR1, SIGUSR2, SIGALARM  en el proceso */
    
            if(error){
                /* Tratamiento del error*/
                perror("Error al bloquear las señales SIGUSR1, SIGUSR2, SIGALARM");
                exit(EXIT_FAILURE);
            }
    
            /*********************************************************/
            
            for (counter = 0; counter < NUM_PROC; counter++){
                printf("%d\n", counter);
                sleep(1);
            }
            
            /*Al finalizar el bucle de impresión de números, y antes de la espera de 3 segundos,
            se desbloqueará la señal SIGALRM y SIGUSR1.*/
    
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1); /* Máscara que desbloqueará la señal USR1*/
            sigaddset(&set, SIGALRM); /* Añade a la máscara el desbloqueo por ALRM*/
            
            
            error = sigprocmask(SIG_UNBLOCK, &set,&oset); /*Desbloquea la recepción de las
            señales SIGUSR1, SIGUSR2, SIGALARM  en el proceso */
            
            if(error){
                /* Tratamiento del error*/
                perror("Error al desbloquear las señales SIGUSR1, SIGUSR2, SIGALARM");
                exit(EXIT_FAILURE);
            }
            /*********************************************************/
            
            sleep(3);
        }
    }
    
    while(wait(NULL) > 0);
    
    
    return 0;
}