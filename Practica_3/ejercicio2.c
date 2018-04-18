/**
 * @brief Ejercicio 2 de la Práctica
 * 
 * En este ejercicio creamos una serie de procesos que, 
 * mediante memoria compartida y la terminal preguntan
 * al usuario una serie de nombres, y guardan la informacion
 * de los usuarios
 * 
 * @file ejercicio2.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 17-4-2018
 */
 
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

#define MAX_WAIT 10 /*!< Máxima espera de los procesos (en segundos) */
#define MIN_WAIT 1 /*!< Míima espera de los procesos (en segundos) */
#define KEY 1300 /*!< Key para ftok */
#define TAMANIO_NOMBRE 80 /*!< Tamaño del campo del nombre en memoria compartida */
#define FILEKEY "/bin/bash" /*!< Filekey para ftok */


/* Estructuras */

/**
 * @brief Estructura que almacena la información de un usuario
 */
typedef struct info{
    char nombre[TAMANIO_NOMBRE]; /*!< Nombre del usuario */
    int id; /*!< Identificador del usuario */
} Informacion;


/* Funciones privadas*/

/**
 * @brief Manejador de la señal
 *
 * Esta función es llamada cuando se recibe la señal 
 * SIGUSR1, y simplemente permite, mediante la llamada a pause(),
 * esperar a que reciba la señal, puesto que está vacía
 * 
 * @param senal Señal recibida
 * @return void
 */
void manejador(int senal);

/**
 * @brief Función que solicita memoria compartida.
 *
 * Esta función es llamada para solicitar memoria compartida.
 * Si el key especificado ya ha sido creado, devuelve la zona,
 * y si no la crea
 * 
 * @param size Tamaño en bytes de la zona deseada
 * @param key Key creada por ftok
 * @return Identificador de la zona de memoria compartida
 */
int reservashm(int size, int key);

/**
 * @brief Función que ejecuta el hijo
 *
 * Esta función engloba las acciones de los procesos hijos. Estas son
 * preguntar por terminal el nombre y guardarlo en memoria compartida
 * 
 * @param size Tamaño en bytes de la zona deseada
 * @param key Key creada por ftok
 * @return Identificador de la zona de memoria compartida
 */
void ejecucionHijo();

/*FUNCION MAIN*/



/**
 * @brief Función principal del programa
 *
 * Este programa coordina la ejecución de procesos hijos,
 * solicitan y almacenan información de usuarios
 * 
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main(int argc, char **argv){
    int n_proc;
    int i, j;
    int shmid;
    int numero_procesos_hijo_terminados;
    int key;
    int retorno_shmctl, retorno_shmdt;
    Informacion *info;
    sigset_t set, oset;/*, set_usr1, set_completo;*/
    pid_t child_pid;
    Informacion **datos;

    if (argc < 2){
        printf("Not enough parameters, %d\n", argc);
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
    
    /* Crea la memoria compartida */
    shmid = reservashm(sizeof(Informacion*), key);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
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
        perror("Error al inicializar la estructura");
        while(wait(NULL) > 0);
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < n_proc; i++) {
        datos[i] = (Informacion *)malloc(sizeof(Informacion));
        if (datos[i] == NULL) {
            perror("Error al inicializar la estructura");
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
        
        strcpy(datos[numero_procesos_hijo_terminados]->nombre, info->nombre);
        datos[numero_procesos_hijo_terminados]->id = info->id;
        
        /*DEBUGGING*//*printf("nombre %s id %d\n", info->nombre, info->id);fflush(stdout);*/
        /*DEBUGGING*//*printf("nombre %s id %d\n", datos[numero_procesos_hijo_terminados]->nombre, datos[numero_procesos_hijo_terminados]->id);fflush(stdout);*/
        
        numero_procesos_hijo_terminados++;
        
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
        printf("Nombre de usuario: %sIdentificador: %d\n", datos[i]->nombre, datos[i]->id);
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
    int retorno_shmdt;
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
    
    /* Duerme durante un tiempo aleatorio */
    sleep(aleat_num(MIN_WAIT, MAX_WAIT));
    
    /*****************************/
    /* Pide el nombre del usuario*/
    /*****************************/
    
    /* 2- Pregunta por terminal */
    printf("Introduzca el nombre del cliente: ");
    fflush(stdout);
    fgets(info->nombre, TAMANIO_NOMBRE, stdin);
    info->id++;
    
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