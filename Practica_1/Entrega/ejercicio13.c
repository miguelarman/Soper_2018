/**
* @brief Ejercicio sobre paso de parámetros en funciones
*
* Este fichero contiene nuestra solución al ejercicio 13 enunciado
* en el pdf de la práctica
* @file ejercicio13.c
* @author Miguel Arconada Manteca y José Manuel Chacón Aguilera
* @date 8-3-2018
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 256    /*!< Tamanio maximo del buffer cadena_aux */
#define ARRAY_SIZE 30      /*!< Tamanio maximo de los buffers valores*/


/**
* @brief Estructura de parámetros de un hilo
*
* Esta estructura almacena la información necesaria para los hilos en
* este ejercicio
*/
typedef struct param {
    int **matriz;       /*!< Matriz */
    int mult;           /*!< Multiplicador */
    int dim;            /*!< Dimension de las matrices */
    int id;             /*!< Identificador del hilo */
} Param;



/**
 * @brief multiplica una matriz por un escalar
 *
 * calcula_matriz multiplica una matriz por un escalar. Está
 * almacenada en forma de puntero a función para ser ejecutada
 * por varios threads simultáneamente.
 * @param arg estructura del tipo Param que contiene la matriz,
 * el escalar, la dimension y un identificador del hilo
 * @return void
 */
void (*calcula_matriz (void *arg));




/**
 * @brief Función principal del programa
 *
 * Este programa pide por consola una dimensión, dos multiplicadores
 * y dos matrices, y lanza dos hilos que multiplican cada matriz por
 * los escalares, y que van imprimiendo cada línea según la calculan
 * @param void
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main () {
    
    int dim;
    int mult_1, mult_2;
    int **matriz_1, **matriz_2;
    int valores_1[ARRAY_SIZE], valores_2[ARRAY_SIZE];
    int cantidad;
    int indice;
    int i, j, k;
    
    char cadena_aux[BUFFER_SIZE];
    char *tok;
    char *s1 = " ", *s2 = " ";
    
    pthread_t h1;
    pthread_t h2;
    
    Param *param_1 = NULL, *param_2 = NULL;
    
    
    
    /*Lee la dimensión*/
    
    printf("Introduzca dimension de la matriz cuadrada:\n");
    fgets(cadena_aux, BUFFER_SIZE, stdin);
    dim = atoi(cadena_aux);
 
    /*La vuelve a pedir hasta que se recibe una válida*/
    
    while (dim > 5 || dim < 1) {
        printf("\nLa dimensión no puede exceder 5 ni ser menor que 1.\n");
        printf("Introduzca dimension de la matriz cuadrada: ");
        fgets(cadena_aux, BUFFER_SIZE, stdin);
        dim = atoi(cadena_aux);
    }
    
    
    /*Lee el primer multiplicador*/
    
    printf("\nIntroduzca multiplicador 1:\n");
    fgets(cadena_aux, BUFFER_SIZE, stdin);
    mult_1 = atoi(cadena_aux);
    
    
    
    /*Lee el segundo multiplicador*/
    
    printf("\nIntroduzca multiplicador 2:\n");
    fgets(cadena_aux, BUFFER_SIZE, stdin);
    mult_2 = atoi(cadena_aux);
    
    
    
    /*Lee la primera matriz*/
    
    printf("\nIntroduzca matriz 1:\n");
    fgets(cadena_aux, BUFFER_SIZE, stdin);
    
    if (cadena_aux == NULL) {
        printf("Error al leer la matriz 1");
        return -1;
    }
    
    tok = strtok(cadena_aux, s1);
    cantidad = 0;
    
    while (tok != NULL) {
        valores_1[cantidad] = atoi(tok);
        cantidad++;
        
        tok = strtok(NULL, s1);
    }
    
    
    /*Comprueba que la cantidad de números insertados
    sea la necesaria para rellenar una matriz de la
    dimensión especificada*/
    
    if (cantidad != dim * dim) {
        printf("Error al insertar los valores de la matriz 1");
        return -1;
    }
    
    
    /*Lee la segunda matriz*/
    
    printf("\nIntroduzca matriz 2:\n");
    fgets(cadena_aux, BUFFERSIZE, stdin);
    
    if (cadena_aux == NULL) {
        printf("Error al leer la matriz 2");
        return -1;
    }
    
    tok = strtok(cadena_aux, s2);
    cantidad = 0;
    
    
    while (tok != NULL) {
        valores_2[cantidad] = atoi(tok);
        cantidad++;
        
        tok = strtok(NULL, s2);
    }
    
    
    /*Comprueba que la cantidad de números insertados
    sea la necesaria para rellenar una matriz de la
    dimensión especificada*/
    
    if (cantidad != dim * dim) {
        printf("Error al insertar los valores de la matriz 2");
        return -1;
    }
    
    
    /*Reserva la memoria necesaria para la matriz 1.
    Si hay algún error libera la reservada anteriormente*/
    
    matriz_1 = (int **)malloc(dim * sizeof(int *));
    if (matriz_1 == NULL) {
        printf("Error al crear la matriz 1");
    }
    
    for (i = 0; i < dim; i++) {
        matriz_1[i] = (int *)malloc(dim * sizeof(int));
        
        if (matriz_1[i] == NULL) {
            printf("Error al reservar matriz_1[%d]", i);
            
            for (j = 0; j < i; j++) {
                free(matriz_1[j]);
            }
            
            free(matriz_1);
            return -1;
        }
    }
    
    
    /*Reserva la memoria necesaria para la matriz 2.
    Si hay algún error libera toda la reservada anteriormente*/
    
    matriz_2 = (int **)malloc(dim * sizeof(int *));
    if (matriz_2 == NULL) {
        printf("Error al crear la matriz 2");
        
        for (i = 0; i < dim; i++) {
            free(matriz_1[i]);
        }
        
        free(matriz_1);
        return -1;
    }
    
    for (i = 0; i < dim; i++) {
        matriz_2[i] = (int *)malloc(dim * sizeof(int));
        
        if (matriz_2[i] == NULL) {
            printf("Error al reservar matriz_2[%d]", i);
            
            for (j = 0; j < i; j++) {
                free(matriz_2[j]);
            }
            
            free(matriz_2);
            
            for (k = 0; k < dim; k++) {
                free(matriz_1[k]);
            }
            
            free(matriz_1);
            return -1;
        }
    }
    
    
    
    
    /*Guarda los valores en las matrices*/
    
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++) {
            indice = i * dim + j;       /*Calcula el índice de pasar de un
                                        array unidimensional a una matriz
                                        bidimensional*/
            
            matriz_1[i][j] = valores_1[indice];
            matriz_2[i][j] = valores_2[indice];
        }
    }
    
    /*Crea una estructura para los parámetros*/
    
    param_1 = (Param *)malloc(sizeof(Param));
    if (param_1 == NULL) {
        printf("Error al crear los parámetros");
        return -1;
    }
    
    param_1->matriz = matriz_1;
    param_1->mult = mult_1;
    param_1->dim = dim;
    param_1->id = 0;
    
    param_2 = (Param *)malloc(sizeof(Param));
    if (param_2 == NULL) {
        printf("Error al crear los parámetros");
        
        free(param_1);
        return -1;
    }
    
    param_2->matriz = matriz_2;
    param_2->mult = mult_2;
    param_2->dim = dim;
    param_2->id = 1;
    
    /*Se ejecutan los threads*/
    
    pthread_create(&h1, NULL, calcula_matriz, param_1);
    pthread_create(&h2, NULL, calcula_matriz, param_2);
    
    pthread_join(h1,NULL);
    pthread_join(h2,NULL);

    printf("\nEl programa termino correctamente \n");
    exit(EXIT_SUCCESS);
    
    return 0;
    
}

/****************************************************************/

/*Funciones auxiliares*/

void (*calcula_matriz (void *arg)) {
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