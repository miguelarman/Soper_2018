/**
 * @brief Ejercicio 9 de la Práctica
 * 
 * En este ejercicio creamos una serie de procesos hijos,
 * que realizan cálculos simulando una caja de supermercado,
 * y que mandan señales al proceso padre cuando exceden los 1000
 * euros o cuando terminan. Se implementa el uso de semáforos
 * 
 * @file ejercicio9.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 6-4-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#include "semaforos.h"
#include "aleat_num.h"

#define N_CAJAS 5 /*!< Numero de cajas*/
#define TAM_PATH 256 /*!< Maximo tamaño de las path de ficheros*/
#define N_OPERACIONES 50 /*!< Numero de operaciones a realizar por los cajeros*/
#define FICHERO_SALDO_TOTAL "files/saldoTotal.txt" /*!< Path del fichero con el saldo total*/
#define TAMANIO_ARGV_HIJOS 128 /*!< Tamaño máximo de los argumentos que se pasan a los hijos*/
#define KEY 15 /*!< Key precompartida por los procesos. Al utilizar exec no podemos usar IPC_PRIVATE*/


/**
 * @brief manejador de la señal SIGRTMIN+1
 *
 * Esta funcion es ejecutada cuando el padre recibe
 * la señal SIGRTMIN+1. Esto ocurre cuando un proceso
 * tiene acumulado más de 1000. En ese momento, como
 * el padre no puede saber quién le ha mandado la señal,
 * recorre la lista de ficheros de saldos hasta que encuentra
 * uno con excedentes, y lo actualiza
 * 
 * @param senal Código de la señal recibida
 * @return void
 */
void manejador_usr1 (int senal);

/**
 * @brief manejador de la señal SIGRTMIN
 *
 * Esta funcion es ejecutada cuando el padre recibe
 * la señal SIGRTMIN. Esto ocurre cuando un proceso
 * ha finalizado su ejecución, tras haber leído todas
 * las operaciones. Como tampoco podemos saber qué proceso 
 * la ha lanzado, el padre espera hasta que terminan todos
 * los procesos, y recorre los ficheros actualizando el saldo
 * 
 * @param senal Código de la señal recibida
 * @return void
 */
void manejador_usr2 (int senal);

/*Variables globales. Deben ser globales, puesto que se
utilizan en más de una función, y si no, no podrían ser modificadas*/

sigset_t set, oset; /*!< Máscaras de bloqueo y desbloqueo de señales */
int num_procesos_terminados; /*!< Numero de procesos terminados */


/**
 * @brief Función principal del programa
 *
 * Este programa al iniciarse escribe en N_CAJAS ficheros
 * un número de operaciones aleatorias. Después, crea 5
 * procesos hijos, que ejecutan el programa ejercicio9hijos.
 * Entonces se pone a la espera de las señales SIGRTMIN y
 * SIGRTMIN+1, con las cuales actualiza el saldo total del
 * supermercado
 * 
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier
 * otro caso
 */
int main () {
    FILE *pf;
    char aux[TAM_PATH];
    int i, j;
    int v_aleat;
    pid_t child_pid;
    char *argv_hijos_id, *argv_hijos_num_operaciones;
    char fichero_saldo[TAM_PATH];
    int saldo, saldo_total;
    FILE *pf_saldo, *pf_saldo_total;
    int semid;
    unsigned short *array;
    int retorno_semaforos;


    /*El padre genera N ficheros*/
    for (i = 1; i <= N_CAJAS; i++){
        sprintf(aux, "files/clientesCaja%d.txt", i);
        if (aux == NULL){
            perror("Error al crear la path");
            exit(EXIT_FAILURE);
        }
        
        pf = fopen (aux, "w");
        if (pf == NULL) {
            perror("Error al abrir algun fichero para inicializarlo");
            exit(EXIT_FAILURE);
        }
        
        for (j = 0; j < N_OPERACIONES; j++){
            v_aleat = aleat_num(0, 300);
            fwrite(&v_aleat, sizeof(int), i, pf);
        }
        
        fclose(pf);
    }
    
    /*Inicializa el saldo total a 0*/
    saldo_total = 0;
    
    pf_saldo_total = fopen(FICHERO_SALDO_TOTAL, "w");
    fwrite(&saldo_total, sizeof(int), 1, pf_saldo_total);
    fclose(pf_saldo_total);

    
    /*Prepara los manejadores de las señales que espera*/
    
    if(signal (SIGRTMIN+1, manejador_usr1) == SIG_ERR){
        perror("Error en el manejador SIGRTMIN+1");
        exit(EXIT_FAILURE);
    }
    
    if(signal (SIGRTMIN, manejador_usr2) == SIG_ERR){
        perror("Error en el manejador sigusr2");
        exit(EXIT_FAILURE);
    }
    
    /*Crea los semaforos*/
    
    retorno_semaforos = Crear_Semaforo(KEY, N_CAJAS, &semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al crear los semaforos");
        exit(EXIT_FAILURE);
    }
    
    /*Los pone todos a 1*/
    
    array = (unsigned short *)malloc(N_CAJAS * sizeof(unsigned short));
    if (array == NULL) {
        perror("Error al inicializar los semaforos");
        exit(EXIT_FAILURE);
    }
    
    for (i = 0; i < N_CAJAS; i++) {
        array[i] = 1;
    }
    
    retorno_semaforos = Inicializar_Semaforo(semid, array);
    if (retorno_semaforos == ERROR) {
        perror("Error al inicializar los semaforos");
        exit(EXIT_FAILURE);
    }
    
    /*El padre genera N hijos (cajeros)*/
    
    for (i = 1; i <= N_CAJAS; i++) {
        child_pid = fork();
        
        if (child_pid == -1) {
            perror("Error al crear los hijos");
            
            /*esperar a los hijos*/
            while(wait(NULL) > 0);
            
            exit(EXIT_FAILURE);
        }
        
        if (child_pid == 0) {
            /*Inicializa el id de cada hijo*/
            argv_hijos_id = (char *)malloc(TAMANIO_ARGV_HIJOS * sizeof(char));
            if (argv_hijos_id == NULL) {
                perror ("Error al crear los argumentos para los hijos");
                
                /*esperar a los hijos*/
                while(wait(NULL) > 0);
                
                exit(EXIT_FAILURE);
            }
            
            sprintf(argv_hijos_id, "%d", i);
            
            /*Inicilaiza el numero de operaciones de cada hijo*/
            argv_hijos_num_operaciones = (char *)malloc(TAMANIO_ARGV_HIJOS * sizeof(char));
            if (argv_hijos_num_operaciones == NULL) {
                perror ("Error al crear los argumentos para los hijos");
                
                /*esperar a los hijos*/
                while(wait(NULL) > 0);
                
                exit(EXIT_FAILURE);
            }
            
            sprintf(argv_hijos_num_operaciones, "%d", N_OPERACIONES);
            
            /*Ejecutamos el codigo de otro fichero para mejor legibilidad del código*/
            execlp("./ejercicio9hijos", "ejercicio9hijos", argv_hijos_id, argv_hijos_num_operaciones, NULL);
            
            perror("Error en el execlp");
            
            /*esperar a los hijos*/
            while(wait(NULL) > 0);
            
            exit(EXIT_FAILURE);
        }
    }
    
    /*El padre espera las señales SIGRTMIN+1 y SIGRTMIN*/
    
    num_procesos_terminados = 0;
    
    while(1) {
        pause();
        
        if (num_procesos_terminados == N_CAJAS) {
            break;
        }
        
        sigemptyset(&set);
        sigaddset(&set, SIGRTMIN+1); /* Añade a la máscara el bloqueo por USR1*/
        sigaddset(&set, SIGRTMIN); /* Añade a la máscara el bloqueo por USR2*/
        sigprocmask(SIG_UNBLOCK, &set,&oset);
    }
    
    /*DEBUGGING*/
    printf("\nSoy el padre y he terminado %d procesos", num_procesos_terminados);
    /*DEBUGGING*/
    
    /*Han acabado todas las cajas. Se retira el dinero*/
    
    while(wait(NULL) > 0);
    
    pf_saldo_total = fopen(FICHERO_SALDO_TOTAL, "r");
    fread(&saldo_total, sizeof(int), 1, pf_saldo_total);
    fclose(pf_saldo_total);
    
    /*DEBUGGING*/
    printf("\nSaldo total antes de la ronda final: %d", saldo_total);
    /*DEBUGGING*/
    
    for (i = 1; i <= N_CAJAS; i++) {
        /*Crea la path*/
        
        sprintf(fichero_saldo, "files/saldoCaja%d.txt", i);
        if (fichero_saldo == NULL) {
            perror ("Error al crear la path al fichero de saldo de algun hijo");
        }
        
        /*Lee el saldo*/
        Down_Semaforo(semid, i, SEM_UNDO);
        
        pf_saldo = fopen(fichero_saldo, "r");
        if (pf_saldo == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_saldo);
            exit(EXIT_FAILURE);
        }
        fread(&saldo, sizeof(int), 1, pf_saldo);
        fclose(pf_saldo);
        
        /*DEBUGGING*/
        printf("\nSaldo a anadir en la ronda final: %d", saldo);
        /*DEBUGGING*/
        
        /*Actualiza el nuevo saldo total*/
        
        saldo_total += saldo;
        
        /*Guarda el saldo a 0*/
        
        saldo = 0;
        
        pf_saldo = fopen(fichero_saldo, "w");
        if (pf_saldo == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_saldo);
            exit(EXIT_FAILURE);
        }
        fwrite(&saldo, sizeof(int), 1, pf_saldo);
        fclose(pf_saldo);
        
        Up_Semaforo(semid, i, SEM_UNDO);
    }
    
    pf_saldo_total = fopen(FICHERO_SALDO_TOTAL, "w");
    fwrite(&saldo_total, sizeof(int), 1, pf_saldo_total);
    fclose(pf_saldo_total);
    
    /*DEBUGGING*/
    printf("\nSaldo total: %d", saldo_total);
    /*DEBUGGING*/
    
    retorno_semaforos = Borrar_Semaforo(semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al borrar los semaforos");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
    
}

/*Funciones auxiliares*/

void manejador_usr1 (int senal) {
    
    char fichero_saldo[TAM_PATH];
    int saldo, saldo_total;
    int i;
    FILE *pf_saldo, *pf_saldo_total;
    int semid;
    int retorno_semaforos;
    
    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN+1);
    sigaddset(&set, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &set,&oset);
    
    /*Obtiene los semaforos ya creados*/
    
    retorno_semaforos = Crear_Semaforo(KEY, N_CAJAS, &semid);
    if (retorno_semaforos == ERROR) {
        perror("Error al crear los semaforos en el hijo");
        exit(EXIT_FAILURE);
    } else if (retorno_semaforos != 1) {
        perror("Los semaforos creados no se han obtenido en manejador");
    }
    
    /*El padre recorre los ficheros de las cajas, buscando alguno
    con mas de 1000 de saldo. Le resta 900*/
    
    for (i = 1; i <= N_CAJAS; i++) {
        /*Crea la path*/
        
        sprintf(fichero_saldo, "files/saldoCaja%d.txt", i);
        if (fichero_saldo == NULL) {
            perror ("Error al crear la path al fichero de saldo de algun hijo");
        }
        
        /*Lee el saldo*/
        
        retorno_semaforos = Down_Semaforo(semid, i, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer down en los semaforos");
            exit(EXIT_FAILURE);
        }
        
        pf_saldo = fopen(fichero_saldo, "r");
        if (pf_saldo == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_saldo);
            exit(EXIT_FAILURE);
        }
        fread(&saldo, sizeof(int), 1, pf_saldo);
        fclose(pf_saldo);
        
        
        
        if (saldo >= 1000) {
        
            /*Guarda el nuevo saldo*/
            
            saldo -= 900;
            
            pf_saldo = fopen(fichero_saldo, "w");
            if (pf_saldo == NULL) {
                printf("No se pudo abrir el fichero %s", fichero_saldo);
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
            fwrite(&saldo, sizeof(int), 1, pf_saldo);
            fclose(pf_saldo);
            
            /*Actualiza el nuevo saldo total*/
            
            pf_saldo_total = fopen(FICHERO_SALDO_TOTAL, "r");
            fread(&saldo_total, sizeof(int), 1, pf_saldo_total);
            fclose(pf_saldo_total);
            
            saldo_total += 900;
            
            pf_saldo_total = fopen(FICHERO_SALDO_TOTAL, "w");
            fwrite(&saldo_total, sizeof(int), 1, pf_saldo_total);
            fclose(pf_saldo_total);
            
            /*Ha encontrado una caja a la que sacar dinero*/
            retorno_semaforos = Up_Semaforo(semid, i, SEM_UNDO);
            if (retorno_semaforos == ERROR) {
                perror("Error al hacer up en los semaforos");
                exit(EXIT_FAILURE);
            }
            return;
        }
        
        retorno_semaforos = Up_Semaforo(semid, i, SEM_UNDO);
        if (retorno_semaforos == ERROR) {
            perror("Error al hacer up en los semaforos");
            exit(EXIT_FAILURE);
        }
    }
}

void manejador_usr2 (int senal) {
    
    /*El padre incrementa la cuenta de cajas terminadas*/
    
    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN+1);
    sigaddset(&set, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &set,&oset);
    
    num_procesos_terminados++;
    
    wait(NULL);
}