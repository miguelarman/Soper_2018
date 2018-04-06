/**
 * @brief Ejercicio 6b de la Práctica
 * 
 * En este ejercicio modificamos un código dado,
 * para poner en práctica el uso de alarmas, así
 * como máscaras de bloqueo, y sus diferencias
 * en los bucles, en conjunción con el ejercicio
 * 6b
 * 
 * @file ejercicio6b.c
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
 * @brief manejador de la señal SIGTERM
 *
 * Esta funcion es ejecutada cuando el padre recibe
 * la señal SIGTERM. Cuando la recibe, imprime un
 * mensaje y termina su ejecución
 * 
 * @param senal Código de la señal recibida
 * @return void
 */
void manejador (int senal);


/**
 * @brief Función principal del programa
 *
 * Este programa crea 5 procesos hijos, que tras ser creado,
 * establecen una alarma. Entonces entran en un bucle,
 * pero no bloquean las señales, como hacía el ejercicio6a.c
 * Por lo tanto, se pueden acaban en medio de una impresión
 * 
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
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

/*Funciones auxiliares*/

void manejador (int senal) {
    printf("Soy <%d> y he recibido la señal SIGTERM\n", getpid());
    fflush(stdout);
    exit(EXIT_SUCCESS);
}