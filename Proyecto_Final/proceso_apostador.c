#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
       
#include "estructuras.h"
#include "memoria_compartida.h"
#include "aleat_num.h"

#define MAX_NAME 20 /*!< Número maximo de caracteres para el nombre de los apostadores*/
#define KEY 1300 /*!< KEY necesaria para el ftok */
#define FILEKEY "/bin/bash" /*!< FILEKEY necesario para ftok */


int main(int argc, char **argv){
    Memoria_Compartida *memoria_compartida;
    int n_apostadores;
    int apuesta_maxima;
    int n_caballos;
    char nombre_apostador[MAX_NAME];
    int caballo_aleat;
    int apuesta_aleat;
    int apostador_aleat;
    int key;
    int shmid;
    int msqid;
    int retorno_envio;
    int retorno_shmctl;
    int retorno_shmdt;
    sigset_t set, oset;
    
    if (argc < 4){
        perror("No suficientes parámetros");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) - getpid()); /*Seed para generación de números aleatorios*/
    
    n_apostadores = atoi(argv[1]);
    apuesta_maxima = atoi(argv[2]);
    n_caballos = atoi(argv[3]);
    
    /* Calcula la key para la cola de mensajes*/
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        perror("Error al usar ftok");
        /*liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
        
     /* Crea la cola de mensajes */
    msqid = msgget (key, IPC_CREAT | IPC_EXCL | 0660);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    
    
    /* Crea la memoria compartida */
    shmid = reservashm(sizeof(Memoria_Compartida*), key);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue la memoria compartida*/
    memoria_compartida = shmat (shmid, (char *)0, 0);
    if (memoria_compartida == (void *) -1) {
        perror("Error al conseguir la memoria compartida en el proceso principal");
        exit(EXIT_FAILURE);
    }
    
    /* Guarda el numero total de apostadores y de caballos */
    memoria_compartida->num_apuestas = 0;
    
    
    /* Inicializa la mascara de bloqueos y crea el manejador de SIGUSR1*/
    
    if (sigfillset(&set) == -1) {
        perror("Error con sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SIGUSR1) == -1) {
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SIGTERM) == -1) { /* No bloqueamos la senal SIGTERM */
        perror("Error con sigdelset");    /* para poder matar el proceso desde */
        exit(EXIT_FAILURE);               /* la terminal si es necesario*/
    }
    if (sigprocmask (SIG_BLOCK, &set, &oset) == -1) {
        perror("Error con sigprocmask");
        exit(EXIT_FAILURE);
    }
    
    if(signal (SIGUSR1, manejador) == SIG_ERR){
        perror("Error en el manejador");
        exit(EXIT_FAILURE);
    }
    
    
    /*Generación de Apuestas*/
    while (1){
        apostador_aleat = aleat_num(1, n_apostadores);
        sprintf(nombre_apostador, "Apostador_%d", apostador_aleat);
        caballo_aleat = aleat_num(1, n_caballos);
        apuesta_aleat = aleat_num(0, apuesta_maxima);
        
        memoria_compartida->historial_apuestas[memoria_compartida->num_apuestas].numero_caballo = caballo_aleat;
        memoria_compartida->historial_apuestas[memoria_compartida->num_apuestas].cuantia = apuesta_aleat;
        strcpy(memoria_compartida->historial_apuestas[memoria_compartida->num_apuestas].nombre, nombre_apostador);
        
        memoria_compartida->num_apuestas++;
        
        /* Manda el mensaje al padre */
        retorno_envio = msgsnd (msqid, (struct Memoria_Compartida*) memoria_compartida, sizeof(Memoria_Compartida) - sizeof(long), 0);
        
        if (retorno_envio == -1) {
            perror("Error al mandar el mensaje en el caballo");
            
            /*Eliminamos la memoria compartida*/
            retorno_shmdt = shmdt ((char *)memoria_compartida);
            if (retorno_shmdt == -1) {
                perror("Error al hacer el dettach en el proceso principal");
                exit(EXIT_FAILURE);
            }
            
            retorno_shmctl = shmctl (shmid, IPC_RMID, (struct shmid_ds *)NULL);
            if (retorno_shmctl == -1) {
                perror("Error al borrar la memoria en el proceso principal");
                exit(EXIT_FAILURE);
            }
            
            exit(EXIT_FAILURE);
        }
        
        
        
        /*sleep(0.0001);  Vermos mas adelanta si se llena el buffer para usar esto*/
    }
    
    exit(EXIT_SUCCESS);
}
