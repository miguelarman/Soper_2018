#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "memoria_compartida.h"
#include "estructuras.h"
#include "semaforos.h"

int estado_carrera = SIN_EMPEZAR;

void manejador_estado_carrera_cambia(int senal);
void manejador_interrupcion_usuario(int senal);
void manejador_vacio(int senal);


int main (int argc, char **argv) {
    Memoria_Compartida *memoria_compartida;
    sigset_t set, oset;
    int shmid, semid;
    int retorno_semaforo, retorno_down, retorno_up;
    key_t key;
    int i;
    char nombre_apostador[MAX_NAME];
    int segundos_restantes;
    int num_caballo, num_ventanilla, indice_apostador;
    int posicion_caballo, ultima_tirada_caballo;
    double cotizacion_caballo, total_apostado_caballo;
    double beneficios, cantidad_apostada, cotizacion_previa, dinero_restante, total_apostado;
    
    /* Lee los argumentos que le manda el proceso principal */
    key = atoi(argv[1]);
    
    /* Prepara los manejadores de las senales */  
    if (signal(SENALESTADOCARRERACAMBIA, manejador_estado_carrera_cambia) == SIG_ERR){
        perror("Error en el manejador SENALESTADOCARRERACAMBIA");
        exit(EXIT_FAILURE);
    }
    if (signal(SENALINTERRUPCIONUSUARIO, manejador_interrupcion_usuario) == SIG_ERR){
        perror("Error en el manejador SENALINTERRUPCIONUSUARIO");
        exit(EXIT_FAILURE);
    }
    if (signal(SENALTIEMPORESTANTE, manejador_vacio) == SIG_ERR){
        perror("Error en el manejador SENALTIEMPORESTANTE");
        exit(EXIT_FAILURE);
    }
    if (signal(SENALDATOSCARRERAACTUALIZADOS, manejador_vacio) == SIG_ERR){
        perror("Error en el manejador SENALDATOSCARRERAACTUALIZADOS");
        exit(EXIT_FAILURE);
    }
    
    /* Bloquea todas las senales menos las tres anteriores */
    if (sigfillset(&set) == -1) {
        perror("Error con sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SENALESTADOCARRERACAMBIA) == -1) {
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SENALTIEMPORESTANTE) == -1) {
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SENALDATOSCARRERAACTUALIZADOS) == -1) {
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
    
    
    /* Consigue el shmid */
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
    
    /* Consigue los semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUM_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al conseguir los semaforos en el monitor");
        exit(EXIT_FAILURE);
    }
    
    while(1) {
        
        pause();

        if (estado_carrera == SIN_EMPEZAR) {
            
            printf("\n\n");
            
            segundos_restantes = memoria_compartida->segundos_restantes;
            
            /* Imprimimos los segundos restantes para que empiece la carrera */
            printf("\nQuedan %d segundos para empezar la carrera", segundos_restantes);
            
            for (i = 0; i < memoria_compartida->n_caballos; i++) {
                
                /* Hace down del semaforo para leer de memoria */
                retorno_down = Down_Semaforo(semid, MUTEX_GUARDAR_OFERTA, SEM_UNDO);
                if (retorno_down == ERROR) {
                    perror("Error al hacer down en el monitor");
                    exit(EXIT_FAILURE);
                }
                
                /* Leemos de memoria compartida la cotización y el total apostado del caballo */
                cotizacion_caballo = memoria_compartida->caballos[i].cotizacion;
                total_apostado_caballo = memoria_compartida->caballos[i].total_apostado;
                
                printf("\n\tEl caballo %d se cotiza a %f con un total apostado de %f", i + 1, cotizacion_caballo, total_apostado_caballo);
                
                /* Hace up del semaforo para leer de memoria */
                retorno_up = Up_Semaforo(semid, MUTEX_GUARDAR_OFERTA, SEM_UNDO);
                if (retorno_up == ERROR) {
                    perror("Error al hacer up en el monitor");
                    exit(EXIT_FAILURE);
                }
            }
            fflush(stdout);
            
        } else if (estado_carrera == EMPEZADA) {
            
            printf("\n\n");
            
            for (i = 0; i < memoria_compartida->n_caballos; i++) {
                posicion_caballo = memoria_compartida->caballos[i].posicion;
                ultima_tirada_caballo = memoria_compartida->caballos[i].ultima_tirada;
                
                printf("\nEl caballo %d va en la posicion %d, y su ultima tirada ha sido de %d", i + 1, posicion_caballo, ultima_tirada_caballo);
            }
            fflush(stdout);
            
        } else if (estado_carrera == TERMINADA) {
            
            /* Espera a que el proceso gestor indique que ya estan calculados los beneficios y el top */
            retorno_down = Down_Semaforo(semid, MUTEX_BENEFICIOS_CALCULADOS, SEM_UNDO);
            if (retorno_down == ERROR) {
                perror("Error al hacer down en el monitor");
                exit(EXIT_FAILURE);
            }
            
            printf("\n\n");
            
            /* Imprimimos el resultado de la carrera */
            printf("\nEstado final de la carrera");
            for (i = 0; i < memoria_compartida->n_caballos; i++) {
                posicion_caballo = memoria_compartida->caballos[i].posicion;
                printf("\n\tEl caballo %d ha finalizado en la posicion %d", i, posicion_caballo);
                fflush(stdout);
            }
            
            printf("\nPor lo tanto los caballos ganadores son: ");
            for (i = 0; i < memoria_compartida->n_caballos_ganadores; i++) {
                printf("%d ", memoria_compartida->caballos_ganadores[i] + 1);
            }
            fflush(stdout);
            
            /* Listado de apuestas realizadas */
            printf("\n\nListado de apuestas realizadas:");
            fflush(stdout);
            for (i = 0; i < memoria_compartida->n_apuestas; i++) {
                
                strcpy(nombre_apostador, memoria_compartida->historial_apuestas[i].nombre);
                num_ventanilla = memoria_compartida->historial_apuestas[i].ventanilla;
                num_caballo = memoria_compartida->historial_apuestas[i].numero_caballo;
                cotizacion_previa = memoria_compartida->historial_apuestas[i].cotizacion_previa;
                cantidad_apostada = memoria_compartida->historial_apuestas[i].cuantia;
                
                
                printf("\n\tLa apuesta %d fue realizada por %s, procesada por la ventanilla %d, y aposto al caballo %d, con una cotizacion de %f un total de %f", i + 1, nombre_apostador, num_ventanilla, num_caballo, cotizacion_previa, cantidad_apostada);
                fflush(stdout);
            }
            
            /* Imprimimos el resultado de las apuestas */
            printf("\n\nLos apostadores con más beneficios son:");
            fflush(stdout);
            for (i = 0; i < 10; i++) {
                indice_apostador = memoria_compartida->top_apostadores[i];
                
                strcpy(nombre_apostador, memoria_compartida->apostadores[indice_apostador].nombre);
                beneficios = memoria_compartida->apostadores[indice_apostador].beneficios;
                printf("\n\tEl apostador en la posicion %d de mayores beneficios es el %s, con unos beneficios de %f", i + 1, nombre_apostador, beneficios);
                fflush(stdout);
            }
            
            /* Imprimimos el resultado de la carrera */
            printf("\n\nResultado de la carrera:");
            fflush(stdout);
            for (i = 0; i < memoria_compartida->n_caballos; i++) {
                posicion_caballo = memoria_compartida->caballos[i].posicion;
                printf("\n\tEl caballo %d ha finalizado en la posicion %d", i, posicion_caballo);
                fflush(stdout);
            }
            
            /* Imprimimos el estado final de cada apostador */
            printf("\n\nEstado final de los apostadores:");
            fflush(stdout);
            for (i = 0; i < memoria_compartida->n_apostadores; i++) {
                strcpy(nombre_apostador, memoria_compartida->apostadores[i].nombre);
                total_apostado = memoria_compartida->apostadores[i].cantidad_apostada;
                beneficios = memoria_compartida->apostadores[i].beneficios;
                dinero_restante = memoria_compartida->apostadores[i].dinero_restante;
                
                printf("\n\tEl apostador %s ha apostado un total de %f, obteniendo unos beneficios de %f, y con un dinero restante de %f", nombre_apostador, total_apostado, beneficios, dinero_restante);
                fflush(stdout);
            }
            
            
            printf("\n\nSIMULACION COMPLETADA");
            fflush(stdout);
            
            exit(EXIT_SUCCESS);
        }
    }
    
        
    exit(EXIT_SUCCESS);
}


/**************************************************************/
/******************* MANEJADORES DE SENALES *******************/
/**************************************************************/
void manejador_estado_carrera_cambia(int senal) {
    
    if (estado_carrera == SIN_EMPEZAR) {
        printf("\n\nLA CARRERA HA EMPEZADO");
        estado_carrera = EMPEZADA;
    } else {
        printf("\n\nLA CARRERA HA TERMINADO");
        estado_carrera = TERMINADA;
    }
}

void manejador_interrupcion_usuario(int senal) {
    /* TODO */
}

void manejador_vacio(int senal) {}