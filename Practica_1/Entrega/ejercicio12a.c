/**
* @brief Ejercicio para la comparación de tiempo: procesos
*
* Este fichero, en conjunto con el ejercicio12b.c, sirven para comparar la
* eficiencia de los threads comparados con los procesos
* @file ejercicio12a.c
* @author Miguel Arconada Manteca y José Manuel Chacón Aguilera
* @date 8-3-2018
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define NUMERO_PROCESOS 100    /*!< Numero total de procesos que queremos crear */


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
 * @brief calcula los n primos numeros primos
 *
 * Esta funcion calcula los n primero numeros primos
 * @param N numero de primos a calcular
 * @return void
 */
void calcula_primos (int N);

/**
 * @brief Función principal del programa
 *
 * Este programa lanza 100 procesos simultáneamente, en los que
 * se calculan los N primeros primos
 * @param argc numero de parametros de entrada
 * @param argv array de cada parametro. El primero es el nombre
 * del programa, y el segundo es N
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main (int argc, char **argv) {
    int n;
    int i;
    int pid;
    clock_t start_t, end_t;
    double tiempoTotal;
    estructura *dinamico;
    
    /*Comprobacion de los parametros de entrada*/
    if (argc<2){
        printf("Parametros insuficientes");
        return -1;
    }
    
    n = atoi(argv[1]);
    
    
    
    dinamico = (estructura *) malloc (1 * sizeof(estructura));
    if (dinamico == NULL){
        printf("\nError en main, reserva1\n");
        return -1;
    }
    
    dinamico->cadena = (char *) malloc (100 * sizeof(char));
     if (dinamico->cadena == NULL){
        printf("\nError en main, reserva2\n");
        return -1;
    }
    
    dinamico->entero = (int *) malloc (1 * sizeof(int));
     if (dinamico->entero == NULL){
        printf("\nError en main, reserva3\n");
        return -1;
    }
    
    
    /*Empezamos a medir el tiempo*/
    start_t = clock();
    
    
    /*Generamos 100 procesos hijos*/
    for (i = 0; i < NUMERO_PROCESOS; i++){
        
        if ((pid=fork()) < 0 ){              /*Da error*/
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0){                /*Es un proceso hijo*/
            break;
        } else {
            continue;                        /*Es el proceso padre.
                                               vuelve a ejecutar
                                               el fork*/
        }
    }
    
    
    if (pid != 0) {                          /*Es el proceso padre*/
    
        for (i = 0; i < NUMERO_PROCESOS; i++) {  /*Hace wait para
                                                   todos sus hijos*/
            wait(NULL);
        }
    } else {        /*Es un hijo*/
        
        calcula_primos(n);  /*Ejecuta la función calcula_primos y lo imprime*/
        printf("El proceso con PID: %d ha calculado %d primos\n", getpid(), n);
        
        fflush(stdout);         /*Vacia el buffer para asegurarse de
                                  que se imprime*/
        exit(EXIT_SUCCESS);
        
    }
    
    
    /*Dejamos de contar el tiempo*/
    end_t = clock();
    tiempoTotal = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    printf("El programa ha tardado %f segundos en realizar las operaciones con N = %d\n", tiempoTotal, n);
    
    
    free(dinamico->cadena);
    free(dinamico->entero);
    free(dinamico);
    
    return 0;
}




/****************************************************************/

/*Funciones auxiliares*/

void calcula_primos (int N) {
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
}

/*Version previa de la misma funcion. La hicimos para ver cual
era mas eficiente, puesto que esta version solo comprueba si se divide por
algun numero primo menor, de forma similar a la criba de Eratóstenes.
Los resultados nos han dado que esta version
es mas lenta que la anterior*/

/*void calcula_primos (int N) {
    int cantidad = 0;
    int *primos = NULL;
    int iterador = 2;
    boolean primo;
    
    
    primos = (int *)malloc(N * sizeof(int));
    if (primos == NULL) {
        exit(EXIT_FAILURE);
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
}*/