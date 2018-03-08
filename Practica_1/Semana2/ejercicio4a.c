/**
 * @brief Ejercicio 4a de la Práctica
 * 
 * En este ejercicio vemos el ejemplo de ejecucioón de un programa que lanza procesos hijos.
 * 
 * @file ejercicio4a.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 8-3-2018
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define NUM_PROC 6 /*!< Número de iteraciones del bucle creador de procesos*/

int main (void) {
    int pid;
    int i;

    for (i = 0; i <= NUM_PROC; i++){
        if (i % 2 == 0) {
            if ((pid=fork()) < 0 ){
                printf("Error al emplear fork\n");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                printf("HIJO %d\n", getpid());
                printf ("PADRE %d \n", getppid());
            }
        }
    }
    exit(EXIT_SUCCESS);
} 
