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
#include <limits.h>
#include <time.h>


#include "estructuras.h"
#include "aleat_num.h"
#include "semaforos.h"
#include "memoria_compartida.h"

#define KEY 1300 /*!< KEY necesaria para el ftok */
#define FILEKEY "/bin/bash" /*!< FILEKEY necesario para ftok */
#define TAMANIO_ARGUMENTOS_EXEC 256 /*!< Tamaño máximo de los parámetros enviados a los distintos procesos*/
#define ESPERA_APUESTAS 30

pid_t pid_monitor, pid_gestor_apuestas, pid_apostador;
pid_t pids_caballos[MAX_CABALLOS];
int n_caballos;

void manejador_vacio(int senal);
void manejador_sigint(int senal);

int main(int argc, char ** argv) {
    int i;
    int n_apostadores, n_ventanillas;
    char *numero_apostadores_arg, *numero_caballos_arg, *key_arg, *numero_ventanillas_arg;
    int longitud_carrera;
    double dinero_apostadores;
    key_t key;
    int shmid;
    Memoria_Compartida *memoria_compartida;
    int pipe_status;
    int pipes_caballos[MAX_CABALLOS][2];
    int posicion;
    Mensaje_Tirada_Caballo *mensaje_caballo, *mensaje_caballo_principal;
    int retorno_shmdt, retorno_shmctl, retorno_envio, retorno_recepcion, retorno_msgctl, retorno_semaforo;
    int msqid, semid;
    int mejor_posicion, peor_posicion;
    int carrera_terminada;
    int id_caballo_mensaje, tirada_mensaje;
    sigset_t set, oset;
    unsigned short valores_iniciales_semaforos[NUM_SEMAFOROS];
    
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
    
    /* Comprueba la linea de comandos */
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
            sscanf(argv[++i], "%lf", &dinero_apostadores);
        } else {
            fprintf(stderr, "Parametro %s es incorrecto\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    
    /* Comprueba que los valores estan entre el maximo y el minimo de cada tipo */
    if (n_caballos < 1 || n_caballos > MAX_CABALLOS) {
        printf("El valor del parametro n_caballos tiene que estar comprendido entre 1 y %d", MAX_CABALLOS);
        exit(EXIT_FAILURE);
    }
    if (longitud_carrera < MIN_LONGITUD || longitud_carrera > MAX_LONGITUD) {
        printf("El valor del parametro logitud_carrera tiene que estar comprendido entre %d y %d", MIN_LONGITUD, MAX_LONGITUD);
        exit(EXIT_FAILURE);
    }
    if (n_apostadores < 1 || n_apostadores > MAX_APOSTADORES) {
        printf("El valor del parametro n_apostadores tiene que estar comprendido entre 1 y %d", MAX_APOSTADORES);
        exit(EXIT_FAILURE);
    }
    if (n_ventanillas < 1 || n_ventanillas > MAX_VENTANILLAS) {
        printf("El valor del paramtro n_ventanillas tiene que estar comprendido entre 1 y %d", MAX_VENTANILLAS);
        exit(EXIT_FAILURE);
    }
    if (dinero_apostadores < MIN_DINERO || dinero_apostadores > MAX_DINERO) {
        printf("El valor del paramtro dinero_apostadores tiene que estar comprendido entre %d y %d", MIN_DINERO, MAX_DINERO);
        exit(EXIT_FAILURE);
    }
    
    /* Genera una seed para generar los numeros aleatorios */
    srand(time(NULL) - getpid());
    
    /* Calcula la key para la memoria compartida y los semaforos */
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        perror("Error al usar ftok");
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
        perror("Error al conseguir la memoria compartida en el proceso principal");
        exit(EXIT_FAILURE);
    }
    
    /* Inicializa la memoria compartida para evitar valores erroneos */
    memoria_compartida->segundos_restantes = 30;
    memoria_compartida->n_apuestas = 0;
    memoria_compartida->n_caballos = n_caballos;
    memoria_compartida->n_apostadores = n_apostadores;
    
    for (i = 0; i < MAX_APOSTADORES; i++) {
        memoria_compartida->apostadores[i].cantidad_apostada = 0;
        memoria_compartida->apostadores[i].beneficios = 0;
        memoria_compartida->apostadores[i].dinero_restante = dinero_apostadores;
        sprintf(memoria_compartida->apostadores[i].nombre, "Apostador %d", i + 1);
    }
    
    for (i = 0; i < MAX_CABALLOS; i++) {
        memoria_compartida->caballos[i].posicion = 0;
        memoria_compartida->caballos[i].ultima_tirada = 0;
        memoria_compartida->caballos[i].total_apostado = 1;
        memoria_compartida->caballos[i].cotizacion = 0;
    }
    
    for (i = 0; i < MAX_APUESTAS; i++) {
        memoria_compartida->historial_apuestas[i].numero_caballo = 0;
        memoria_compartida->historial_apuestas[i].cuantia = 0;
        memoria_compartida->historial_apuestas[i].ventanilla = 0;
        memoria_compartida->historial_apuestas[i].cotizacion_previa = 0;
        sprintf(memoria_compartida->historial_apuestas[i].nombre, "Apostador %d", 0);
    }
    
    for (i = 0; i < 10; i++) {
        memoria_compartida->top_apostadores[i] = 0;
    }
    
    memoria_compartida->n_caballos_ganadores = 0;
    for (i = 0; i < MAX_CABALLOS; i++) {
        memoria_compartida->caballos_ganadores[i] = -1;
    }
    

    /* Crea la cola de mensajes */
    msqid = msgget (key, IPC_CREAT | IPC_EXCL | 0660);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes en el principal");
        exit(EXIT_FAILURE);
    }
    
    
    /* Crea el array de semaforos */
    retorno_semaforo = Crear_Semaforo(key, NUM_SEMAFOROS, &semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al obtener semid");
        exit(EXIT_FAILURE);
    }
    
    /* Inicializa el array de semaforos a los valores deseados (todos a 0 menos el de beneficios calculados) */
    for (i = 0; i < NUM_SEMAFOROS; i++) {
        valores_iniciales_semaforos[i] = 1;
    }
    valores_iniciales_semaforos[MUTEX_BENEFICIOS_CALCULADOS] = 0;
    retorno_semaforo = Inicializar_Semaforo(semid, valores_iniciales_semaforos);
    if (retorno_semaforo == ERROR) {
        perror("Error al inicializar los semaforos");
        exit(EXIT_FAILURE);
    }
    
    /* Inicializa los pids de los procesos que va a crear a -1 para evitar valores basura */
    pid_apostador = -1;
    pid_gestor_apuestas = -1;
    pid_monitor = -1;
    for (i = 0; i < MAX_CABALLOS; i++) {
        pids_caballos[i] = -1;
    }
    


    /* Crea los parametros que van a ser usados en los exec */
    
    
    numero_apostadores_arg = (char *)malloc(TAMANIO_ARGUMENTOS_EXEC * sizeof(char));
    if (numero_apostadores_arg == NULL) {
        perror("Error al reservar el argumento numero_apostadores_arg");
        exit(EXIT_FAILURE);
    }
    sprintf(numero_apostadores_arg, "%d", n_apostadores);
    
    numero_caballos_arg = (char *)malloc(TAMANIO_ARGUMENTOS_EXEC * sizeof(char));
    if (numero_caballos_arg == NULL) {
        perror("Error al reservar el argumento numero_caballos_arg");
        exit(EXIT_FAILURE);
    }
    sprintf(numero_caballos_arg, "%d", n_caballos);
    
    numero_ventanillas_arg = (char *)malloc(TAMANIO_ARGUMENTOS_EXEC * sizeof(char));
    if (numero_ventanillas_arg == NULL) {
        perror("Error al reservar el argumento numero_ventanillas_arg");
        exit(EXIT_FAILURE);
    }
    sprintf(numero_ventanillas_arg, "%d", n_ventanillas);
    
    key_arg = (char *)malloc(TAMANIO_ARGUMENTOS_EXEC * sizeof(char));
    if (key_arg == NULL) {
        perror("Error al reservar el argumento key_arg");
        exit(EXIT_FAILURE);
    }
    sprintf(key_arg, "%d", key);
    
    
    
    /* El proceso principal crea: */
    /* 1- El proceso monitor */

    pid_monitor = fork();
    if (pid_monitor == -1) {
        perror("Error al crear el proceso monitor");
        exit(EXIT_FAILURE);
    } else if (pid_monitor == 0) {
        execlp("./proceso_monitor", "proceso_monitor", key_arg, NULL);
        perror("Error en el execlp del proceso monitor");
        exit(EXIT_FAILURE);
    }
    
    /* 2- El gestor de apuestas */
    
    pid_gestor_apuestas = fork();
    if (pid_gestor_apuestas == -1) {
        perror("Error al crear el proceso gestor de apuestas");
        exit(EXIT_FAILURE);
    } else if (pid_gestor_apuestas == 0) {
        execlp("./proceso_gestor_apuestas", "proceso_gestor_apuestas", numero_caballos_arg, numero_ventanillas_arg, NULL);
        perror("Error en el execlp del proceso gestor de apuestas");
        exit(EXIT_FAILURE);
    }
    
    /* 3- El proceso apostador */
    
    pid_apostador = fork();
    if (pid_apostador == -1) {
        perror("Error al crear el proceso apostador");
        exit(EXIT_FAILURE);
    } else if (pid_apostador == 0) {
        execlp("./proceso_apostador", "proceso_apostador", numero_apostadores_arg, numero_caballos_arg, NULL);
        perror("Error en el execlp del proceso apostador");
        exit(EXIT_FAILURE);
    }
    
    /* 4- Los caballos */
    
    /* Crea las pipes necesarias para la comunicación */
    for (i = 0; i < n_caballos; i++) {
        pipe_status = pipe(pipes_caballos[i]);
        if (pipe_status == -1) {
            perror("error creando las pipes\n");
            exit(EXIT_FAILURE);
        }
    
    }
    
    for (i = 0; i < n_caballos; i++) {
        pids_caballos[i] = fork();
        
        if (pids_caballos[i] == -1) {
            perror("Error al crear caballos");
            exit(EXIT_FAILURE);
        } else if (pids_caballos[i] == 0) {
            
            /*******************************************/
            /******** EJECUCION DE CADA CABALLO ********/
            /*******************************************/
            
            /* Genera una seed para generar los numeros aleatorios */
            srand(time(NULL) - getpid());
            
            /* Prepara el manejador */
            if (signal(SENALCABALLOLEEPOSICION, manejador_vacio) == SIG_ERR){
                perror("Error en el manejador SENALCABALLOLEEPOSICION");
                exit(EXIT_FAILURE);
            }
            
            /* Bloquea todas las senales menos SENALCABALLOLEEPOSICION (y SIGTERM) */
            if (sigfillset(&set) == -1) {
                perror("Error con sigfillset");
                exit(EXIT_FAILURE);
            }
            if (sigdelset(&set, SIGTERM) == -1) {
                /* No bloqueamos la senal SIGTERM para poder matar el proceso desde la terminal si es necesario*/
                perror("Error con sigdelset");
                exit(EXIT_FAILURE);
            }
            if (sigdelset(&set, SIGINT) == -1) {
                /* No bloqueamos la senal SIGINT para poder matar el proceso desde la terminal si es necesario*/
                perror("Error con sigdelset");
                exit(EXIT_FAILURE);
            }
            if (sigdelset(&set, SENALCABALLOLEEPOSICION) == -1) {
                perror("Error con sigdelset para SENALCABALLOLEEPOSICION");
                exit(EXIT_FAILURE);
            }
            if (sigprocmask (SIG_BLOCK, &set, &oset) == -1) {
                perror("Error con sigprocmask");
                exit(EXIT_FAILURE);
            }
            
            
            /* Cierra la escritura en la pipe */
            close((pipes_caballos[i])[ESCRITURA]);
            
            while(1) {
                /* Espera la señal del padre */
                pause();
                
                /* Reserva una estructura de mensaje (al hacerlo cada iteración nos evitamos memoria sin liberar al final) */
                mensaje_caballo = (Mensaje_Tirada_Caballo *)malloc(sizeof(Mensaje_Tirada_Caballo));
                if (mensaje_caballo == NULL) {
                    perror("Error al reservar memoria para el mensaje en el caballo");
                    exit(EXIT_FAILURE);
                }
                mensaje_caballo->mtype = MENSAJE_CABALLO_A_PRINCIPAL;
                mensaje_caballo->info.id_caballo = i;
                
                /* Lee de la pipe su posición en la carrera */
                read((pipes_caballos[i])[LECTURA], &posicion, sizeof(int));
            
                if (posicion == PRIMERO) {
                    /* Tirada por ir en primer lugar */
                    mensaje_caballo->info.tirada = aleat_num(1, 7);
                    
                } else if (posicion == ULTIMO) {
                    /* Tirada de remontada */
                    mensaje_caballo->info.tirada = aleat_num(1, 6) + aleat_num(1, 6);
                    
                } else if (posicion == MEDIO) {
                    /* Tirada normal */
                    mensaje_caballo->info.tirada = aleat_num(1, 6);
                    
                } else if (posicion == CARRERAYATERMINADA) {
                    exit(EXIT_SUCCESS);
                    
                } else {
                    perror("Error en la comunicación por las pipes");
                    exit(EXIT_FAILURE);
                }
            
                /* Manda el mensaje al padre */
                retorno_envio = msgsnd (msqid, (struct Mensaje_Tirada_Caballo *) mensaje_caballo, sizeof(Mensaje_Tirada_Caballo) - sizeof(long), 0);
                if (retorno_envio == -1) {
                    perror("Error al mandar el mensaje en el caballo");
                    
                    /* Libera la estructura del mensaje*/
                    free(mensaje_caballo);
                    
                    exit(EXIT_FAILURE);
                }
                
                /* Liberamos la estructura del mensaje */
                free(mensaje_caballo);
            }
            
            exit(EXIT_SUCCESS);
        } else {
            /* Cierra la lectura en la pipe de cada caballo para luego escribir*/
            close((pipes_caballos[i])[LECTURA]);
        }
    }
    
    /* Prepara el manejador de la senal si el usuario presiona ctrl+C */
    if (signal(SIGINT, manejador_sigint) == SIG_ERR){
        perror("Error en el manejador SIGINT");
        exit(EXIT_FAILURE);
    }
    
    /* Bloquea todas las senales menos SIGINT y SIGTERM */
    if (sigfillset(&set) == -1) {
        perror("Error con sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SIGINT) == -1) {
        /* No bloqueamos la senal SIGINT para poder matar el proceso desde la terminal si es necesario*/
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigdelset(&set, SIGTERM) == -1) {
        /* No bloqueamos la senal SIGTERM para poder matar el proceso desde la terminal si es necesario*/
        perror("Error con sigdelset");
        exit(EXIT_FAILURE);
    }
    if (sigprocmask (SIG_BLOCK, &set, &oset) == -1) {
        perror("Error con sigprocmask");
        exit(EXIT_FAILURE);
    }

    
    /* Espera 30 segundos */
    /* Cada segundo actualiza el tiempo restante en la memoria compartida y manda una señal al proceso monitor */
    for (i = 0; i < ESPERA_APUESTAS; i++) {
        sleep(1);
        
        memoria_compartida->segundos_restantes--;
        
        kill(pid_monitor, SENALTIEMPORESTANTE); 
    }
    
    /* Señaliza la carrera como empezada mandando senales */
    
    kill(pid_monitor, SENALESTADOCARRERACAMBIA); /* Manda la senal al proceso monitor */
    kill(pid_gestor_apuestas, SENALESTADOCARRERACAMBIA); /* Manda la senal al proceso gestor */
    for (i = 0; i < n_caballos; i++) {
        kill(pids_caballos[i], SENALESTADOCARRERACAMBIA); /* Manda la senal a los caballos */
    }
    kill(pid_apostador, SENALESTADOCARRERACAMBIA); /* Manda la señal al proceso apostador */
    
    
    
    /****************************************************/
    /************* SIMULACION DE LA CARRERA *************/
    /****************************************************/
    
    while(1) {
        /* Calcula la mejor y peor posición de los caballos */
        
        mejor_posicion = -1;
        peor_posicion = INT_MAX;
        
        for (i = 0; i < n_caballos; i++) {
            if (memoria_compartida->caballos[i].posicion > mejor_posicion) {
                mejor_posicion = memoria_compartida->caballos[i].posicion;
            }
            if (memoria_compartida->caballos[i].posicion < peor_posicion) {
                peor_posicion = memoria_compartida->caballos[i].posicion;
            }
        }
        
        /* Manda por cada pipe a los hijos si van primeros, en el medio o últimos */
        for (i = 0; i < n_caballos; i++) {
            if (memoria_compartida->caballos[i].posicion == mejor_posicion && memoria_compartida->caballos[i].posicion != 0) {
                /* Manda que va en primera posicion si no es la primera iteración */
                posicion = PRIMERO;
                write((pipes_caballos[i])[ESCRITURA], &posicion, sizeof(int));
            } else if (memoria_compartida->caballos[i].posicion == peor_posicion  && memoria_compartida->caballos[i].posicion != 0) {
                /* Manda que va en la peor posicion si no es la primera iteración*/
                posicion = ULTIMO;
                write((pipes_caballos[i])[ESCRITURA], &posicion, sizeof(int));
            } else {
                /* Manda que va en una posicion del medio */
                posicion = MEDIO;
                write((pipes_caballos[i])[ESCRITURA], &posicion, sizeof(int));
            }
        }
        
        
        
        /* Manda a todos los caballos una señal para que lean su posicion */
        for (i = 0; i < n_caballos; i++) {
            kill(pids_caballos[i], SENALCABALLOLEEPOSICION);
        }
        
        /* Reserva una estructura de mensaje (al hacerlo cada iteración nos evitamos memoria sin liberar al final) */
        mensaje_caballo_principal = (Mensaje_Tirada_Caballo *)malloc(sizeof(Mensaje_Tirada_Caballo));
        if (mensaje_caballo_principal == NULL) {
            perror("Error al reservar memoria para el mensaje en el proceso principal");
            exit(EXIT_FAILURE);
        }
        
        /* Lee n_caballos mensajes de la cola de mensajes */
        carrera_terminada = FALSE;
        for (i = 0; i < n_caballos; i++) {
            retorno_recepcion = msgrcv(msqid,  (Mensaje_Tirada_Caballo*)mensaje_caballo_principal, sizeof(Mensaje_Tirada_Caballo) - sizeof(long), MENSAJE_CABALLO_A_PRINCIPAL, 0);
            
            if (retorno_recepcion == -1) {
                perror("Error al recibir el mensaje en el proceso principal");
                
                /* Libera la estructura del mensaje*/
                free(mensaje_caballo_principal);
                
                exit(EXIT_FAILURE);
            }
            
            /* Extrae los datos recibidos en el mensaje */
            id_caballo_mensaje = mensaje_caballo_principal->info.id_caballo;
            tirada_mensaje = mensaje_caballo_principal->info.tirada;
            
            /* Calcula las nuevas posiciones */
            memoria_compartida->caballos[id_caballo_mensaje].posicion += tirada_mensaje;
            memoria_compartida->caballos[id_caballo_mensaje].ultima_tirada = tirada_mensaje;
            
            /* Comprueba si algun caballo ha terminado ya la carrera */
            if (memoria_compartida->caballos[id_caballo_mensaje].posicion >= longitud_carrera) {
                carrera_terminada = TRUE;
            }
        }
        
        /* Libera la estructura del mensaje */
        free(mensaje_caballo_principal);
        
        /* Indica al proceso monitor que ha actualizado los datos de la carrera para ser imprimidos */
        kill(pid_monitor, SENALDATOSCARRERAACTUALIZADOS);
        
        /* Si alguno ha terminado les manda el mensaje mediante las pipes y sale del bucle */
        if (carrera_terminada == TRUE) {
            
            posicion = CARRERAYATERMINADA;
            for (i = 0; i < n_caballos; i++) {
                /* Actualiza su posicion como terminada */
                write((pipes_caballos[i])[ESCRITURA], &posicion, sizeof(int));
                
                /* Les indica que deben leer su nueva posición */
                kill(pids_caballos[i], SENALCABALLOLEEPOSICION);
            }
            
            /* Se sale del bucle */
            break;
        }
    }
    
    /* Guarda en memoria compartida los caballos ganadores */
    for (i = 0; i < n_caballos; i++) {
        if (memoria_compartida->caballos[i].posicion >= longitud_carrera) {
            memoria_compartida->caballos_ganadores[memoria_compartida->n_caballos_ganadores] = i;
            memoria_compartida->n_caballos_ganadores++;
        }
    }
    
    /* Manda la señal de terminación al proceso monitor y al gestor y espera a los caballos */
    for (i = 0; i < n_caballos; i++) {
        waitpid(pids_caballos[i], NULL, 0);
    }
    kill(pid_gestor_apuestas, SENALESTADOCARRERACAMBIA);
    kill(pid_monitor, SENALESTADOCARRERACAMBIA);
    
    
    /************************************************/
    /************* TERMINA SU EJECUCION *************/
    /************************************************/
    
    /* Espera a todos los procesos que ha creado */
    while(wait(NULL) > 0);
    
    
    /* TODO libera todas las estructuras reservadas */
    
    /* Libera la cola de mensajes */
    retorno_msgctl = msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
    if (retorno_msgctl != 0) {
        perror("Error al liberar la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    
    /* Libera los semaforos */
    retorno_semaforo = Borrar_Semaforo(semid);
    if (retorno_semaforo == ERROR) {
        perror("Error al borrar los semaforos");
        exit(EXIT_FAILURE);
    }

    /* Elimina la memoria compartida */
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
    
    exit(EXIT_SUCCESS);
}


/******************************************************************/
/*********************** FUNCIONES PRIVADAS ***********************/
/******************************************************************/

void manejador_vacio(int senal) {}

void manejador_sigint(int senal) {
    int i;
    
    /* Manda la correspondiente senal de terminacion a todos los procesos creados */
    kill(pid_gestor_apuestas, SENALINTERRUPCIONUSUARIO);
    kill(pid_apostador, SENALINTERRUPCIONUSUARIO);
    kill(pid_monitor, SENALINTERRUPCIONUSUARIO);
    for (i = 0; i < n_caballos; i++) {
        kill(pids_caballos[i], SENALINTERRUPCIONUSUARIO);
    }
    
    while(wait(NULL) > 0);
    
    
    /* TODO Libera todos los recursos usados */
}