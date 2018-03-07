#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>



int main () {
    
    int dim;
    int mult1, mult2;
    int **matriz1 = NULL, **matriz2 = NULL;
    
    
    
    printf("Introduzca dimension de la matriz cuadrada: ");
    scanf("%d", &dim);
    
    while (dim > 5) {
        printf("\nLa dimensión no puede exceder 5.\n");
        printf("Introduzca dimension de la matriz cuadrada: ");
        scanf("%d", &dim);
    }
    
    
    printf("\nIntroduzca multiplicador 1: ");
    scanf("%d", &mult1);
    
    printf("\nIntroduzca multiplicador 2: ");
    scanf("%d", &mult2);
    
    
    
    /*DEBUGGING*/
    printf("\n\ndim: %d, mult1: %d, mult2: %d", dim, mult1, mult2);
    /*DEBUGGING*/
    
    
    
    
    
    
    
    
    return 0;
    
}

/*
Introduzca multiplicador 1:
3
Introduzca multiplicador 2:
6
Introduzca matriz 1:
3 5 4 6 2 2 1 2 3
Introduzca matriz 2:
3 5 5 3 2 2 2 2 3
Realizando producto:
Hilo 1 multiplicando fila 0 resultado 9 15 12
Hilo 2 multiplicando fila 0 resultado 18 30 30
Hilo 2 multiplicando fila 1 resultado 18 12 12
…*/