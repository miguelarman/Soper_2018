#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#include "semaforos.h"
#include "aleat_num.h"

#define KEY 1300
#define FILEKEY "/bin/bash"
#define N 10 /* Numero maximo de elementos que pueden estar listos para consumir a la vez*/
#define NUMERO_SEMAFOROS 4
#define MIN_SLEEP_PRODUCTOR 1
#define MAX_SLEEP_PRODUCTOR 5
#define MIN_SLEEP_CONSUMIDOR 1
#define MAX_SLEEP_CONSUMIDOR 5
#define ESPERA_INICIAL_CONSUMIDOR 10
#define ESPERA_INICIAL_PRODUCTOR 0

/* Estructura donde guardamos los valores para compartir entre procesos */
typedef struct {
    int contador; /* contador del numero de elementos disponibles*/
    char lista[N]; /* array circular*/
    int entrada; /* numero de la lista de la siguiente posicion libre */
    int salida; /* numero de la lista del primer elemento ocupado */
} Datos_Compartidos;

/* Enumeracion con los distintos semaforos que utilizamos */
enum semaforos {SEMAFORO_LISTA, SEMAFORO_ENTRADA, SEMAFORO_SALIDA, SEMAFORO_CONTADOR};


/* Funciones privadas */
void productor(int key);
void consumidor(int key);
int reservashm(int size, int key);



int main() {
    
    pid_t child_pid_1, child_pid_2;
    Datos_Compartidos *datos;
    key_t key;
    int retorno_semaforo, retorno_shmdt, retorno_shmctl;
    int shmid, semid;
    unsigned short unos[NUMERO_SEMAFOROS];
    int i;
    
    /* Key para la memoria compartida */
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error con la key \n");
        exit(EXIT_FAILURE);
    }
    
    /* Crea la memoria compartida */
    shmid = reservashm(sizeof(Datos_Compartidos*), key);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue la memoria compartida*/
    datos = shmat (shmid, (char *)0, 0);
    if (datos == (void *) -1) {
        perror("Error al conseguir la memoria compartida en el padre");
        exit(EXIT_FAILURE);
    }
    
    /* Crea los semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUMERO_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al crear los semaforos en el padre");
        exit(EXIT_FAILURE);
    } else if (retorno_semaforo == 1) {
        perror("No se han creado los semaforos en el padre");
        exit(EXIT_FAILURE);
    }
    /* Inicializa los semaforos a 1 */
    for (i = 0; i < NUMERO_SEMAFOROS; i++) {
        unos[i] = 1;
    }
    retorno_semaforo = Inicializar_Semaforo(semid, unos);
    if (retorno_semaforo == ERROR) {
        perror("Error al inicializar los semaforos en el padre");
        exit(EXIT_FAILURE);
    }
    
    /* Inicializamos las variables necesarias para el productor-consumidor */
    datos->contador = 0;
    datos->entrada = 0;
    datos->salida = -1;
    
    /* Creamos el hijo que se va a encargar del productor */
    if ((child_pid_1 = fork()) == -1) {
        perror("Error al crear el primer hijo");
        exit(EXIT_FAILURE);
    } else if (child_pid_1 == 0) {
        productor(key);
    }
    
    /* Creamos el hijo que se va a encargar del consumidor */
    if ((child_pid_2 = fork()) == -1) {
        perror("Error al crear el segundo hijo");
        exit(EXIT_FAILURE);
    } else if (child_pid_2 == 0) {
        consumidor(key);
    }
    
    /* Resto del codigo */
    
    
    /* Espera a sus hijos */
    while(wait(NULL) > 0);
    
    /*Eliminamos la memoria compartida*/
    retorno_shmdt = shmdt ((char *)datos);
    if (retorno_shmdt == -1) {
        perror("Error al hacer el dettach en el padre");
        exit(EXIT_FAILURE);
    }
    retorno_shmctl = shmctl (shmid, IPC_RMID, (struct shmid_ds *)NULL);
    if (retorno_shmctl == -1) {
        perror("Error al borrar la memoria en el padre");
        exit(EXIT_FAILURE);
    }
    
    /* Libera los semaforos */
    retorno_semaforo = Borrar_Semaforo(semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al borrar los semaforos en el padre");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

/******************************************************************************/
/**********************     FUNCIONES PRIVADAS     ****************************/
/******************************************************************************/

int reservashm(int size, int key) {
    int shmid_reserva;
    shmid_reserva = shmget (key, size, IPC_CREAT | IPC_EXCL |SHM_R | SHM_W);
    
    if(shmid_reserva == -1) {
        shmid_reserva = shmget(key, size, 0);
    }
    
    return shmid_reserva;
}

void productor(int key) {
    
    char letra;
    Datos_Compartidos *datos;
    int retorno_shmdt, retorno_semaforo;
    int shmid_productor, semid;
    
    letra = 'A';
    
    /*Consigue la memoria compartida*/
    shmid_productor = reservashm(sizeof(Datos_Compartidos*), key);
    if (shmid_productor == -1) {
        perror("Error al conseguir la memoria compartida en el productor");
        exit(EXIT_FAILURE);
    }
    datos = shmat (shmid_productor, (char *)0, 0);
    if (datos == (void *) -1) {
        perror("Error al conseguir la memoria compartida en el productor");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue los semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUMERO_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al conseguir los semaforos en el productor");
        exit(EXIT_FAILURE);
    } else if (retorno_semaforo == 0) {
        perror("Los semaforos del productor no existian");
        exit(EXIT_FAILURE);
    }
    
    sleep(ESPERA_INICIAL_PRODUCTOR);
    
    while (1) {
        sleep(aleat_num(MIN_SLEEP_PRODUCTOR, MAX_SLEEP_PRODUCTOR));
        
        while (datos->contador == N); /* Aqui no se cómo poner semaforos */
        
        /* Hace down del semaforo que controla el contador */
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de contador en el productor");
            exit(EXIT_FAILURE);
        }
        
        /* Modifica el contador*/
        datos->contador = datos->contador + 1;
        
        /* Hace up del semaforo que controla el contador */
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de contador en el productor");
            exit(EXIT_FAILURE);
        }
        
        /* Hace down de los semaforos de la entrada y la lista */
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_ENTRADA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de entrada en el productor");
            exit(EXIT_FAILURE);
        }
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_LISTA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de lista en el productor");
            exit(EXIT_FAILURE);
        }
        
        /* Asigna la entrada a la nueva letra */
        datos->lista[datos->entrada] = letra;
        
        /* Hace up del semaforo que controla la lista */
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_LISTA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de lista en el productor");
            exit(EXIT_FAILURE);
        }
        
        /* Hace down del semaforo que controla el contador */
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de contador en el productor");
            exit(EXIT_FAILURE);
        }
        
        if (datos->contador == 1) {
            /* Hace down del semaforo que controla la salida */
            retorno_semaforo = Down_Semaforo(semid, SEMAFORO_SALIDA, SEM_UNDO);
            if (retorno_semaforo == ERROR) {
                perror("Error al hacer down en el semaforo de salida en el productor");
                exit(EXIT_FAILURE);
            }
            
            datos->salida = datos->entrada;
            
            /* Hace down del semaforo que controla la salida */
            retorno_semaforo = Up_Semaforo(semid, SEMAFORO_SALIDA, SEM_UNDO);
            if (retorno_semaforo == ERROR) {
                perror("Error al hacer up en el semaforo de salida en el productor");
                exit(EXIT_FAILURE);
            }
        }
        
        if (datos->contador == N) {
            datos->entrada = -1;
        } else {
            datos->entrada = ((datos->entrada) + 1) % N;
        }
        
        /* Hace up de los semaforos que controlan la entrada y el contador */
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_ENTRADA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de entrada en el productor");
            exit(EXIT_FAILURE);
        }
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de contador en el productor");
            exit(EXIT_FAILURE);
        }
        
        /*DEBUGGING*//*printf("Acabo de producir la %c\n", letra);fflush(stdout);*/
        
        if (letra == 'Z') {         /* Actualiza la letra a producir */
            letra = '0';
        } else if (letra == '9') {
            break;
        } else {
            letra++;
        }
    }
    
    /* Hace dettach de la memoria compartida */
    retorno_shmdt = shmdt ((char *)datos);
    if (retorno_shmdt == -1) {
        perror("Error al hacer el dettach en el productor");
        exit(EXIT_FAILURE);
    }
    
    /* Libera los semaforos */
    /*retorno_semaforo = Borrar_Semaforo(semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al borrar los semaforos en el productor");
        exit(EXIT_FAILURE);
    }*/
    
    exit(EXIT_SUCCESS);
}

void consumidor(int key) {
    
    char caracter_consumidor;
    Datos_Compartidos *datos;
    int retorno_shmdt, retorno_semaforo;
    int shmid_consumidor, semid;
    
    /*Consigue la memoria compartida*/
    shmid_consumidor = reservashm(sizeof(Datos_Compartidos*), key);
    if (shmid_consumidor == -1) {
        perror("Error al conseguir la memoria compartida en el consumidor");
        exit(EXIT_FAILURE);
    }
    datos = shmat (shmid_consumidor, (char *)0, 0);
    if (datos == (void *) -1) {
        perror("Error al conseguir la memoria compartida en el consumidor");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue los semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUMERO_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al conseguir los semaforos en el consumidor");
        exit(EXIT_FAILURE);
    } else if (retorno_semaforo == 0) {
        perror("Los semaforos del consumidor no existian");
        exit(EXIT_FAILURE);
    }
    
    sleep(ESPERA_INICIAL_CONSUMIDOR);
    
    while (1) {
        sleep(aleat_num(MIN_SLEEP_PRODUCTOR, MAX_SLEEP_PRODUCTOR));
        
        while (datos->contador == 0);
        
        /* Hace down del semaforo que controla el contador */
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de contador en el consumidor");
            exit(EXIT_FAILURE);
        }
        
        datos->contador = datos->contador - 1;
        
        /* Hace up del semaforo que controla el contador */
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de contador en el consumidor");
            exit(EXIT_FAILURE);
        }
        
        
        /* Hace down del semaforo que controla la salida */
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_SALIDA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de salida en el consumidor");
            exit(EXIT_FAILURE);
        }
        /* Hace down del semaforo que controla la lista */
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_LISTA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de lista en el consumidor");
            exit(EXIT_FAILURE);
        }
        
        /* Lee la letra */
        caracter_consumidor = datos->lista[datos->salida];
        
        /* Hace up del semaforo que controla la lista */
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_LISTA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de lista en el consumidor");
            exit(EXIT_FAILURE);
        }
        
        /* Hace down del semaforo que controla el contador */
        retorno_semaforo = Down_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer down en el semaforo de contador en el consumidor");
            exit(EXIT_FAILURE);
        }
        
        if (datos->contador == N - 1) {
            /* Hace down del semaforo que controla la entrada */
            retorno_semaforo = Down_Semaforo(semid, SEMAFORO_ENTRADA, SEM_UNDO);
            if (retorno_semaforo == ERROR) {
                perror("Error al hacer down en el semaforo de entrada en el consumidor");
                exit(EXIT_FAILURE);
            }
            datos->entrada = datos->salida;
            /* Hace up del semaforo que controla la entrada */
            retorno_semaforo = Up_Semaforo(semid, SEMAFORO_ENTRADA, SEM_UNDO);
            if (retorno_semaforo == ERROR) {
                perror("Error al hacer up en el semaforo de entrada en el consumidor");
                exit(EXIT_FAILURE);
            }
        }
        
        if (datos->contador == 0) {
            datos->salida = -1;
        } else {
            datos->salida = ((datos->salida) + 1) % N;
        }
        
        /* Hace down de los semaforos que controlan el contador y la salida */
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_CONTADOR, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de contador en el consumidor");
            exit(EXIT_FAILURE);
        }
        retorno_semaforo = Up_Semaforo(semid, SEMAFORO_SALIDA, SEM_UNDO);
        if (retorno_semaforo == ERROR) {
            perror("Error al hacer up en el semaforo de salida en el consumidor");
            exit(EXIT_FAILURE);
        }
        
        printf("%c\n", caracter_consumidor);
        /*DEBUGGING*//*printf("%c (in %d out %d)\n", caracter_consumidor, datos->entrada, datos->salida);*/
        
        if (caracter_consumidor == '9') {
            break;
        }
    }
    
    /* Hace dettach de la memoria compartida */
    retorno_shmdt = shmdt ((char *)datos);
    if (retorno_shmdt == -1) {
        perror("Error al hacer el dettach en el consumidor");
        exit(EXIT_FAILURE);
    }
    
    /* Libera los semaforos */
    /*retorno_semaforo = Borrar_Semaforo(semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al borrar los semaforos en el consumidor");
        exit(EXIT_FAILURE);
    }*/
    
    exit(EXIT_SUCCESS);
}