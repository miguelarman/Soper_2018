/**
 * @brief Ejercicio 4 de la Práctica
 * 
 * En este ejercicio se nos pide trabajar
 * con la funcion de C mmap que es útil para
 * mapear memoria y acceder desde otros 
 * procesos a la misma. Un hilo se comunica 
 * en este ejercicio con otro hilo mediante
 * el uso de dicha función.
 * 
 * @file ejercicio4.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 17-4-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "aleat_num.h"

#define FILENAME "files/ej4/fichero_ej4" /*!< Nombre del Fichero donde se genera*/


/* Funciones privadas */

/**
 * @brief Ejecución del primer hilo, escribe números aleatorios en un fichero de texto
 * 
 * @return void
 */
void *primer_hilo();

/**
 * @brief Ejecución del Segundo hilo, mapea la memoria del fichero escrito por el primer hilo y modifica el 
 * 
 * @return void
 */
void *segundo_hilo();

/**
 * @brief Función Main del ejercicio, invoca a dos hilos y posteriormente los ejecuta con las funciones descritas en este mismo archivo
 * 
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main () {
    pthread_t h1;
    pthread_t h2;
    
    /*Primer Hilo*/
    pthread_create(&h1, NULL, primer_hilo, NULL);
    pthread_join(h1, NULL);
    pthread_cancel(h1);
    
    /*Segundo Hilo*/
    pthread_create(&h2, NULL, segundo_hilo, NULL);
    pthread_join(h2, NULL);
    pthread_cancel(h2);
    
    exit(EXIT_SUCCESS);
}

/******************************************************************************/
/**********************     FUNCIONES PRIVADAS     ****************************/
/******************************************************************************/


void *primer_hilo() {
    int cantidad;
    int i;
    int numero;
    FILE *pf;
    
    
    /* Abre el fichero para escribir los numeros */
    pf = fopen(FILENAME, "w");
    if (pf == NULL) {
        perror("Error al abrir el fichero");
        exit(EXIT_FAILURE);
    }
    
    cantidad = aleat_num(1000, 2000);
    
    for (i = 0; i < cantidad; i++) {
        numero = aleat_num(100, 1000);
        
        fprintf(pf, "%d", numero);
        fprintf(pf, ",");
        
    }
    
    
    /* Cierra el fichero */
    fclose(pf);
    pthread_exit(NULL);
}

void *segundo_hilo() {
    char *ruta;
    int i, id, debbug;
    struct stat buf;
    /*Conseguimos el fichero en memoria compartida*/
    id = open(FILENAME, O_RDWR);
    fstat(id, &buf);
    
    ruta = (char*) mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0);
    if (ruta == MAP_FAILED){
        close(id);
        perror("Error al hacer el mapeo de memoria");
        exit(EXIT_FAILURE);
    }
    
    /*Ahora cambiamos las comas por espacios recoriendo el fichero caractera a caracter*/
    for (i = 0; i < strlen(ruta); i++){
        if (ruta[i] == ','){
            ruta[i] = ' ';
        }
    }
    printf("Estos son los datos leidos y modificados: %s\n", ruta);
    
    /*Eliminamos el Mapeo*/
    debbug = munmap(ruta, buf.st_size);
    if (debbug == -1){
        close(id);
        perror("Error al hacer munmap");
        exit(EXIT_FAILURE);
    }
    
    close(id);
    pthread_exit(NULL);
}
