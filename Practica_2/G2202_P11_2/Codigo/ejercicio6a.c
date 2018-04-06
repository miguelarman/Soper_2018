/**
 * @brief Ejercicio 6a de la Práctica
 * 
 * En este ejercicio modificamos un código dado,
 * para poner en práctica el uso de alarmas, así
 * como máscaras de bloqueo, y sus diferencias
 * en los bucles
 * 
 * @file ejercicio6a.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 6-4-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define NUM_PROC 5 /*!< Número de procesos a crear*/
#define SEGUNDOS 40 /*!< Número de segundos para la alarma*/

/**
 * @brief Función principal del programa
 *
 * Este programa crea 5 procesos hijos, que tras ser creado,
 * establecen una alarma. Entonces entran en un bucle,
 * y bloquean la recepcion de señales hasta haber imprimido.
 * Por lo tanto, realizan la impresión completa
 * 
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
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