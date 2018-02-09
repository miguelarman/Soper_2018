#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    
    int *entero = NULL;
    int pid;
    char * cadena = NULL;
    
    entero = (int *)malloc(1 * sizeof(int));
    if (entero == NULL) {
        exit(EXIT_FAILURE);
    }
    
    cadena = (char *)malloc(81 * sizeof(char));
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