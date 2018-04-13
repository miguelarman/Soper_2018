#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "semaforos.h"
#include "aleat_num.h"

#define SEGUNDOS(X) (X) * 1000000 /*!< Macro para tranformar segundos a microsegundos*/
#define MAX_WAIT 5
#define MIN_WAIT 1
#define KEY 1300
#define TAMANIO_NOMBRE 80
#define SEMAFORO_ENTRADA 0
#define SEMAFORO_SHM 1
#define FILEKEY "/bin/bash"


/* Estructuras */
typedef struct info{
    char nombre[TAMANIO_NOMBRE];
    int id;
} Informacion;

/* Funciones privadas*/
void manejador(int senal);
int reservashm(int size, int key);
void ejecucionHijo();

/* Variables globales */


/*FUNCION MAIN*/

int main(int argc, char **argv){
    int n_proc;
    int i;
    int shmid;
    int semid;
    int numero_procesos_hijo_terminados;
    int key;
    int retorno_semaforos;
    Informacion *info;
    sigset_t set, oset;
    pid_t child_pid;
    
    if (argc < 2){
        printf("Not enough parameters, %d", argc);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    
    /* Key para la memoria compartida */
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error con la key \n");
        exit(EXIT_FAILURE);
    }
    
    /*Numero de procesos*/
    n_proc = atoi(argv[1]);
    
    numero_procesos_hijo_terminados = 0;
    
    /* Crea los semaforos */
    retorno_semaforos = Crear_Semaforo(key, 2, &semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al crear los semaforos");
        exit(EXIT_FAILURE);
    } else if (retorno_semaforos != 0) {
        perror("No se han creado los semaforos en el padre");
    }
    
    for (i = 0; i<n_proc; i++){
        child_pid = fork();
        
        if (child_pid == -1){
            perror("Error al crear un hijo");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) {
            /*Código del hijo*/
            ejecucionHijo(key);
            
        }else{
            /*Código del Padre*/
            continue;
        }
    }
    
    /* Consigue la memoria compartida*/
    
    shmid = reservashm(sizeof(Informacion*), key);
    if (shmid == -1) {
        perror("Error al conseguir la memoria compartida en el hijo");
        exit(EXIT_FAILURE);
    }
    info = shmat (shmid, (char *)0, 0);
    
    /* Inicializa el id a 1 */
    info->id = 1;
    
    /* Inicializa la mascara de bloqueos y crea el manejador de SIGUSR1*/
    
    if (sigfillset(&set) == -1) {
        perror("Error con sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SIGUSR1) == -1) {
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SIGKILL) == -1) {
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigprocmask (SIG_BLOCK, &set, &oset) == -1) {
        perror("Error con sigprocmask");
        exit(EXIT_FAILURE);
    }
    
    if(signal (SIGUSR1, manejador) == SIG_ERR){
        perror("Error en el manejador");
        exit(EXIT_FAILURE);
    }
    
    while(1) {
        /* Espera a una senal SIGUSR1 */
        pause();

        /*Imprime la informacion*/
        /* Hace down del semaforo de la memoria compartida*/
        retorno_semaforos = Down_Semaforo(semid, SEMAFORO_SHM, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer down en los semaforos en el padre");
            exit(EXIT_FAILURE);
        }
        printf("Nombre de usuario: %s\nIdentificador: %d", info->nombre, info->id);
        fflush(stdout);
        
        /*up de los dos semaforos*/
        /* Hace up del semaforo de la memoria compartida*/
        retorno_semaforos = Up_Semaforo(semid, SEMAFORO_SHM, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer down en los semaforos en el padre");
            exit(EXIT_FAILURE);
        }
        /* Hace up del semaforo de la entrada*/
        retorno_semaforos = Up_Semaforo(semid, SEMAFORO_ENTRADA, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer down en los semaforos en el padre");
            exit(EXIT_FAILURE);
        }
        
        
        numero_procesos_hijo_terminados++;
        
        if (numero_procesos_hijo_terminados == n_proc){
            break;
        }
    }
    
    while(wait(NULL)>0);
    
    /*Eliminamos la memoria compartida*/
    
    exit(EXIT_SUCCESS);
}

/******************************************************************************/
/**********************     FUNCIONES PRIVADAS     ****************************/
/******************************************************************************/

void manejador(int senal){}

int reservashm(int size, int key) {
    int shmid_reserva;
    shmid_reserva = shmget (key, size, IPC_CREAT | IPC_EXCL |SHM_R | SHM_W);
    
    if(shmid_reserva == -1) {
        shmid_reserva = shmget(key, size, 0);
    }
    
    return shmid_reserva;
}

void ejecucionHijo(int key){
    Informacion *info;
    int shmid_hijo;
    int semid;
    int retorno_semaforos;
    
    /*Consigue la memoria compartida*/
    
    shmid_hijo = reservashm(sizeof(Informacion*), key);
    if (shmid_hijo == -1) {
        perror("Error al conseguir la memoria compartida en el hijo");
        exit(EXIT_FAILURE);
    }
    info = shmat (shmid_hijo, (char *)0, 0);
    
    /* Consigue los semaforos */
    retorno_semaforos = Crear_Semaforo(key, 2, &semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al obtener los semaforos creados en el hijo");
        exit(EXIT_FAILURE);
    } else if (retorno_semaforos != 1) {
        perror("Los semaforos creados no se han obtenido en los hijos");
        exit(EXIT_FAILURE);
    }
    
    /* Duerme durante un tiempo aleatorio */
    sleep(SEGUNDOS(aleat_num(MIN_WAIT, MAX_WAIT)));
    
    /* Pidel nombre del usuario*/
    
    /* Hace down del semaforo de la entrada*/
    retorno_semaforos = Down_Semaforo(semid, SEMAFORO_ENTRADA, SEM_UNDO);
    if (retorno_semaforos == ERROR) {
        perror("Error al hacer down en los semaforos en el hijo");
        exit(EXIT_FAILURE);
    }
    
    printf("\nIntroduzca el nombre del cliente: ");
    fflush(stdout);
    /* Hace down del semaforo de la memoria compartida*/
    retorno_semaforos = Down_Semaforo(semid, SEMAFORO_SHM, SEM_UNDO);
    if (retorno_semaforos == ERROR) {
        perror("Error al hacer down en los semaforos en el hijo");
        exit(EXIT_FAILURE);
    }
    fgets(info->nombre, TAMANIO_NOMBRE, stdin);
    info->id++;
    /* Hace up del semaforo de la memoria compartida*/
    retorno_semaforos = Up_Semaforo(semid, SEMAFORO_SHM, SEM_UNDO);
    if (retorno_semaforos == ERROR) {
        perror("Error al hacer down en los semaforos en el hijo");
        exit(EXIT_FAILURE);
    }
    
    /*Manda la senal al padre*/
    kill(getppid(), SIGUSR1);
    
    /* Borra la memoria compartida */
    /* Borra los semaforos*/
    
    exit(EXIT_SUCCESS);
}