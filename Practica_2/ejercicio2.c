#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM_HIJOS 4
#define SEGUNDOS(X) (X) * 1000000

int main (){
    pid_t child_pid;
    int i;
    int retorno_senial;
    
    for (i = 0; i < NUM_HIJOS; i++) {
        
        child_pid = fork();
        
        if (child_pid == -1) {
            perror("Error al hacer fork");
            
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) {
            break;
        } else {
            /*Tiempo de espera requerido en el enunciado*/
            usleep(SEGUNDOS(5));
            
            /*Padre manda señales*/
            retorno_senial = kill (child_pid, SIGTERM);
            
            if (retorno_senial == -1) {
                perror("Error al mandar señal");
                
                exit(EXIT_FAILURE);
            }
            
        }
        
    }
    if (child_pid != 0){
        /*Ejecución del padre fuera del bucle*/
        for (i = 0; i< NUM_HIJOS; i++){
            wait(NULL);
        }
        
        exit(EXIT_SUCCESS);
        
    } else {
        
        printf("Soy el proceso hijo <%d>\n", getpid());
        fflush(stdout);
        
        /*Tiempo de espera requerido en el enunciado*/
        usleep(SEGUNDOS(30));
        
        printf("Soy el proceso hijo <%d> y ya me toca terminar\n", getpid());
        fflush(stdout);
        
        exit(EXIT_SUCCESS);    
    }
    
    exit(EXIT_SUCCESS);
}
