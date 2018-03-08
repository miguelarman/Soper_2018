/**
 * @brief Ejercicio 6 de la Práctica
 * 
 * En este ejercicio combinamos cuestiones teóricas sobre 
 * reserva de memoria dinamica y ejecución del programa en procesos distintos
 * 
 * @file ejercicio6.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 8-3-2018
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define MAX_CAD 128 /*!< Tamaño máximo de las cadenas inicializadas*/

int main () {
    int *entero = NULL;
    int pid;
    char *cadena = NULL;
    
    entero = (int *)malloc(1 * sizeof(int));
    if (entero == NULL) {
        exit(EXIT_FAILURE);
    }
    
    cadena = (char *)malloc(MAX_CAD * sizeof(char));
    if (cadena == NULL) {
        exit(EXIT_FAILURE);
    }
    
    
    pid = fork();
    
    if (pid == 0) {
        printf("Introduce tu nombre: ");
        scanf("%s", cadena);
        
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL); /*Esperamos a que el proceso hijo reciba el nombre*/
        
        printf("%s", cadena);
        free(cadena);
        free(entero);
        
        exit(EXIT_SUCCESS);
    }
}