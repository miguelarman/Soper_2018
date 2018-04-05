#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "semaforos.h"
#include "aleat_num.h"

#define SEGUNDOS(X) (X) * 1000000
#define MAX_TAM 256


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
    }
    
    sprintf(fichero_saldo, "files/saldoCaja%d.txt", id);
    if (fichero_operaciones == NULL) {
        perror ("Error al crear la path al fichero de saldo de algun hijo");
    }
    
    /*Pone su saldo inicial a cero*/
    saldo = 0;
    
    pf_saldo = fopen(fichero_saldo, "w");
    if (pf_saldo == NULL) {
        printf("No se pudo abrir el fichero %s", fichero_saldo);
        exit(EXIT_FAILURE);
    }
    fwrite(&saldo, sizeof(int), 1, pf_saldo);
    fclose(pf_saldo);
        
        
    
    for (i = 0; i < numero_operaciones; i++) {
        /*DEBUGGING*/
        /*printf("\n Soy la caja %d y voy a hacer la iteracion %d", id, i);
        fflush(stdout);*/
        /*DEBUGGING*/
        
        /*Lee una operación*/
        pf_operaciones = fopen(fichero_operaciones, "r");
        if (pf_operaciones == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_operaciones);
            exit(EXIT_FAILURE);
        }
        
        fseek(pf_operaciones, i * (sizeof(int) + sizeof(char)), SEEK_SET); /*Salta un entero y un salto de línea*/
        fread(&cantidad, sizeof(int), 1, pf_operaciones);
        
        fclose(pf_operaciones);
    
        /*Espera aleatoria*/
        
        usleep(SEGUNDOS(aleat_num(1, 5)));
        
        /*Lee el saldo*/
        
        pf_saldo = fopen(fichero_saldo, "r");
        if (pf_saldo == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_saldo);
            exit(EXIT_FAILURE);
        }
        fread(&saldo, sizeof(int), 1, pf_saldo);
        fclose(pf_saldo);
    
        /*Guarda el nuevo saldo*/
        
        saldo += cantidad;
        
        pf_saldo = fopen(fichero_saldo, "w");
        if (pf_saldo == NULL) {
            printf("No se pudo abrir el fichero %s", fichero_saldo);
            exit(EXIT_FAILURE);
        }
        fwrite(&saldo, sizeof(int), 1, pf_saldo);
        fclose(pf_saldo);
        
        /*Comprueba si tiene mas de 1000 euros*/
        
        if (saldo >= 1000) {
            /*Manda señal SIGUSR1 al padre para que retire el dinero*/
            
            retorno_senial = kill(padre_id, SIGUSR1);
            if (retorno_senial == -1) {
                perror("Error al mandar señal SIGUSR1 al padre");
                
                exit(EXIT_FAILURE);
            }
        }
    }
    
    
    /*Avisa al padre de que ha terminado con la señal SIGUSR2*/
    
    /*DEBUGGING*/
    printf("\nSoy la caja %d y he voy a avisar a mi padre de que he temrinado", id);
    fflush(stdout);
    /*DEBUGGING*/
    
    retorno_senial = kill(padre_id, SIGALRM);
    if (retorno_senial == -1) {
        perror("Error al mandar señal SIGUSR2 al padre");
        
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}