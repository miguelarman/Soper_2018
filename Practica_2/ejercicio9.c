#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#include "semaforos.h"
#include "aleat_num.h"

#define N_CAJAS 5
#define TAM_PATH 256
#define N_OPERACIONES 5
#define FICHERO_SALDO_TOTAL "saldoTotal.txt"
#define TAMANIO_ARGV_HIJOS 128

void manejador_usr1 (int senal);
void manejador_usr2 (int senal);

int num_procesos_terminados;

int main () {
    FILE *pf;
    char aux[TAM_PATH];
    int i, j;
    int v_aleat;
    pid_t child_pid;
    char *argv_hijos_id, *argv_hijos_num_operaciones;
    /*int num_procesos_terminados;*/

    /*El padre genera N ficheros*/
    for (i = 1; i <= N_CAJAS; i++){
        sprintf(aux, "files/clientesCaja%d.txt", i);
        if (aux == NULL){
            perror("Error al crear la path");
            exit(EXIT_FAILURE);
        }
        
        pf = fopen (aux, "wb");
        if (pf == NULL) {
            perror("Error al abrir algun fichero para inicializarlo");
            exit(EXIT_FAILURE);
        }
        
        for (j = 0; j < N_OPERACIONES; j++){
            v_aleat = aleat_num(0, 300);
            fprintf(pf, "%d\n", v_aleat);
        }
        
        fclose(pf);
    }
    

    
    /*Prepara los manejadores de las señales que espera*/
    
    if(signal (SIGUSR1, manejador_usr1) == SIG_ERR){
        perror("Error en el manejador sigusr1");
        exit(EXIT_FAILURE);
    }
    
    if(signal (SIGALRM, manejador_usr2) == SIG_ERR){
        perror("Error en el manejador sigusr2");
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
            
            execlp("./ejercicio9hijos", "ejercicio9hijos", argv_hijos_id, argv_hijos_num_operaciones, NULL);
            
            perror("Error en el execlp");
            
            /*esperar a los hijos*/
            while(wait(NULL) > 0);
            
            exit(EXIT_FAILURE);
        }
    }
    
    /*El padre espera las señales SIGUSR1 y SIGUSR2*/
    
    num_procesos_terminados = 0;
    
    while(1) {
        pause();
        
        /*DEBUGGING*/
        printf("\nProcesos terminados: %d", num_procesos_terminados);
        fflush(stdout);
        /*DEBUGGING*/
        
        if (num_procesos_terminados == N_CAJAS) {
            break;
        }
    }
    
    /*DEBUGGING*/
    printf("\nSoy el padre y he terminado %d procesos", num_procesos_terminados);
    /*DEBUGGING*/
    
    exit(EXIT_SUCCESS);
    
}

void manejador_usr1 (int senal) {
    
    /*El padre retira 900 euros del saldo de la caja
    que ha mandado la señal y los suma al total*/
    
}

void manejador_usr2 (int senal) {
    
    /*El padre retira todo del saldo de la caja que
    ha mandado la señal y los suma al total. Además,
    espera a ese hijo, que ya ha terminado y incrementa
    el numero de procesos terminados*/
    
    num_procesos_terminados++;
}