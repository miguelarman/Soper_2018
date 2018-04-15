#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#include "aleat_num.h"

#define FILENAME "files/ej4/fichero_ej4"



/* Funciones privadas */
void primer_hijo();
void segundo_hijo();



int main() {
    pid_t child_pid_1, child_pid_2;
    
    /* Crea el primer hijo */
    if ((child_pid_1 = fork()) == -1) {
        perror("Error al hacer el primer fork");
    } else if (child_pid_1 == 0) {
        primer_hijo();
    }
    
    /* Espera a que termine el primer hijo */
    wait(NULL);
    
    /* Crea el segundo hijo */
    if ((child_pid_2 = fork()) == -1) {
        perror("Error al hacer el segundo fork");
    } else if (child_pid_2 == 0) {
        segundo_hijo();
    }
    
    /* Espera a que termine el segundo hijo */
    wait(NULL);
    
    exit(EXIT_SUCCESS);
}

/******************************************************************************/
/**********************     FUNCIONES PRIVADAS     ****************************/
/******************************************************************************/


void primer_hijo() {
    int cantidad;
    int i;
    int numero;
    FILE *pf;
    char coma;
    
    coma = ',';
    
    
    /* Abre el fichero para escribir los numeros */
    pf = fopen(FILENAME, "w");
    if (pf == NULL) {
        perror("Error al abrir el fichero");
        exit(EXIT_FAILURE);
    }
    
    cantidad = aleat_num(1000, 2000);
    
    for (i = 0; i < cantidad; i++) {
        numero = aleat_num(100, 1000);
        
        /* Escribe el numero en el fichero */
        /*DEBUGGING*//*printf("%d\n", numero);fflush(stdout);*/
        
        fwrite(&numero, sizeof(int), 1, pf);
        fwrite(&coma, sizeof(char), 1, pf);
    }
    
    
    /* Cierra el fichero */
    fclose(pf);
    
    exit(EXIT_SUCCESS);
}

void segundo_hijo() {
    
    
    
    
    
    
    exit(EXIT_SUCCESS);
}
/*
int main1(){
    pthread_t h1;
    pthread_t h2;
    
    pthread_create(&h1, NULL, primer_hijo, NULL);
    pthread_create(&h2, NULL, segundo_hijo, NULL);
    
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);
}
*/