/**
 * @brief Proceseo Gestor del Proyecto Final de la Asignatura
 * 
 * Este Proceso es el encargado de gestionar todo lo relacionado con 
 * las apuestas sobre la carrera de caballos. Es el encargado de
 * inicializar las apuestas y posteriormente generar todos los hilos
 * que desempeñan la funcion de las ventanillas de apuestas.
 * 
 * @file proceso_gestor.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 4-5-2018
 */



/*
1- Inicializa las apuestas:
a. Total de dinero apostado a cada caballo = 1.0
b. Cotización de cada caballo = <total dinero apostado a todos los
caballos hasta ese momento> / <total del dinero apostado al caballo
hasta ese momento>
c. Dinero a pagar a cada apostador para cada caballo = 0
2- Inicializa tantos threads como ventanillas de gestión de apuestas
3- Recibe mensajes de apuestas en una cola
4- Los mensajes de apuestas que se van recibiendo son procesados por los
threads “ventanilla”
5- Sólo se procesan apuestas hasta el comienzo de la carrera. Está prohibido
procesar ninguna apuesta una vez comenzada la carrera.
6- Cada ventanilla:
a. Asume uno de los mensajes de apuesta
b. Comprueba el caballo de la apuesta
c. Se le asigna al apostador la cantidad que se le pagara en caso de que
el caballo gane = <dinero apostado> * <cotización del caballo>
d. Se actualiza la cotización de los caballos:
i. <Cotización de un caballo> = <total dinero apostado a todos
los caballos> / <total dinero apostado al caballo>
*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define KEY 1300 /*!< KEY necesaria para el ftok */
#define FILEKEY "/bin/bash" /*!< FILEKEY necesario para ftok */
/**
 * @brief Funcion que desempeña la ejecucion de cada ventanilla
 * 
 * @return void
 * 
 */
void *ventanilla();

int main (int argc, char **argv){ /*Primer parámetro, número de caballos*//*Segundo Parametro, nº ventanillas*/
    int i;
    int j;
    int n_caballos;
    int n_pthreads;
    double *apostado;
    double *cotizacion;
    pthread_t *pthread_array;
    
    if(argc<3){
        perror("Not enough parameters");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) - getpid()); /*Seed para generación de números aleatorios*/
    
    n_caballos = atoi(argv[1]);
    n_pthreads = atoi(argv[2]);
    
    /*Inicialización*/
    apostado = (double *)malloc(n_caballos*sizeof(double));
    if (apostado == NULL){
        perror("Error en la reserva de memoria 1-apostado");
        exit(EXIT_FAILURE);
    }
    cotizacion  = (double *)malloc(n_caballos*sizeof(double));
    if (cotizacion == NULL){
        perror("Error en la reserva de memoria 1-apostado");
        free(apostado);
        exit(EXIT_FAILURE);
    }

    /*************************El array de cotizacion y apostado va de 1 al numero de caballos*/
    apostado[0] = 0;
    cotizacion[0] = 0;
    
    for (i = 1; i <= n_caballos; i++){
        apostado[1] = 1.0;
        cotizacion[1] = n_caballos;
    }
    
    pthread_array = (pthread*)malloc(n_pthreads*sizeof(pthread));
    if (pthread_array == NULL){
        free(apostado);
        free(cotizacion);
        exit(EXIT_FAILURE);
    }
    
    for (j = 0; j< n_pthreads; j++){
        pthread_create(&pthread_array[j], NULL, ventanilla, NULL);
        pthread_join(&pthread_array[j], NULL);
    }
    
    /*Cuando la carrera acabe */
    for (k =0; k< n_pthreads; k++){
        pthread_cancel(pthread_array[k]);
    }
    free(pthread_array);
    free(apostado); /*????????????????*/
    free(cotizacion); /*????????????????*/
}

void *ventanilla(){
    int caballo;
    int apostador;
    
    while (1){
        /*Recibir Mensaje*/
        /*Leer Datos del Mensaje y guardarlos en las variables propias de la funcion ventanilla*/
        
    }
    
    
}