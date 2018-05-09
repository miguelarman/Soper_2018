/**
 * @brief Proceso Gestor del Proyecto Final de la Asignatura
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

int msqid;
int estado_carrera = SIN_EMPEZAR;
int key;
int n_caballos;

int main (int argc, char **argv){ /*Primer parámetro, número de caballos*//*Segundo Parametro, nº ventanillas*/
    int k;
    /*int j;
    int i;*/
    int n_pthreads;
    /*Datos_Apostador swap;*/
    pthread_t pthread_array[MAX_VENTANILLAS];
    sigset_t set, oset;
    int n_ventanilla;
    int *arg;
    int retorno_up, retorno_semaforo;
    int semid, shmid;
    /*double cantidad_a_pagar;
    int id_apostador;*/
    Memoria_Compartida *memoria_compartida;

    if(argc<3){
        perror("Not enough parameters");
        exit(EXIT_FAILURE);
    }
    
    
    srand(time(NULL) - getpid()); /* Seed para generación de números aleatorios */
    
    n_caballos = atoi(argv[1]);
    n_pthreads = atoi(argv[2]);
    
    
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
    if (sigdelset(&set, SIGTERM) == -1) { 
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
        exit(EXIT_FAILURE);
    }
        
     /* Crea la cola de mensajes */
    msqid = msgget (key, IPC_EXCL | 0660);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes en el gestor");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue el array de semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUM_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al obtener semid");
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
        perror("Error al conseguir la memoria compartida en el gestor");
        exit(EXIT_FAILURE);
    }
    
    for (n_ventanilla = 0; n_ventanilla < n_pthreads; n_ventanilla++){
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
    for (k = 0; k < n_pthreads; k++){
        /* pthread_join(pthread_array[k], NULL); */
        pthread_cancel(pthread_array[k]);
    }
    
    /* Espera a la siguiente senal */
    pause();
    
    /* El codigo comentado a continuacion es una implementacion de
     * bubblesort, cuya funcion era calcular cuales eran los diez apostadores con mayor beneficio
     * No podemos descomentarlo puesto que no nos funciona y causa un bloqueo, por
     * una razón que no nos ha dado tiempo a averiguar. Esta es la razon por la
     * que al final de la ejecucion se imprime que los apostadores mas beneficiados
     * son todos el Apostador_1
     */
    
    
    /* Calcula los beneficios de todos los apostadores */
    /*for (i = 0; i < memoria_compartida->n_apuestas; i++){
        for (j = 0; j < memoria_compartida->n_caballos_ganadores; j++) {
            if (memoria_compartida->historial_apuestas[i].numero_caballo == memoria_compartida->caballos_ganadores[j]){
                cantidad_a_pagar = memoria_compartida->historial_apuestas[i].cuantia * memoria_compartida->historial_apuestas[i].cotizacion_previa;
                
                sscanf(memoria_compartida->historial_apuestas[i].nombre, "Apostador_%d", &id_apostador);
                memoria_compartida->apostadores[id_apostador - 1].dinero_restante += cantidad_a_pagar;
            }
        }
    }*/
    
    
    /* Calcula los 10 apostadores con mas beneficios */
    
    /*for (i = 0 ; i < memoria_compartida->n_apostadores -1; i++){
        for (j = 0 ; j < memoria_compartida->n_apostadores - i - 1; j++){
            if (memoria_compartida->apostadores[j].dinero_restante > memoria_compartida->apostadores[j+1].dinero_restante){
                swap = memoria_compartida->apostadores[i];
                memoria_compartida->apostadores[j] = memoria_compartida->apostadores[i+1];
                memoria_compartida->apostadores[j+1] = swap;
            }
        }
    }*/
    
    
    /*for (i =0; i< 10; i++){
        sscanf(memoria_compartida->apostadores[i].nombre, "Apostador_%d", &(memoria_compartida->top_apostadores[i]));
    }*/
    

    /* Hace up del semaforo en el que esta esperando el proceso monitor */
    retorno_up = Up_Semaforo(semid, MUTEX_BENEFICIOS_CALCULADOS, SEM_UNDO);
    if (retorno_up == ERROR) {
        perror("Error al hacer up en el gestor de apuestas");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void *ventanilla(void* n_ventanilla){
    int caballo;
    char nombre_apostador[MAX_NAME];
    int shmid, semid;
    int retorno_recepcion;
    double cotizacion_posterior;
    double cantidad;
    double total;
    Memoria_Compartida *memoria_compartida;
    Mensaje_Apostador *mensaje;
    int ventanilla = *((int *) n_ventanilla);
    int retorno_down, retorno_up, retorno_semaforo;
    int i;

    /* Crea la memoria compartida */
    shmid = reservashm(sizeof(Memoria_Compartida), key);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue la memoria compartida*/
    memoria_compartida = shmat (shmid, (char *)0, 0);
    if (memoria_compartida == (void *) -1) {
        perror("Error al conseguir la memoria compartida en el gestor");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue los semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUM_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al conseguir los semaforos en el gestor");
        exit(EXIT_FAILURE);
    }
    
    mensaje = (Mensaje_Apostador*)malloc(sizeof(Mensaje_Apostador*));
    if (mensaje == NULL) {
        perror("Error al reservar para el mensaje en el gestor");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        
        retorno_recepcion = msgrcv (msqid, mensaje, sizeof(Info_apostador_mensaje), MENSAJE_APOSTADOR_A_GESTOR, MSG_NOERROR);
        
        if (retorno_recepcion == -1) {
            perror("Error al recibir el mensaje en el gestor");
            /* Libera la estructura del mensaje*/
            free(mensaje);
            exit(EXIT_FAILURE);
        }
        
        caballo = mensaje->info.numero_caballo;
        cantidad = mensaje->info.cuantia;
        
        /*Guardamos datos en Memoria Compartida*/
        retorno_down = Down_Semaforo(semid, MUTEX_GUARDAR_OFERTA, SEM_UNDO);
        if (retorno_down == ERROR) {
            perror("Error al hacer down en el gestor");
            exit(EXIT_FAILURE);
        }
        
        total = 0.0;
        for (i = 0; i < n_caballos; i++) {
            total += memoria_compartida->caballos[i].total_apostado;
        }
        cotizacion_posterior = (total + cantidad)/(memoria_compartida->caballos[caballo].total_apostado + cantidad);
        strcpy(nombre_apostador, mensaje->info.nombre);
        
        
        /* Guardamos en memoria compartida la nueva apuesta */
        memoria_compartida->caballos[caballo].total_apostado = memoria_compartida->caballos[caballo].total_apostado + cantidad;
        memoria_compartida->caballos[caballo].cotizacion = cotizacion_posterior;
        strcpy(memoria_compartida->historial_apuestas[memoria_compartida->n_apuestas].nombre, nombre_apostador);
        memoria_compartida->historial_apuestas[memoria_compartida->n_apuestas].cotizacion_previa = cotizacion_posterior;
        memoria_compartida->historial_apuestas[memoria_compartida->n_apuestas].numero_caballo = caballo;
        memoria_compartida->historial_apuestas[memoria_compartida->n_apuestas].cuantia = cantidad;
        memoria_compartida->historial_apuestas[memoria_compartida->n_apuestas].ventanilla = ventanilla;
        
        memoria_compartida->n_apuestas++;
        
        retorno_up = Up_Semaforo(semid, MUTEX_GUARDAR_OFERTA, SEM_UNDO);
        if (retorno_up == ERROR) {
            perror("Error al hacer down en el gestor");
            exit(EXIT_FAILURE);
        }
    }
    
    /* Liberamos la estructura del mensaje */
        free(mensaje);
    
}

void manejador_vacio(int senal){}

void manejador_interrupcion_usuario(int senal) {
    
}