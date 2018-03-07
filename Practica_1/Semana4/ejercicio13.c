#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFERSIZE 256
#define ARRAYSIZE 30


typedef struct param {
    int **matriz;
    int mult;
    int dim;
    int id;
} Param;


/*Función auxiliar*/

void (*calculaMatriz (void *arg)) {
    int **matriz = ((Param *)arg)->matriz;
    int mult = ((Param *)arg)->mult;
    int dim = ((Param *)arg)->dim;
    int id = ((Param *)arg)->id;
    
    int i, fila, col;
    
    
    for (fila = 0; fila < dim; fila++) {
        for (col = 0; col < dim; col++) {
            matriz[fila][col] *= mult;
        }
        
        printf("\nHilo %d multiplicando fila %d resultado: ", id, fila);
        for (i = 0; i < dim; i++) {
            printf("%d ", matriz[fila][i]);
        }
        
        fflush(stdout);
        usleep (1000000) ;
    }
    
    pthread_exit(NULL);
}


int main () {
    
    int dim;
    int mult1, mult2;
    int **matriz1 = NULL, **matriz2 = NULL;
    char cadenaAux[BUFFERSIZE];
    
    int valores1[ARRAYSIZE], valores2[ARRAYSIZE];
    char *tok;
    char *s1 = " ", *s2 = " ";
    
    int cantidad = 0;
    
    int indice;
    
    pthread_t h1;
    pthread_t h2;
    
    Param *param1 = NULL, *param2 = NULL;
    
    int i, j, k;
    
    /*Lee la dimensión*/
    printf("Introduzca dimension de la matriz cuadrada:\n");
    // scanf("%d", &dim);
    fgets(cadenaAux, BUFFERSIZE, stdin);
    dim = atoi(cadenaAux);
 
    /*La vuelve a pedir hasta que se recibe una válida*/
    while (dim > 5 || dim < 1) {
        printf("\nLa dimensión no puede exceder 5 ni ser menor que 1.\n");
        printf("Introduzca dimension de la matriz cuadrada: ");
        // scanf("%d", &dim);
        fgets(cadenaAux, BUFFERSIZE, stdin);
        dim = atoi(cadenaAux);
    }
    
    
    /*Lee el primer multiplicador*/
    printf("\nIntroduzca multiplicador 1:\n");
    // scanf("%d", &mult1);
    fgets(cadenaAux, BUFFERSIZE, stdin);
    mult1 = atoi(cadenaAux);
    
    
    
    /*Lee el segundo multiplicador*/
    printf("\nIntroduzca multiplicador 2:\n");
    // scanf("%d", &mult2);
    fgets(cadenaAux, BUFFERSIZE, stdin);
    mult2 = atoi(cadenaAux);
    
    
    
    /*Lee la primera matriz*/
    printf("\nIntroduzca matriz 1:\n");
    fgets(cadenaAux, BUFFERSIZE, stdin);
    
    if (cadenaAux == NULL) {
        printf("Error al leer la matriz 1");
        return -1;
    }
    
    tok = strtok(cadenaAux, s1);
    
    while (tok != NULL) {
        valores1[cantidad] = atoi(tok);
        cantidad++;
        
        tok = strtok(NULL, s1);
    }
    
    
    /*Comprueba que la cantidad de números insertados sea la necesaria para rellenar una matriz de la dimensión especificada*/
    if (cantidad != dim * dim) {
        printf("Error al insertar los valores de la matriz 1");
        return -1;
    }
    
    
    /*Lee la segunda matriz*/
    printf("\nIntroduzca matriz 2:\n");
    fgets(cadenaAux, BUFFERSIZE, stdin);
    
    if (cadenaAux == NULL) {
        printf("Error al leer la matriz 2");
        return -1;
    }
    
    tok = strtok(cadenaAux, s2);
    cantidad = 0;
    
    
    while (tok != NULL) {
        valores2[cantidad] = atoi(tok);
        cantidad++;
        
        tok = strtok(NULL, s2);
    }
    
    
    /*Comprueba que la cantidad de números insertados sea la necesaria para rellenar una matriz de la dimensión especificada*/
    if (cantidad != dim * dim) {
        printf("Error al insertar los valores de la matriz 2");
        return -1;
    }
    
    
    /*Reserva la memoria necesaria para la matriz 1. Si hay algún error libera la reservada anteriormente*/
    matriz1 = (int **)malloc(dim * sizeof(int *));
    if (matriz1 == NULL) {
        printf("Error al crear la matriz 1");
    }
    
    for (i = 0; i < dim; i++) {
        matriz1[i] = (int *)malloc(dim * sizeof(int));
        
        if (matriz1[i] == NULL) {
            printf("Error al reservar matriz1[%d]", i);
            
            for (j = 0; j < i; j++) {
                free(matriz1[j]);
            }
            
            free(matriz1);
            return -1;
        }
    }
    
    
    /*Reserva la memoria necesaria para la matriz 2. Si hay algún error libera toda la reservada anteriormente*/
    matriz2 = (int **)malloc(dim * sizeof(int *));
    if (matriz2 == NULL) {
        printf("Error al crear la matriz 2");
        
        for (i = 0; i < dim; i++) {
            free(matriz1[i]);
        }
        
        free(matriz1);
        return -1;
    }
    
    for (i = 0; i < dim; i++) {
        matriz2[i] = (int *)malloc(dim * sizeof(int));
        
        if (matriz2[i] == NULL) {
            printf("Error al reservar matriz2[%d]", i);
            
            for (j = 0; j < i; j++) {
                free(matriz2[j]);
            }
            
            free(matriz2);
            
            for (k = 0; k < dim; k++) {
                free(matriz1[k]);
            }
            
            free(matriz1);
            return -1;
        }
    }
    
    
    
    
    /*Guarda los valores en las matrices*/
    
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++) {
            indice = i * dim + j;       /*Calcula el índice de pasar de un array unidimensional a una matriz bidimensional*/
            
            matriz1[i][j] = valores1[indice];
            matriz2[i][j] = valores2[indice];
        }
    }
    
    /*Crea una estructura para los parámetros*/
    param1 = (Param *)malloc(sizeof(Param));
    if (param1 == NULL) {
        printf("Error al crear los parámetros");
        return -1;
    }
    
    param1->matriz = matriz1;
    param1->mult = mult1;
    param1->dim = dim;
    param1->id = 0;
    
    param2 = (Param *)malloc(sizeof(Param));
    if (param2 == NULL) {
        printf("Error al crear los parámetros");
        
        free(param1);
        return -1;
    }
    
    param2->matriz = matriz2;
    param2->mult = mult2;
    param2->dim = dim;
    param2->id = 1;
    
    
    pthread_create(&h1, NULL, calculaMatriz, param1);
    pthread_create(&h2, NULL, calculaMatriz, param2);
    
    /*Se ejecutan los threads*/
    
    pthread_join(h1,NULL);
    pthread_join(h2,NULL);

    printf("\nEl programa termino correctamente \n");
    exit(EXIT_SUCCESS);
    
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