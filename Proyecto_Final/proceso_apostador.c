#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
       
#include "estructuras.h"
#include "memoria_compartida.h"
#include "aleat_num.h"

#define MAX_NAME 20 /*!< Número maximo de caracteres para el nombre de los apostadores*/
#define KEY 1300 /*!< KEY necesaria para el ftok */
#define FILEKEY "/bin/bash" /*!< FILEKEY necesario para ftok */

void manejador(int senal);

int estado_carrera = SIN_EMPEZAR;

int main(int argc, char **argv){
    Mensaje_Apostador *apuesta = NULL;
    int n_apostadores;
    int n_caballos;
    int caballo_aleat;
    int apuesta_aleat;
    int apostador_aleat;
    int key;
    int msqid;
    int shmid;
    int retorno_envio;
    int apuesta_maxima;
    sigset_t set, oset;
    Memoria_Compartida *memoria_compartida;
    
    if (argc < 3){
        printf("No suficientes parámetros en el ----> Apostador.");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) - getpid()); /*Seed para generación de números aleatorios*/
    
    n_apostadores = atoi(argv[1]);
    n_caballos = atoi(argv[2]);
    
    /* Calcula la key para la cola de mensajes*/
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        perror("Error al usar ftok");
        /*liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
        
    /* Consigue la cola de mensajes */
    msqid = msgget (key, IPC_EXCL | 0660);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes en el apostador");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    
    /* Crea la memoria compartida */
    shmid = reservashm(sizeof(Memoria_Compartida), key);
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
    
    
    /* Inicializa la mascara de bloqueos y crea el manejador*/
    
    if (sigfillset(&set) == -1) {
        perror("Error con sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SENALESTADOCARRERACAMBIA) == -1) {
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SENALINTERRUPCIONUSUARIO) == -1) { 
        perror("Error con sigdelset");    
        exit(EXIT_FAILURE);               
    }
    if (sigdelset(&set, SIGTERM) == -1) { 
        perror("Error con sigdelset");    
        exit(EXIT_FAILURE);               
    }
    if (sigprocmask (SIG_BLOCK, &set, &oset) == -1) {
        perror("Error con sigprocmask");
        exit(EXIT_FAILURE);
    }
    
    if(signal (SENALINTERRUPCIONUSUARIO, manejador) == SIG_ERR){
        perror("Error en el manejador");
        exit(EXIT_FAILURE);
    }
    
    if(signal (SENALESTADOCARRERACAMBIA, manejador) == SIG_ERR){
        perror("Error en el manejador");
        exit(EXIT_FAILURE);
    }
    
    
    /*Generación de Apuestas*/
    while (1){
        
        apuesta = (Mensaje_Apostador*)malloc(sizeof(Mensaje_Apostador));
        if (apuesta == NULL) {
            perror("Error al reservar el espacio para el mensaje en apostador");
            exit(EXIT_FAILURE);
        }
        apuesta->mtype = MENSAJE_APOSTADOR_A_GESTOR;
        apostador_aleat = aleat_num(0, n_apostadores-1);
        apuesta_maxima = memoria_compartida->apostadores[apostador_aleat].dinero_restante;
        apuesta_aleat = aleat_num(1, apuesta_maxima);
        
        if (apuesta_aleat != 0) {
        
            memoria_compartida->apostadores[apostador_aleat].dinero_restante = apuesta_maxima - apuesta_aleat;
            caballo_aleat = aleat_num(0, n_caballos - 1);
            
    
            strcpy(apuesta->info.nombre, memoria_compartida->apostadores[apostador_aleat].nombre);
            apuesta->info.numero_caballo = caballo_aleat;
            apuesta->info.cuantia = apuesta_aleat;
            
            
            
            
            
            /* Manda el mensaje al gestor*/
            retorno_envio = msgsnd (msqid, (struct Apuesta*) apuesta, sizeof(Apuesta) - sizeof(long), 0);
            
            if (retorno_envio == -1) {
                perror("Error al mandar el mensaje en el caballo");
                
                exit(EXIT_FAILURE);
            }
        }
        free(apuesta);
        
        sleep(0.1); /*Vermos mas adelanta si se llena el buffer para usar esto*/
    }
    
    exit(EXIT_SUCCESS);
}

void manejador(int senal){
    /*Liberar  todo como podamos*/
    
    
    exit(EXIT_SUCCESS);
}