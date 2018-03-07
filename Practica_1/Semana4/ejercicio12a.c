#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define NPROCESOS 100

typedef struct estruct{
    char *cadena;
    int *entero;
}Estruct;

typedef enum {TRUE, FALSE} boolean;


/*Funciones Auxiliares*/

void calculaPrimos (int N) {
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
}

/*Funcion Principal*/

int main(int argc, char **argv){
    int n, i, childpid, status;
    clock_t start_t, end_t;
    double tiempoTotal;
    Estruct *dinamico;
    if (argc<2){
        printf("\nToo few arguments\n");
        return -1;
    }
    
    n = atoi(argv[1]);
    
    start_t = clock();
    
    dinamico = (Estruct*) malloc (1*sizeof(Estruct));
    if (dinamico == NULL){
        printf("\nError en main, reserva1\n");
        return -1;
    }
    
    dinamico->cadena = (char*) malloc (100*sizeof(char));
     if (dinamico->cadena == NULL){
        printf("\nError en main, reserva2\n");
        return -1;
    }
    
    dinamico->entero = (int*) malloc (1*sizeof(int));
     if (dinamico->entero == NULL){
        printf("\nError en main, reserva3\n");
        return -1;
    }
    
    for (i = 1; i<=NPROCESOS; i++){
        childpid = fork();
        if (!childpid){
            calculaPrimos (n);
            printf("El proceso %d, con PID = %d ha calculado %d primos\n", i, getpid(), n);
            fflush(stdout);
            exit(EXIT_SUCCESS);
        }else{
            waitpid(childpid, &status, 0);
        }
    }
    
    end_t = clock();
    tiempoTotal = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    printf("El programa ha tardado %lf segundos en realizar las operaciones con N = %d\n", tiempoTotal, n);
    
    
    free(dinamico->cadena);
    free(dinamico->entero);
    free(dinamico);
    return 0;
}