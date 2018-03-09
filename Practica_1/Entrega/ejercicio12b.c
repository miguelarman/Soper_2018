/**
* @brief Ejercicio para la comparación de tiempo: hilos
*
* Este fichero, en conjunto con el ejercicio12b.c, sirven para comparar la
* eficiencia de los threads comparados con los procesos
* @file ejercicio12b.c
* @author Miguel Arconada Manteca y José Manuel Chacón Aguilera
* @date 8-3-2018
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define NUMERO_HILOS 100    /*!< Numero total de hilos que queremos crear */

/**
* @brief estructura generada al principio de la ejecución
*
* Esta estructura almacena una cadena de 100 caracteres y un entero.
* Se usa para comparar el uso de memoria entre los hilos y los procesos
*/
typedef struct _estructura{
    char *cadena;       /*!< Cadena de 100 caracteres */
    int *entero;        /*!< Puntero a un entero */
} estructura;



/**
 * @brief calcula los N primeros primos
 *
 * calcula_primos calcula los N primeros números primos
 * @param arg puntero a un entero que contiene N hecho casting a void*
 * @return void
 */
void (*calcula_primos (void *arg));


/**
 * @brief Función principal del programa
 *
 * Este programa lanza 100 hilos simultáneamente, en los que
 * se calculan los N primeros primos
 * @param argc numero de parametros de entrada
 * @param argv array de cada parametro. El primero es el nombre
 * del programa, y el segundo es N
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main (int argc, char **argv) {
    
    clock_t start_t, end_t;
    double tiempo_total;
    int N;
    estructura *dinamico = NULL;
    pthread_t hilos[NUMERO_HILOS];
    int i, j;
    
    
    /*Comprobacion de los parametros de entrada*/
    if (argc < 2) {
        printf("Parametros insuficientes");
        return 0;
    }
    
    N = atoi(argv[1]);
    
    
    /*Empezamos a medir el tiempo*/
    start_t = clock();
    
    /*Creamos la estructura que nos pide el enunciado con una cadena
    de 100 caracteres y un entero*/
    dinamico = (estructura*) malloc (1*sizeof(estructura));
    if (dinamico == NULL){
        printf("\nError en main, reserva1\n");
        return -1;
    }
    
    
    dinamico->cadena = (char*) malloc (100*sizeof(char));
    if (dinamico->cadena == NULL){
        printf("\nError en main, reserva2\n");
        free(dinamico);
        return -1;
    }
    dinamico->entero = (int*) malloc (1*sizeof(int));
    if (dinamico->entero == NULL){
        printf("\nError en main, reserva3\n");
        free(dinamico);
        free(dinamico->cadena);
        return -1;
    }
    
    
    /*Creamos 100 hilos que ejecutan calcula_primos*/
    for (i = 0; i < NUMERO_HILOS; i++) {
        pthread_create(&hilos[i], NULL , calcula_primos , (void *)(&N));
    }
    
    /*Se ejecutan los 100 hilos*/
    
    /*Juntamos los hilos*/
    for (j = 0; j < NUMERO_HILOS; j++) {
        pthread_join(hilos[j], NULL);
    }
    
    
    /*Paramos de medir el tiempo*/
    end_t = clock();
    tiempo_total = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    printf("El programa ha tardado %f segundos en realizar las operaciones con N = %d\n", tiempo_total, N);
    
    if (dinamico != NULL) {
        free(dinamico->cadena);
        free(dinamico->entero);
        free(dinamico);
    }
    
    return (0);
}



/****************************************************************/

/*Funciones auxiliares*/

void (*calcula_primos (void *arg)) {
    int N = *((int *) arg);
    int cantidad;
    int iterador;
    int i;
     int esPrimo;
        
    cantidad = 0;   /*Cuenta cuantos primos se han hayado hasta el momento*/
    iterador = 2;
        
    while (cantidad < N) {
        esPrimo = 1;
        
        for (i = 2; i <= sqrt(iterador); i++) {
            if (iterador % i == 0) {
                esPrimo = 0;
                
                break;
            }
        }
        
        if (esPrimo == 1){
            cantidad++;
        }
        
        iterador++;
    }
    
    
    
    
    pthread_exit(NULL);
}



/*Version previa de la misma funcion. La hicimos para ver cual
era mas eficiente. Los resultados nos han dado que esta version
es mas lenta que la anterior*/

/*void (*calcula_primos (void *arg)) {
    int N = *((int *) arg);
    int cantidad = 0;
    int *primos = NULL;
    int iterador = 2;
    boolean primo;
    
    
    primos = (int *)malloc(N * sizeof(int));
    if (primos == NULL) {
        pthread_exit(NULL);
    }
    
    
    while (cantidad < N) {
        primo = TRUE;
        
        for (int i = 0; i < cantidad; i++) {
            if (iterador % primos[i] == 0) {
                primo = FALSE;
                
                break;
            }
        }
        
        if (primo == TRUE) {
            primos[cantidad] = iterador;
            cantidad++;
        }
        
        iterador++;
    }
    
    pthread_exit(NULL);
}*/