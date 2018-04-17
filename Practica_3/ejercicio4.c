#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <signal.h>
#include <string.h>

#include "aleat_num.h"

#define FILENAME "files/ej4/fichero_ej4"



/* Funciones privadas */
void *primer_hilo();
void *segundo_hilo();
void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);

int main(){
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
        if (ruta[i] == ","){
            ruta[i] = " ";
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
