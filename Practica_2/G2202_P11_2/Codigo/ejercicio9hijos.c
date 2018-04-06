/**
 * @brief Ejercicio 9 de la Práctica (bis)
 * 
 * Este programa es ejecutado por los procesos hijos
 * creados en el ejercicio9. Su ejecución consiste en
 * leer una serie de operaciones de un fichero, y mandar
 * señales al proceso padre, bien cuando exceden un saldo
 * máximo, o cuando han terminado de leer todas las señales
 * 
 * @file ejercicio9hijos.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 6-4-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "semaforos.h"
#include "aleat_num.h"

#define SEGUNDOS(X) (X) * 1000000 /*!< Macro para tranformar segundos a microsegundos*/
#define MAX_TAM 256 /*!< Máximo tamaño de las paths*/
#define KEY 15 /*!< Key precompartida de los semáforos*/
#define N_CAJAS 5 /*!< Número de cajas*/


/**
 * @brief Función principal del programa
 *
 * Este programa lee operaciones de un fichero, y va actualizando
 * otro fichero donde guarda su saldo. Cuando excede los 1000
 * euros de saldo manda una señal a su padre para que le retire
 * el excedente, y cuando termina, avisa también al padre, para
 * que retire lo que quede de dinero
 * 
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main (int argc, char ** argv) {
    char fichero_operaciones[MAX_TAM], fichero_saldo[MAX_TAM];
    int id;
    int numero_operaciones;
    int cantidad;
    FILE *pf_operaciones, *pf_saldo;
    pid_t padre_id;
    int i;
    int saldo;
    int retorno_senial;
    int semid;
    int retorno_semaforos;
    
    if (argc < 3) {
        printf("Error en los argumentos de un hijo. Debe especificar el id y el numero de operaciones");
        exit(EXIT_FAILURE);
    }
    
    id = atoi(argv[1]);
    numero_operaciones = atoi(argv[2]);
    
    padre_id = getppid();
    
    sprintf(fichero_operaciones, "files/clientesCaja%d.txt", id);
    if (fichero_operaciones == NULL) {
        perror ("Error al crear la path al fichero de operaciones de algun hijo");
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    }
    
    sprintf(fichero_saldo, "files/saldoCaja%d.txt", id);
    if (fichero_saldo == NULL) {
        perror ("Error al crear la path al fichero de saldo de algun hijo");
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    }
    
    /*Obtiene los semaforos creados por el padre*/
    
    retorno_semaforos = Crear_Semaforo(KEY, N_CAJAS, &semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al obtener los semaforos creados en el hijo");
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    } else if (retorno_semaforos != 1) {
        perror("Los semaforos creados no se han obtenido en los hijos");
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    }
    
    /*Pone su saldo inicial a cero*/
    
    retorno_semaforos = Down_Semaforo(semid, id - 1, SEM_UNDO);
    if (retorno_semaforos == ERROR) {
        perror("Error al hacer down en los semaforos en el hijo");
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    }
    
    pf_saldo = fopen(fichero_saldo, "w");
    if (pf_saldo == NULL) {
        printf("No se pudo abrir el fichero %s", fichero_saldo);
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    }
    saldo = 0;
    fwrite(&saldo, sizeof(int), 1, pf_saldo);
    fclose(pf_saldo);
        
    retorno_semaforos = Up_Semaforo(semid, id - 1, SEM_UNDO);
    if (retorno_semaforos == ERROR) {
        perror("Error al hacer u pen los semaforos en el hijo");
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    }
    
    
    pf_operaciones = fopen(fichero_operaciones, "r");
    if (pf_operaciones == NULL) {
        printf("No se pudo abrir el fichero %s", fichero_operaciones);
        kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
    }
    
    
    for (i = 0; i < numero_operaciones; i++) {
        
        /*Lee una operación*/
        
        fread(&cantidad, sizeof(int), 1, pf_operaciones);
        
        /*Espera aleatoria*/
        
        usleep(SEGUNDOS(aleat_num(1, 5)));
        
        /*Lee el saldo*/
        
        retorno_semaforos = Down_Semaforo(semid, id - 1, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer down en los semaforos en el hijo");
            kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
        }
        
        pf_saldo = fopen(fichero_saldo, "r");
        if (pf_saldo == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_saldo);
            kill(padre_id, SIGRTMIN);
            exit(EXIT_FAILURE);
        }
        fread(&saldo, sizeof(int), 1, pf_saldo);
        fclose(pf_saldo);
    
        /*Guarda el nuevo saldo*/
        
        saldo += cantidad;
        
        pf_saldo = fopen(fichero_saldo, "w");
        if (pf_saldo == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_saldo);
            kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
        }
        fwrite(&saldo, sizeof(int), 1, pf_saldo);
        fclose(pf_saldo);
        
        /*Comprueba si tiene mas de 1000 euros*/
        
        if (saldo >= 1000) {
            /*Manda señal SIGRTMIN+1 al padre para que retire el dinero*/
            
            retorno_senial = kill(padre_id, SIGRTMIN+1);
            if (retorno_senial == -1) {
                perror("Error al mandar señal SIGRTMIN+1 al padre");
                
                kill(padre_id, SIGRTMIN);
                exit(EXIT_FAILURE);
            }
        }
        
        retorno_semaforos = Up_Semaforo(semid, id - 1, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer up en los semaforos en el hijo");
            kill(padre_id, SIGRTMIN);
        exit(EXIT_FAILURE);
        }
    }
    
    fclose(pf_operaciones);
    
    
    /*Avisa al padre de que ha terminado con la señal SIGUSR2*/
    
    retorno_senial = kill(padre_id, SIGRTMIN);
    if (retorno_senial == -1) {
        perror("Error al mandar señal SIGRTMIN al padre");
        
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}