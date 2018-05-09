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
    int apuesta_maxima;
    int n_caballos;
    char nombre_apostador[MAX_NAME];
    int caballo_aleat;
    int apuesta_aleat;
    int apostador_aleat;
    int key;
    int msqid;
    int retorno_envio;
    sigset_t set, oset;
    
    if (argc < 4){
        printf("No suficientes parámetros");
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
        perror("Error al obtener identificador para cola mensajes en el apostador");
        /** liberamos memoria y mas cosas ***************************************/
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
        apostador_aleat = aleat_num(1, n_apostadores);
        sprintf(nombre_apostador, "Apostador_%d", apostador_aleat);
        caballo_aleat = aleat_num(1, n_caballos);
        apuesta_aleat = aleat_num(0, apuesta_maxima);
        
        strcpy(apuesta->nombre, nombre_apostador);
        apuesta->numero_caballo = caballo_aleat;
        apuesta->cuantia = apuesta_aleat;
        
        
        /* Manda el mensaje al gestor*/
        retorno_envio = msgsnd (msqid, (struct Apuesta*) apuesta, sizeof(Apuesta) - sizeof(long), 0);
        
        if (retorno_envio == -1) {
            perror("Error al mandar el mensaje en el caballo");
            
            exit(EXIT_FAILURE);
        }
        sleep(1); /*Vermos mas adelanta si se llena el buffer para usar esto*/
    }
    
    exit(EXIT_SUCCESS);
}

void manejador(int senal){
    /*Liberar  todo como podamos*/
    
    
    exit(EXIT_SUCCESS);
}