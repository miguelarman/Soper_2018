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



#define TAMANO_MENSAJE 2048
#define OK 0
#define ERROR 1
#define KEY 1300
#define FILEKEY "/bin/bash"

enum Tipo_Mensaje {PROCESO_AB = 1, PROCESO_BC = 2};

typedef struct _Mensaje{
    long id; /* Id del tipo de mensaje */
    char texto[TAMANO_MENSAJE];
} Mensaje;

/* Funciones privadas */
int numero_mensajes_pendientes(int msqid, int *cantidad);
void proceso_A(char *fichero_entrada, int msqid);
void proceso_B(int msqid);
void proceso_C(char *fichero_salida, int msqid);

int main (int argc, char **argv) {
    
    pid_t child_pid_1, child_pid_2, child_pid_3;
    int retorno_msgctl;
    int msqid;
    key_t key;
    
    if (argc < 3) {
        printf("Argumentos insuficientes");
        exit(EXIT_FAILURE);
    }
    
    /* Key para las colas de mensajes */
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error con la key \n");
        exit(EXIT_FAILURE);
    }
    
    
    /* Crea la cola de mensajes */
    msqid = msgget (key, IPC_CREAT | IPC_EXCL |0600);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes");
        exit(EXIT_FAILURE);
    }
    
    /* Crea el proceso A */
    if ((child_pid_1 = fork()) == -1) {
        perror("Error al crear el primer hijo");
        
        /* Libera la cola de mensajes */
        retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        if (retorno_msgctl != 0) {
            perror("Error al liberar la cola de mensajes");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_FAILURE);
    } else if (child_pid_1 == 0) {
        proceso_A(argv[1], msqid);
    }
    
    /* Crea el proceso B */
    if ((child_pid_2 = fork()) == -1) {
        perror("Error al crear el segundo hijo");
        
        kill(child_pid_1, SIGTERM);
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
    
    /* Crea el proceso C */
    if ((child_pid_3 = fork()) == -1) {
        perror("Error al crear el tercer hijo");
        
        kill(child_pid_1, SIGTERM);
        kill(child_pid_2, SIGTERM);
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
    
    
    /* Borra la cola de mensajes */
    retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
    if (retorno_msgctl != 0) {
        perror("Error al liberar la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    /* Espera a los procesos hijos */
    while(wait(NULL) > 0);
    
    exit(EXIT_SUCCESS);
}

/******************************************************************************/
/**********************     FUNCIONES PRIVADAS     ****************************/
/******************************************************************************/


void proceso_A(char *fichero_entrada, int msqid) {
    Mensaje *mensaje;
    FILE *pf;
    int retorno_envio, retorno_msgctl;
    
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
    mensaje->id = PROCESO_AB;
    
    
    /* Lee todo el fichero */
    while (!feof(pf)) {
        
        /* Lee 2KB de texto */
        fread(&(mensaje->texto), TAMANO_MENSAJE - 1, 1, pf);
        mensaje->texto[TAMANO_MENSAJE - 1] = '\0';
        
        /* Manda un mensaje del tipo PROCESO_AB a la cola */
        retorno_envio = msgsnd (msqid, (struct msgbuf *) mensaje, sizeof(Mensaje) - sizeof(long), 0);
        if (retorno_envio == -1) {
            perror("Error al mandar el mensaje en el proceso A");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }
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
    
    
    exit(EXIT_SUCCESS);
}

void proceso_B(int msqid) {
    Mensaje *mensaje;
    int i, cantidad;
    int retorno_envio, retorno_recepcion, retorno, retorno_msgctl;
    
    /* Reserva memoria para la estructura del mensaje */
    mensaje = (Mensaje *)malloc(sizeof(Mensaje));
    if (mensaje == NULL) {
        perror("Error al reservar memoria para el mensaje en el proceso A");
        exit(EXIT_FAILURE);
    }

    cantidad = 1;
    
    do {
        
        /* Lee un mensaje del tipo PROCESO_AB */
        retorno_recepcion = msgrcv(msqid,  (struct msgbuf *)mensaje, sizeof(Mensaje) - sizeof(long), PROCESO_AB, IPC_NOWAIT);
        if (retorno_recepcion == -1) {
            perror("Error al recibir el mensaje en el proceso B");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }
        }
        
        /* Modifica el texto del mensaje */
        
        for (i = 0; i < strlen(mensaje->texto) - 1; i++) {
            
            if (mensaje->texto[i] > 'z' || mensaje->texto[i] < 'a') {
                /* Ha terminado de leerlo */
                break;
            } else if (mensaje->texto[i] == 'z') {
                mensaje->texto[i] = 'a';
            }  else {
                mensaje->texto[i]++;
            }
        }
        
        /* Manda un mensaje del tipo PROCESO_BC */
        mensaje->id = PROCESO_BC;
        retorno_envio = msgsnd (msqid, (struct msgbuf *) mensaje, sizeof(Mensaje) - sizeof(long), 0);
        if (retorno_envio == -1) {
            perror("Error al mandar el mensaje en el proceso B");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }
        }
        
        
        /* Vuelve a leer el numero de mensajes en la cola */
        retorno = numero_mensajes_pendientes(msqid, &cantidad);
        if (retorno == ERROR) {
            perror("Error al leer el numero de mensajes pendientes en el proceso B");
        }
    } while (cantidad > 0);

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
    int retorno_recepcion, retorno, retorno_msgctl;
    int cantidad;
    
    /* Consigue la cola de mensajes */

    
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
        retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        if (retorno_msgctl != 0) {
            perror("Error al liberar la cola de mensajes");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_FAILURE);
    }
    
    
    /* Lee los mensajes del tipo PROCESO_BC*/
    do {
        
        /* Lee un mensaje del tipo PROCESO_BC */
        retorno_recepcion = msgrcv(msqid,  (struct msgbuf *)mensaje, sizeof(Mensaje) - sizeof(long), PROCESO_BC, IPC_NOWAIT);
        if (retorno_recepcion == -1) {
            perror("Error al recibir el mensaje en el proceso C");
            
            /* Libera la estructura del mensaje*/
            free(mensaje);
            
            /* Libera la cola de mensajes */
            retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            if (retorno_msgctl != 0) {
                perror("Error al liberar la cola de mensajes");
                exit(EXIT_FAILURE);
            }
        }
        
        
        /* Vuelca en el fichero de salida los datos*/
        fwrite(&(mensaje->texto), TAMANO_MENSAJE - 1, 1, pf);
        
        
        
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