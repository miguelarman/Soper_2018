#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NHILOS 100


typedef struct estruct{
    char *cadena;
    int *entero;
} Estruct;

typedef enum {TRUE, FALSE} boolean;


void (*calculaPrimos (void *arg)) {
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
}



int main (int argc, char **argv) {
    
    clock_t start_t, end_t;
    double tiempoTotal;
    int N;
    Estruct *dinamico = NULL;
    pthread_t hilos[NHILOS];
    
    
    
    if (argc < 2) {
        printf("Parametros insuficientes");
        return 0;
    }
    
    N = atoi(argv[1]);
    
    
    start_t = clock();
    
    
    dinamico = (Estruct*) malloc (1*sizeof(Estruct));
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
    
    
    
    for (int i = 0; i < NHILOS; i++) {
        pthread_create(&hilos[i], NULL , calculaPrimos , (void *)(&N));
    }
    
    
    for (int j = 0; j < NHILOS; j++) {
        pthread_join(hilos[j], NULL);
    }
    
    end_t = clock();
    tiempoTotal = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    printf("El programa ha tardado %lf segundos en realizar las operaciones con N = %d\n", tiempoTotal, N);
    
    return (0);
}