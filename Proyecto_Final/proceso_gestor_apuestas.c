/**
 * @brief Proceseo Gestor del Proyecto Final de la Asignatura
 * 
 * Este Proceso es el encargado de gestionar todo lo relacionado con 
 * las apuestas sobre la carrera de caballos. Es el encargado de
 * inicializar las apuestas y posteriormente generar todos los hilos
 * que desempeñan la funcion de las ventanillas de apuestas.
 * 
 * @file proceso_gestor.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 4-5-2018
 */



/*
1- Inicializa las apuestas:
a. Total de dinero apostado a cada caballo = 1.0
b. Cotización de cada caballo = <total dinero apostado a todos los
caballos hasta ese momento> / <total del dinero apostado al caballo
hasta ese momento>
c. Dinero a pagar a cada apostador para cada caballo = 0
2- Inicializa tantos threads como ventanillas de gestión de apuestas
3- Recibe mensajes de apuestas en una cola
4- Los mensajes de apuestas que se van recibiendo son procesados por los
threads “ventanilla”
5- Sólo se procesan apuestas hasta el comienzo de la carrera. Está prohibido
procesar ninguna apuesta una vez comenzada la carrera.
6- Cada ventanilla:
a. Asume uno de los mensajes de apuesta
b. Comprueba el caballo de la apuesta
c. Se le asigna al apostador la cantidad que se le pagara en caso de que
el caballo gane = <dinero apostado> * <cotización del caballo>
d. Se actualiza la cotización de los caballos:
i. <Cotización de un caballo> = <total dinero apostado a todos
los caballos> / <total dinero apostado al caballo>
*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>


#include "estructuras.h"
#include "memoria_compartida.h"
#include "semaforos.h"

#define KEY 1300 /*!< KEY necesaria para el ftok */
#define FILEKEY "/bin/bash" /*!< FILEKEY necesario para ftok */
/**
 * @brief Funcion que desempeña la ejecucion de cada ventanilla
 * 
 * @return void
 * 
 */
void *ventanilla(void *n_ventanilla);

void manejador_interrupcion_usuario(int senal);
void manejador_vacio(int senal);

double *apostado;
double *cotizacion;
int msqid;
int estado_carrera = SIN_EMPEZAR;
int key;

int main (int argc, char **argv){ /*Primer parámetro, número de caballos*//*Segundo Parametro, nº ventanillas*/
    int i;
    int k;
    int n_caballos;
    int n_pthreads;
    pthread_t pthread_array[MAX_VENTANILLAS];
    sigset_t set, oset;
    int n_ventanilla;
    int *arg;
    int retorno_up, retorno_semaforo;
    int semid;

    if(argc<3){
        perror("Not enough parameters");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) - getpid()); /*Seed para generación de números aleatorios*/
    
    n_caballos = atoi(argv[1]);
    n_pthreads = atoi(argv[2]);
    
    /*Inicialización*/
    apostado = (double *)malloc(n_caballos*sizeof(double));
    if (apostado == NULL){
        perror("Error en la reserva de memoria 1-apostado");
        exit(EXIT_FAILURE);
    }
    cotizacion  = (double *)malloc(n_caballos*sizeof(double));
    if (cotizacion == NULL){
        perror("Error en la reserva de memoria 1-apostado");
        free(apostado);
        exit(EXIT_FAILURE);
    }
    
    /* Consigue el array de semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUM_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al obtener semid");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    
    /*Inicializa la máscara y crea el manejador*/
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
    
    if(signal (SENALINTERRUPCIONUSUARIO, manejador_interrupcion_usuario) == SIG_ERR){
        perror("Error en el manejador");
        exit(EXIT_FAILURE);
    }
    
    if(signal (SENALESTADOCARRERACAMBIA, manejador_vacio) == SIG_ERR){
        perror("Error en el manejador");
        exit(EXIT_FAILURE);
    }
    
    
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
        perror("Error al obtener identificador para cola mensajes en el gestor");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }

    /*************************El array de cotizacion y apostado va de 1 al numero de caballos*/
    apostado[0] = 0;
    cotizacion[0] = 0;
    
    for (i = 1; i <= n_caballos; i++){
        apostado[1] = 1.0;
        cotizacion[1] = n_caballos;
    }
    
    for (n_ventanilla = 0; n_ventanilla<n_pthreads; n_ventanilla++){
        arg = (int*)malloc(sizeof(*arg));
        if ( arg == NULL ) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }
        *arg = n_ventanilla;
        pthread_create(&pthread_array[n_ventanilla], NULL, ventanilla , arg); /*Numeradas a partir del 1*/
        free(arg);
    }
    
    pause();
    
    /*Cuando la carrera vaya a empezar*/
    for (k =0; k< n_pthreads; k++){
        /* pthread_join(pthread_array[k], NULL); */
        pthread_cancel(pthread_array[k]);
    }
    
    /* Espera a la siguiente senal */
    pause();
    
    /* Calcula los beneficios de todos los apostadores */
    
    /* Calcula los 10 apostadores con mas beneficios */
    
    
    
    /* Hace up del semaforo en el que esta esperando el proceso monitor */
    retorno_up = Up_Semaforo(semid, MUTEX_BENEFICIOS_CALCULADOS, SEM_UNDO);
    if (retorno_up == ERROR) {
        perror("Error al hacer up en el gestor de apuestas");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

void *ventanilla(void* n_ventanilla){
    int caballo;
    char nombre_apostador[MAX_NAME];
    int shmid;
    int retorno_recepcion;
    double cotizacion_posterior;
    double cotizacion_previa;
    double cantidad;
    double total;
    Memoria_Compartida *memoria_compartida;
    Mensaje_Apostador *mensaje;
    int ventanilla = *((int *) n_ventanilla);
    
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

    
    while (1){
        
        mensaje = (Mensaje_Apostador*)malloc(sizeof(Mensaje_Apostador*));
        if (mensaje == NULL) {
            /***********************************************************/
        }
        
        retorno_recepcion = msgrcv (msqid, (Mensaje_Apostador*) mensaje, sizeof(Mensaje_Apostador) - sizeof(long), MENSAJE_APOSTADOR_A_GESTOR, 0);
        
        if (retorno_recepcion == -1) {
            perror("Error al recibir el mensaje en el proceso B");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            exit(EXIT_FAILURE);
        }
        
        printf("Proceso Gestor -----> Mensaje Recibido: N_Caballo-> %d, Cantidad-> %f, Nombre-> %s\n ", mensaje->numero_caballo, mensaje->cuantia, mensaje->nombre);
        
        caballo = mensaje->numero_caballo;
        cotizacion_previa = cotizacion[caballo];
        cantidad = mensaje->cuantia;
        total = cotizacion_previa * apostado[caballo];
        cotizacion_posterior = (total + cantidad)/(apostado[caballo] + cantidad);
        apostado[caballo] = apostado[caballo] + cantidad;
        strcpy(nombre_apostador, mensaje->nombre);
         
         
         
        /*Guardamos datos en Memoria Compartida*/
        memoria_compartida->n_apuestas ++;
        memoria_compartida->caballos[caballo].total_apostado = apostado[caballo];
        memoria_compartida->caballos[caballo].cotizacion = cotizacion_posterior;
        strcpy(memoria_compartida->historial_apuestas->nombre, nombre_apostador);
        memoria_compartida->historial_apuestas->numero_caballo = caballo;
        memoria_compartida->historial_apuestas->cuantia = cantidad;
        memoria_compartida->historial_apuestas->ventanilla = ventanilla;
        
        
        /* Liberamos la estructura del mensaje */
        free(mensaje);
    }
    
    
}

void manejador_vacio(int senal){}

void manejador_interrupcion_usuario(int senal) {
    
}