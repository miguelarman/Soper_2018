/**
 * @brief Ejercicio 4 de la Práctica
 * 
 * En este ejercicio vemos cómo crear una serie
 * de procesos hijos, que tras un tiempo piden
 * al padre ser relevados mediante el uso de señales
 * 
 * @file ejercicio4.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 6-4-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define SEGUNDOS(X) (X) * 1000000 /*!< Macro para tranformar segundos a microsegundos*/


/**
 * @brief manejador de la señal SIGUSR1
 *
 * Esta funcion es ejecutada cuando el padre recibe
 * la señal SIGUSR1. Es una función vacía, puesto lo que 
 * nos interesa es simplemente esperar a dicha señal
 * 
 * @param senal Código de la señal recibida
 * @return void
 */
void manejador (int senal);


/**
 * @brief Función principal del programa
 *
 * Este programa crea un hijo, que realiza un trabajo (imprimir
 * por pantalla y esperar) 10 veces. Tras esto, manda una señal
 * al padre pidiendo relevo. El padre entonces mata a este proceso
 * y crea otro igual, hasta un máximo de N. Entonces finaliza
 * 
 * @param argc numero de parametros de entrada
 * @param argv array de cada parametro. El segundo es el número máximo
 * de procesos a crear
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main (int argc, char **argv) {
    pid_t child_pid, padre_pid;
    int i;
    int total_creados;
    int n_proc;
    int retorno_senial, retorno_senial_hijo;
    
    
    /*Comprobaciones de entrada*/
    
    if (argc < 2) {
        printf("Error en los parámetros de entrada. Debe especificar un número");
        
        exit(EXIT_FAILURE);
    }
    
    if (argv[1] == NULL) {
        printf("Error con el segundo parámetro");
        
        exit(EXIT_FAILURE);
    }
    
    
    
    
    
    
    n_proc = atoi (argv[1]);
    
    total_creados = 0;
    
    while(1) {
        /*crea el hijo*/
        child_pid = fork();
        total_creados++;
        
        if (child_pid == -1) {
            
            perror("Error con el fork");
            
            exit(EXIT_FAILURE);
            
        } else if (child_pid == 0) {
            break;
        }
        
        
        if(signal (SIGUSR1, manejador)==SIG_ERR){
            perror("manejador");
            exit(EXIT_FAILURE);
        }
        
        
        /*espera la señal*/
        pause();
        
        
        /*Si ha hecho N manda la terminacion al hijo y termina*/
        
        retorno_senial = kill(child_pid, SIGTERM);
            
            if (retorno_senial == -1) {
                perror("Error al mandar señal");
                
                exit(EXIT_FAILURE);
            }
            
            wait(NULL);
            
            
        if (total_creados >= n_proc) {
            
            printf("\nTerminado");
            exit(EXIT_SUCCESS);
        }
    }
    
    
    
    /*cosas de hijo*/
    
    i = 0;  
    
    while (1){
        
        /*Trabajo del hijo*/
        printf("Soy <%d> y estoy trabajando\n", getpid());
        fflush(stdout);
        usleep(SEGUNDOS(1));
        i++;
        
        if (i == 10) {
            
            padre_pid = getppid();
            
            
            retorno_senial_hijo = kill(padre_pid, SIGUSR1);
            
            
            if (retorno_senial_hijo == -1) {
                perror("Error al mandar señal el hijo");
                
                exit(EXIT_FAILURE);
            }
        }
    }
    
    exit(EXIT_SUCCESS);
}


/*Funciones auxiliares*/
void manejador (int senal) {}