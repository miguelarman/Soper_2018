#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>


#include "estructuras.h"
#include "aleat_num.h"
#include "semaforos.h"

#define KEY 1300 /*!< KEY necesaria para el ftok */
#define FILEKEY "/bin/bash" /*!< FILEKEY necesario para ftok */
#define TAMANIO_ARGUMENTOS_EXEC 256 /*!< Tamaño máximo de los parámetros enviados a los distintos procesos*/

int main(int argc, char ** argv) {
    
    /* Lee los argumentos de entrada */
    
    if (argc != 11) {
        fprintf(stderr, "Error en los parametros de entrada:\n\n");
        fprintf(stderr, "%s -n_caballos <int> -longitud_carrera <int> -n_apostadores <int>\n", argv[0]);
        fprintf(stderr, "\t\t -n_ventanillas <int> -dinero_apostadores <double> \n");
        fprintf(stderr, "Donde:\n");
        fprintf(stderr, "-n_caballos: numero de caballos en la carrera\n");
        fprintf(stderr, "-longitud_carrera: longitud de la carrera\n");
        fprintf(stderr, "-n_apostadores: numero total de apostadores\n");
        fprintf(stderr, "-n_ventanillas: numero de ventanillas para gestionar las apuestas\n");
        fprintf(stderr, "-dinero_apostadores: cantidad de dinero con el que empiezan los apostadores\n");
        exit(EXIT_FAILURE);
    }
    
    /* comprueba la linea de comandos */
    for(i = 1; i < argc ; i++) {
        if (strcmp(argv[i], "-n_caballos") == 0) {
            n_caballos = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-longitud_carrera") == 0) {
            longitud_carrera = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-n_apostadores") == 0) {
            n_apostadores = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-n_ventanillas") == 0) {
            n_ventanillas = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-dinero_apostadores") == 0) {
            dinero_apostadores = (double)atoi(argv[++i]);
        } else {
            fprintf(stderr, "Parametro %s es incorrecto\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    
    /* Calcula la key para la memoria compartida y los semaforos */
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        perror("Error al usar ftok");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }

    /* Crea la memoria compartida */
    shmid = reservashm(sizeof(Memoria_Compartida*), key);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    /* Consigue la memoria compartida*/
    memoria_compartida = shmat (shmid, (char *)0, 0);
    if (datos == (void *) -1) {
        perror("Error al conseguir la memoria compartida en el proceso principal");
        exit(EXIT_FAILURE);
    }
    
    /* Guarda el numero total de apostadores y de caballos */
    memoria_compartida->n_caballos = n_caballos;
    memoria_compartida->n_apostadores = n_apostadores;


    /* Crea los parametros que van a ser usados en los exec */
    
    numero_apostadores_arg = (char *)malloc(TAMANIO_ARGUMENTOS_EXEC * sizeof(char));
    if (numero_apostadores_arg == NULL) {
        perror("Error al reservar el argumento numero_apostadores_arg");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    sprintf(numero_apostadores_arg, "%d", n_apostadores);
    
    numero_caballos_arg = (char *)malloc(TAMANIO_ARGUMENTOS_EXEC * sizeof(char));
    if (numero_caballos_arg == NULL) {
        perror("Error al reservar el argumento numero_caballos_arg");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    sprintf(numero_caballos_arg, "%d", n_caballos);
    
    key_arg = (char *)malloc(TAMANIO_ARGUMENTOS_EXEC * sizeof(char));
    if (key_arg == NULL) {
        perror("Error al reservar el argumento key_arg");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    sprintf(key_arg, "%d", key);
    
    
    
    /* El proceso principal crea: */
    /* 1- El proceso monitor */

    pid_monitor = fork();
    if (pid_monitor == -1) {
        /** liberamos memoria y mas cosas ***************************************/
        perror("Error al crear el proceso monitor");
        exit(EXIT_FAILURE);
    } else if (pid_monitor == 0) {
        execlp("./proceso_monitor", "proceso_monitor" /*, resto de parametros necesarios*/, NULL);
        perror("Error en el execlp del proceso monitor");
        /** liberamos memoria y mas cosas ***************************************/
    }
    
    /* 2- El gestor de apuestas */
    
    pid_gestor_apuestas = fork();
    if (pid_gestor_apuestas == -1) {
        /** liberamos memoria y mas cosas ***************************************/
        perror("Error al crear el proceso gestor de apuestas");
        exit(EXIT_FAILURE);
    } else if (pid_gestor_apuestas == 0) {
        execlp("./proceso_gestor_apuestas", "proceso_gestor_apuestas"/*, resto de parametros necesarios*/, NULL);
        perror("Error en el execlp del proceso gestor de apuestas");
        /** liberamos memoria y mas cosas ***************************************/
    }
    
    /* 3- El proceso apostador */
    
    pid_apostador = fork();
    if (pid_apostador == -1) {
        /** liberamos memoria y mas cosas ***************************************/
        perror("Error al crear el proceso apostador");
        exit(EXIT_FAILURE);
    } else if (pid_apostador == 0) {
        execlp("./proceso_apostador", "proceso_apostador", numero_apostadores_arg, apuesta_maxima_arg, numero_caballos_arg, NULL);
        perror("Error en el execlp del proceso apostador");
        /** liberamos memoria y mas cosas ***************************************/
    }
    
    /* 4- Los caballos */
    
    /* TODO Crea las pipes necesarias para la comunicación */
    for (int i = 0; i < n_caballos; i++) {
        pipe_status = pipe(pipes_caballos[i]);
        if (pipe_status == -1) {
            perror("error creando las pipes\n");
            /** liberamos memoria y mas cosas ***************************************/
            exit(EXIT_FAILURE);
        }
    
    }
    /* Crea un array para guardar todos los pids */
    pids_caballos = (pid_t *)malloc(n_caballos * sizeof(pid_t));
    if (pids_caballos == NULL) {
        perror("Error al crear el array de los pids de los caballos");
        /** liberamos memoria y mas cosas ***************************************/
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < n_caballos; i++) {
        pids_caballos[i] = fork();
        if (pids_caballos[i] == -1) {
            /** liberamos memoria y mas cosas ***************************************/
            perror("Error al crear caballos");
            exit(EXIT_FAILURE);
        } else if (pids_caballos[i] == 0) {
            
            /*******************************************/
            /******** EJECUCION DE LOS CABALLOS ********/
            /*******************************************/
            
            /* TODO Prepara las senales */
            /* TODO Reserva una estructura de mensaje */
            
            
            /* Cierra la escritura en la pipe */
            close((pipes_caballos[i])[ESCRITURA]);
            
            while(1) {
                /* Espera la señal del padre */
                pause();
                
                /* Lee de la pipe su posición en la carrera */
                read((pipes_caballos[i])[LECTURA], &posicion, sizeof(int));
            
                if (posicion == PRIMERO) {
                    mensaje_caballo->tirada = aleat_num(1, 7);
                } else if (posicion == ULTIMO) {
                    mensaje_caballo->tirada = aleat_num(1, 6) + aleat_num(1, 6);
                } else IF (posicion == MEDIO) {
                    mensaje_caballo->tirada = aleat_num(1, 6);
                } else {
                    perror("Error en la comunicación por las pipes");
                    /** liberamos memoria y mas cosas ***************************************/
                    exit(EXIT_FAILURE);
                }
            
                /* TODO: Manda el mensaje al padre */
                
            } else {
                /* Cierra la lectura en la pipe */
                close((pipes_caballos[i])[LECTURA]);
            }
            
            
            exit(EXIT_SUCCESS);
        }
    }

    
    /* Espera 30 segundos */
    /* Cada segundo actualiza el tiempo restante en la memoria compartida y manda una señal al proceso monitor */
    for (i = 0; i < ESPERA_APUESTAS; i++) {
        sleep(1);
        
        memoria_compartida->segundos_restantes--;
        
        kill(pid_monitor, SENALTIEMPORESTANTE); /* Si el proceso monitor tiene un bucle esto puede ser inutil */
    }
    
    /* Señaliza la carrera como empezada mandando senales */
    
    kill(pid_monitor, CARRERAEMPEZADA);
    kill(pid_gestor_apuestas, CARRERAEMPEZADA);
    for (i = 0; i < n_caballos; i++) {
        kill(pids_caballos[i], CARRERAEMPEZADA);
    }
    
    /****************************************************/
    /************* SIMULACION DE LA CARRERA *************/
    /****************************************************/
    
    while(1) {
        /* Manda por cada pipe a los hijos si van primeros, en el medio o últimos */
        
        /* Manda a todos los caballos una señal para que lean su posicion */
        
        /* Lee n_caballos mensajes de la cola de mensajes */
        
        /* Calcula las nuevas posiciones. Si alguno ha terminado sale del bucle */
    }
    
    /* Manda la señal de terminación a los caballos y al proceso monitor */
    
    
    
    
    
    /************************************************/
    /************* TERMINA SU EJECUCION *************/
    /************************************************/
    
    /*
    cierra las pipes
    libera todas las estructuras reservadas
    libera la cola, semaforos, memoria compartida
    */
    
    /*Eliminamos la memoria compartida*/
    retorno_shmdt = shmdt ((char *)memoria_compartida);
    if (retorno_shmdt == -1) {
        perror("Error al hacer el dettach en el proceso principal");
        exit(EXIT_FAILURE);
    }
    retorno_shmctl = shmctl (shmid, IPC_RMID, (struct shmid_ds *)NULL);
    if (retorno_shmctl == -1) {
        perror("Error al borrar la memoria en el proceso principal");
        exit(EXIT_FAILURE);
    }
}