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
#define MAX_WAIT 10
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
    int i, j;
    int shmid;
    int semid;
    int numero_procesos_hijo_terminados;
    int key;
    int retorno_semaforos, retorno_shmctl, retorno_shmdt;
    Informacion *info;
    sigset_t set, oset;/*, set_usr1, set_completo;*/
    pid_t child_pid;
    Informacion **datos;
    unsigned short unos[2];
    
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
    
    /* Inicializa los semaforos a 1 */
    unos[0] = 1;
    unos[1] = 1;
    retorno_semaforos = Inicializar_Semaforo(semid, unos);
    if (retorno_semaforos == ERROR) {
        perror("Error al inicializar los semaforos");
        exit(EXIT_FAILURE);
    }
    
    /* Crea la memoria compartida */
    shmid = reservashm(sizeof(Informacion*), key);
    if (shmid == -1) {
        perror("Error al conseguir la memoria compartida en el hijo");
        exit(EXIT_FAILURE);
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
    
    info = shmat (shmid, (char *)0, 0);
    
    /* Inicializa el id a 0 */
    info->id = 0;
    
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
    
    /* Crea un set con todas las senales a bloquear despues del pause */
    /*if (sigfillset(&set_completo) == -1) {
        perror("Error con sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set_completo, SIGTERM) == -1) {
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }*/
    
    /* Crea un set con sigusr1 y sigterm para desbloquearlas */
    /*if (sigemptyset(&set_usr1) == -1) {
        perror("Error con sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&set_usr1, SIGUSR1) == -1) {
        perror("Error con sigaddset");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&set_usr1, SIGTERM) == -1) {
        perror("Error con sigaddset");
        exit(EXIT_FAILURE);
    }*/
    
    /* Crea una estructura para almacenar los datos que va a recibir */
    datos = (Informacion **)malloc(n_proc * sizeof(Informacion *));
    if (datos == NULL) {
        printf("Error al inicializar la estructura");
        while(wait(NULL) > 0);
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < n_proc; i++) {
        datos[i] = (Informacion *)malloc(sizeof(Informacion));
        if (datos[i] == NULL) {
            printf("Error al inicializar la estructura");
            while(wait(NULL) > 0);
            for (j = 0; j < i; j++) {
                free(datos[i]);
            }
            free(datos);
            exit(EXIT_FAILURE);
        }
    }
    
    while(1) {
        /* Espera a una senal SIGUSR1 */
        /*pause();*/
        sigsuspend(&set);
        
        /* Bloquea todas las senales */
        /*if (sigprocmask (SIG_BLOCK, &set_completo, &oset) == -1) {
            perror("Error con sigprocmask");
            exit(EXIT_FAILURE);
        }*/
        

        /*Imprime la informacion*/
        /* Hace down del semaforo de la memoria compartida*/
        retorno_semaforos = Down_Semaforo(semid, SEMAFORO_SHM, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer down en los semaforos en el padre");
            exit(EXIT_FAILURE);
        }
        
        strcpy(datos[numero_procesos_hijo_terminados]->nombre, info->nombre);
        datos[numero_procesos_hijo_terminados]->id = info->id;
        
        /*DEBUGGING*//*printf("\nnombre %s id %d", info->nombre, info->id);fflush(stdout);*/
        /*DEBUGGING*//*printf("\nnombre %s id %d", datos[numero_procesos_hijo_terminados]->nombre, datos[numero_procesos_hijo_terminados]->id);fflush(stdout);*/
        
        numero_procesos_hijo_terminados++;
        
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
        
        if (numero_procesos_hijo_terminados == n_proc){
            break;
        }
        
        /* Desbloquea la senal SIGUSR1 */
        /*if (sigprocmask (SIG_UNBLOCK, &set_usr1, &oset) == -1) {
            perror("Error con sigprocmask");
            exit(EXIT_FAILURE);
        }*/
    }
    
    /* Imprime todos los datos */
    for (i = 0; i< n_proc; i++) {
        printf("\nNombre de usuario: %sIdentificador: %d", datos[i]->nombre, datos[i]->id);
        fflush(stdout);
        
        /* Liberamos los datos de ese usuario */
        free(datos[i]);
    }
    
    while(wait(NULL)>0);
    
    /* Liberamos la estructura con los datos */
    free(datos);
    
    /*Eliminamos la memoria compartida*/
    retorno_shmdt = shmdt ((char *)info);
    if (retorno_shmdt == -1) {
        perror("Error al hacer el dettach en el padre");
        exit(EXIT_FAILURE);
    }
    retorno_shmctl = shmctl (shmid, IPC_RMID, (struct shmid_ds *)NULL);
    if (retorno_shmctl == -1) {
        perror("Error al borrar la memoria en el padre");
        exit(EXIT_FAILURE);
    }
    
    /* Eliminamos los semaforos */
    retorno_semaforos = Borrar_Semaforo(semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al borrar los semaforos en el padre");
        exit(EXIT_FAILURE);
    }
        
        
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
    int retorno_semaforos, retorno_shmdt;
    sigset_t set, oset;
    
    /* Bloquea las senales */
    
    if (sigfillset(&set) == -1) {
        perror("Error con sigfillset");
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
    sleep(aleat_num(MIN_WAIT, MAX_WAIT));
    
    /*****************************/
    /* Pide el nombre del usuario*/
    /*****************************/
    
    /* 1- Hace down del semaforo de la entrada*/
    retorno_semaforos = Down_Semaforo(semid, SEMAFORO_ENTRADA, SEM_UNDO);
    if (retorno_semaforos == ERROR) {
        perror("Error al hacer down en los semaforos en el hijo");
        exit(EXIT_FAILURE);
    }
    
    /* 2- Pregunta por terminal */
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
    
    /* 3- Hace up del semaforo de la memoria compartida*/
    retorno_semaforos = Up_Semaforo(semid, SEMAFORO_SHM, SEM_UNDO);
    if (retorno_semaforos == ERROR) {
        perror("Error al hacer down en los semaforos en el hijo");
        exit(EXIT_FAILURE);
    }
    
    /*Manda la senal al padre*/
    kill(getppid(), SIGUSR1);
    
    /*Eliminamos la memoria compartida*/
    retorno_shmdt = shmdt ((char *)info);
    if (retorno_shmdt == -1) {
        perror("Error al hacer el dettach en los hijos");
        exit(EXIT_FAILURE);
    }
    /*retorno_shmctl = shmctl (shmid_hijo, IPC_RMID, (struct shmid_ds *)NULL);
    if (retorno_shmctl == -1) {
        perror("Error al borrar la memoria en los hijos");
        exit(EXIT_FAILURE);
    }*/
    
    /* Eliminamos los semaforos */
    /*retorno_semaforos = Borrar_Semaforo(semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al borrar los semaforos en los hijos");
        exit(EXIT_FAILURE);
    }*/
    
    exit(EXIT_SUCCESS);
}