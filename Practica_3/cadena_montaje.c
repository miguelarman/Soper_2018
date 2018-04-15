#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "aleat_num.h"



#define TAMANO_MENSAJE 2048
#define OK 0
#define ERROR 1
#define KEY 1300
#define FILEKEY "/bin/bash"

enum Tipo_Mensaje {PROCESO_AB = 1, PROCESO_BC = 2};

typedef struct msgbuf {
    long mtype; /* Id del tipo de mensaje */
    char mtext[TAMANO_MENSAJE];
} Mensaje;

/* Funciones privadas */
void inicializa_fichero_entrada(char *fichero_entrada);
int numero_mensajes_pendientes(int msqid, int *cantidad);
void proceso_A(char *fichero_entrada, int msqid, int child_pid_2);
void proceso_B(int msqid);
void proceso_C(char *fichero_salida, int msqid);
void manejador(int senal);

int proceso_1_terminado;

int main (int argc, char **argv) {
    
    pid_t child_pid_1, child_pid_2, child_pid_3;
    int retorno_msgctl;
    int msqid;
    key_t key;
    
    if (argc < 3) {
        printf("Argumentos insuficientes");
        exit(EXIT_FAILURE);
    }
    
    proceso_1_terminado = 0;
    
    /* Key para las colas de mensajes */
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error con la key \n");
        exit(EXIT_FAILURE);
    }
    
    
    /* Crea la cola de mensajes */
    msqid = msgget (key, IPC_CREAT | IPC_EXCL | 0660);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes");
        exit(EXIT_FAILURE);
    }
    
    /* DEBUGGING *//*printf("msqid: %d\n", msqid); fflush(stdout);*/
    
    /* Inicializa el fichero de entrada */
    inicializa_fichero_entrada(argv[1]);
    
    
    /* Crea el proceso C */
    if ((child_pid_3 = fork()) == -1) {
        perror("Error al crear el tercer hijo");
        
        while(wait(NULL) > 0);
        
        /* Libera la cola de mensajes */
        retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        if (retorno_msgctl != 0) {
            perror("Error al liberar la cola de mensajes");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_FAILURE);
    } else if (child_pid_3 == 0) {
        proceso_C(argv[2], msqid);
    }
    
    /* Crea el proceso B */
    if ((child_pid_2 = fork()) == -1) {
        perror("Error al crear el segundo hijo");
        
        kill(child_pid_3, SIGTERM);
        while(wait(NULL) > 0);
        
        /* Libera la cola de mensajes */
        retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        if (retorno_msgctl != 0) {
            perror("Error al liberar la cola de mensajes");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_FAILURE);
    } else if (child_pid_2 == 0) {
        proceso_B(msqid);
    }
    
    /* Crea el proceso A */
    if ((child_pid_1 = fork()) == -1) {
        perror("Error al crear el primer hijo");
        
        kill(child_pid_3, SIGTERM);
        kill(child_pid_2, SIGTERM);
        while(wait(NULL) > 0);
        
        /* Libera la cola de mensajes */
        retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        if (retorno_msgctl != 0) {
            perror("Error al liberar la cola de mensajes");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_FAILURE);
    } else if (child_pid_1 == 0) {
        proceso_A(argv[1], msqid, child_pid_2);
    }
    
    
    
    
    /* Espera a los procesos hijos */
    while(wait(NULL) > 0);
    
    
    /* Borra la cola de mensajes */
    retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
    if (retorno_msgctl != 0) {
        perror("Error al liberar la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

/******************************************************************************/
/**********************     FUNCIONES PRIVADAS     ****************************/
/******************************************************************************/

void inicializa_fichero_entrada(char *fichero_entrada) {
    FILE *pf;
    char letra;
    int i;
    int numero_caracteres;
    
    /* Numero de caracteres a imprimir */
    numero_caracteres = aleat_num(4 * 2048, 5 * 2048);
    
    pf = fopen(fichero_entrada, "w");
    if (pf == NULL) {
        perror("Error al abrir el fichero de entrada para rellenarlo");
        exit(EXIT_FAILURE);
    }
    
    
    for (i = 0; i < numero_caracteres; i++) {
        letra = aleat_num(97, 122);
        
        fwrite(&letra, sizeof(char), 1, pf);
    }
    
    fclose(pf);
}

void proceso_A(char *fichero_entrada, int msqid, int child_pid_2) {
    Mensaje *mensaje;
    FILE *pf;
    int retorno_envio;/*, retorno_msgctl;*/
    
    /* Reserva memoria para la estructura del mensaje */
    mensaje = (Mensaje *)malloc(sizeof(Mensaje));
    if (mensaje == NULL) {
        perror("Error al reservar memoria para el mensaje en el proceso A");
        exit(EXIT_FAILURE);
    }
    
    
    /* Abre el fichero de entrada */
    pf = fopen(fichero_entrada, "r");
    if (pf == NULL) {
        perror("Error al abrir el fichero de entrada");
        exit(EXIT_FAILURE);
    }
    
    /* Inicializa el tipo de mensaje a PROCESO_AB */
    mensaje->mtype = PROCESO_AB;
    
    
    /* Lee todo el fichero */
    while (!feof(pf)) {
        
        /* Lee 2KB de texto */
        fread(&(mensaje->mtext), TAMANO_MENSAJE, 1, pf);
        /*DEBUGGING*//*printf("\n\ntexto1: %s", mensaje->mtext);fflush(stdout);*/
        
        /* Manda un mensaje del tipo PROCESO_AB a la cola */
        retorno_envio = msgsnd (msqid, (struct Mensaje *) mensaje, sizeof(Mensaje) - sizeof(long), 0);
        if (retorno_envio == -1) {
            perror("Error al mandar el mensaje en el proceso A");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }*/
            
            exit(EXIT_FAILURE);
        }
    }
    
    
    /* Cierra el fichero de entrada */
    fclose(pf);
    
    /* Libera la memoria de la estructura del mensaje */
    free(mensaje);
    
    /* Libera la cola de mensajes */
    /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
    if (retorno_msgctl != 0) {
        perror("Error al liberar la cola de mensajes");
        exit(EXIT_FAILURE);
    }*/
    
    kill(child_pid_2, SIGUSR1);
    
    exit(EXIT_SUCCESS);
}

void proceso_B(int msqid) {
    Mensaje *mensaje;
    int i, cantidad;
    int retorno_envio, retorno_recepcion, retorno;/*, retorno_msgctl;*/
    
    /* Reserva memoria para la estructura del mensaje */
    mensaje = (Mensaje *)malloc(sizeof(Mensaje));
    if (mensaje == NULL) {
        perror("Error al reservar memoria para el mensaje en el proceso A");
        exit(EXIT_FAILURE);
    }

    cantidad = 1;
    proceso_1_terminado = 0;
    
    /* Manejador de la senal SIGUSR1 */
    if (signal(SIGUSR1, manejador) == SIG_ERR) {
        perror("Error en el manejador");
        exit (EXIT_FAILURE);
    }
    
    do {
        
        /* Lee un mensaje del tipo PROCESO_AB */
        retorno_recepcion = msgrcv(msqid,  (struct Mensaje *)mensaje, sizeof(Mensaje) - sizeof(long), PROCESO_AB, IPC_NOWAIT);
        if (proceso_1_terminado == 1 && errno == ENOMSG) {
            errno = 0;
            break;
        }
        if (errno == ENOMSG) {
            errno = 0;
            continue;
        }
        if (retorno_recepcion == -1) {
            perror("Error al recibir el mensaje en el proceso B");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }*/
            
            exit(EXIT_FAILURE);
        }
        
        /* Modifica el texto del mensaje */
        
        /*DEBUGGING*//*printf("\n\ntexto antes: %s", mensaje->mtext);fflush(stdout);*/
        
        for (i = 0; i < strlen(mensaje->mtext); i++) {
            
            if (mensaje->mtext[i] > 'z' || mensaje->mtext[i] < 'a') {
                /* Ha terminado de leerlo */
                break;
            } else if (mensaje->mtext[i] == 'z') {
                mensaje->mtext[i] = 'a';
            }  else {
                mensaje->mtext[i]++;
            }
        }
        /*DEBUGGING*//*printf("\n\ntexto despues: %s", mensaje->mtext);fflush(stdout);*/
        
        /* Manda un mensaje del tipo PROCESO_BC */
        mensaje->mtype = PROCESO_BC;
        retorno_envio = msgsnd (msqid, (struct Mensaje *) mensaje, sizeof(Mensaje) - sizeof(long), 0);
        if (retorno_envio == -1) {
            perror("Error al mandar el mensaje en el proceso B");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }*/
            
            exit(EXIT_FAILURE);
        }
        
        
        /* Vuelve a leer el numero de mensajes en la cola */
        retorno = numero_mensajes_pendientes(msqid, &cantidad);
        if (retorno == ERROR) {
            perror("Error al leer el numero de mensajes pendientes en el proceso B");
        }
    } while (1);

    /* Libera la memoria de la estructura del mensaje */
    free(mensaje);
    
    /* Libera la cola de mensajes */
    /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
    if (retorno_msgctl != 0) {
        perror("Error al liberar la cola de mensajes");
        exit(EXIT_FAILURE);
    }*/
    
    exit(EXIT_SUCCESS);

}

void proceso_C(char *fichero_salida, int msqid) {
    
    FILE *pf;
    Mensaje *mensaje;
    int retorno_recepcion, retorno;/*, retorno_msgctl;*/
    int cantidad;
    
    
    /* Reserva memoria para la estructura del mensaje */
    mensaje = (Mensaje *)malloc(sizeof(Mensaje));
    if (mensaje == NULL) {
        perror("Error al reservar memoria para el mensaje en el proceso A");
        exit(EXIT_FAILURE);
    }
    
    /* Abre el fichero de salida */
    pf = fopen(fichero_salida, "a");
    if (pf == NULL) {
        perror("Error al abrir el fichero de salida");
        
        /* Libera la estructura del mensaje*/
        free(mensaje);
        
        /* Libera la cola de mensajes */
        /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        if (retorno_msgctl != 0) {
            perror("Error al liberar la cola de mensajes");
            exit(EXIT_FAILURE);
        }*/
        
        exit(EXIT_FAILURE);
    }
    
    
    /* Lee los mensajes del tipo PROCESO_BC*/
    do {
        
        /* Lee un mensaje del tipo PROCESO_BC */
        retorno_recepcion = msgrcv(msqid,  (struct Mensaje *)mensaje, sizeof(Mensaje) - sizeof(long), PROCESO_BC, 0);
        if (retorno_recepcion == -1) {
            perror("Error al recibir el mensaje en el proceso C");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }*/
            
            exit(EXIT_FAILURE);
        }
        
        
        /* Vuelca en el fichero de salida los datos*/
        fwrite(&(mensaje->mtext), TAMANO_MENSAJE, 1, pf);
        
        
        
        /* Lee el numero de mensajes en la cola */
        retorno = numero_mensajes_pendientes(msqid, &cantidad);
        if (retorno == ERROR) {
            perror("Error al leer el numero de mensajes pendientes en el proceso C");
        }
    } while (cantidad > 0);
    
    /* Cierra el fichero de salida */
    fclose(pf);

    /* Libera la memoria de la estructura del mensaje */
    free(mensaje);
    
    /* Libera la cola de mensajes */
    /*retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
    if (retorno_msgctl != 0) {
        perror("Error al liberar la cola de mensajes");
        exit(EXIT_FAILURE);
    }*/
    
    exit(EXIT_SUCCESS);
}

int numero_mensajes_pendientes(int msqid, int *cantidad) {
    struct msqid_ds buf;
    int retorno;
    
    retorno = msgctl(msqid, IPC_STAT, &buf);
    
    if (retorno == 0) {
        *cantidad = buf.msg_qnum;
        
        return OK;
    } else {
        return ERROR;
    }
}

void manejador(int senal) {
    proceso_1_terminado = 1;
}