/**
* @brief Ejercicio para la comunicacion entre procesos mediante tuberias
*
* En este fichero hacemos uso de las pipes para implementar la comunicacion
* entre un proceso padre y cuatro procesos hijos, a los cuales les manda dos
* enteros, y estos devuelven el resultado de una operacion con ellos
* @file ejercicio9.c
* @author Miguel Arconada Manteca y José Manuel Chacón Aguilera
* @date 8-3-2018
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

#define LECTURA 0        /*!< Macro que define la zona de lectura de la pipe */
#define ESCRITURA 1      /*!< Macro que define la zona de escritura de la pipe */

#define BUFFER_SIZE 512  /*!< Tamanio maximo de los buffers */
#define MENSAJE_SIZE 512 /*!< Tamanio maximo de los mensajes */



/**
 * @brief calcula el factorial de un numero
 *
 * factorial calcula el factorial de un numero pasado como argumento.
 * Se calcula de forma recursiva, devolviendo el numero multiplicado
 * por el factorial del anterior, y con la condicion de parada
 * de que factorial(1) = 1
 * @param arg estructura del tipo Param que contiene la matriz,
 * el escalar, la dimension y un identificador del hilo
 * @return void
 */
int factorial (int n);




/**
 * @brief Función principal del programa
 *
 * Este programa crea cuatro procesos hijos que se comunican con el padre,
 * e intercambian mensajes con numeros y el resultado de una operacion
 * entre ellos diferente para cada hijo
 * @param argc y argv
 * @return 0 si todo se ejecuta correctamente, y -1 en cualquier otro caso
 */
int main (int argc, char **argv) {
    
    int pipe_status;
    pid_t child_pid_1, child_pid_2, child_pid_3, child_pid_4;
    char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE], buffer3[BUFFER_SIZE], buffer4[BUFFER_SIZE];
    char mensaje_1[MENSAJE_SIZE], mensaje_2[MENSAJE_SIZE], mensaje_3[MENSAJE_SIZE], mensaje_4[MENSAJE_SIZE];
    int status;
    int valor_1, valor_2, resultado;
    
    
    /*Declaración de las 8 pipes necesarias*/
    int fd_p_h1[2], fd_h1_p[2];
    int fd_p_h2[2], fd_h2_p[2];
    int fd_p_h3[2], fd_h3_p[2];
    int fd_p_h4[2], fd_h4_p[2];


    if (argc < 3) {
        printf("Parametros insuficientes");
        return (EXIT_FAILURE);
    }
    
    /******************************************/
    
    /* Pipes Hijo1 y Padre*/
    
    pipe_status = pipe(fd_p_h1);
    if (pipe_status == -1) {
        
        perror("error creando la tuberia de padre al hijo 1\n");

        exit(EXIT_FAILURE);
    }
    
   
    pipe_status = pipe(fd_h1_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 1 al padre\n");
        exit(EXIT_FAILURE);
    }
    

    /*---------------------*/

    /* Pipes Hijo2 y Padre*/

    pipe_status = pipe(fd_p_h2);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia de padre al hijo 2\n");
        exit(EXIT_FAILURE);
    }
    
    pipe_status = pipe(fd_h2_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 2 al padre\n");
        exit(EXIT_FAILURE);
    }
    
    /*------------------------------*/

    /*Pipes entre el hijo 3 y el padre*/
    
    pipe_status = pipe(fd_p_h3);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia de padre al hijo 3\n");
        exit(EXIT_FAILURE);
    }
    
    pipe_status = pipe(fd_h3_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 3 al padre\n");
        exit(EXIT_FAILURE);
    }
    
    /*-------------------------------*/
    
    /*Pipes entre el hijo 4 y el padre*/
    
    pipe_status = pipe(fd_p_h4);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia de padre al hijo 4\n");
        exit(EXIT_FAILURE);
    }
    
    pipe_status = pipe(fd_h4_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 4 al padre\n");
        exit(EXIT_FAILURE);
    }
   
    /*--------------------------------------------------------------------------------------------*/
    
    /*Creación de hijos para los cálculos*/
    
    child_pid_1 = fork();
    if (!child_pid_1) {
        
        /*Primer hijo: potencia*/
        
        
        close(fd_p_h1[ESCRITURA]);
        close(fd_h1_p[LECTURA]);
        
        read(fd_p_h1[LECTURA], buffer1, sizeof(buffer1));
        
        if (buffer1 == NULL) {
            exit(EXIT_FAILURE);
        }
        
        sscanf(buffer1, "%d,%d", &valor_1, &valor_2);
        
        resultado = pow(valor_1, valor_2);
        
        sprintf(mensaje_1, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Potencia: %d", getpid(), valor_1, valor_2, resultado);
        
        write(fd_h1_p[ESCRITURA], mensaje_1, sizeof(mensaje_1));
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (child_pid_1 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }
        
        
        /*Primera operacion padre-hijo*/

        sprintf(mensaje_1, "%d,%d", atoi(argv[1]), atoi(argv[2]));
        
        close(fd_p_h1[LECTURA]);
        close(fd_h1_p[ESCRITURA]);
        
        write(fd_p_h1[ESCRITURA], mensaje_1, sizeof(mensaje_1));
        
        read(fd_h1_p[LECTURA], buffer1, sizeof(buffer1));
        
        wait(&status);
        
        if (status == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n", buffer1);
        
    }
    
    
    
    
    /*Crea otro hijo*/
    
   child_pid_2 = fork();
    if (!child_pid_2) {
        
        /*Segundo hijo: Factorial*/
        
        
        close(fd_p_h2[ESCRITURA]);
        close(fd_h2_p[LECTURA]);
        
        read(fd_p_h2[LECTURA], buffer2, sizeof(buffer2));
       
        if (buffer2 == NULL) {
            exit(EXIT_FAILURE);
        }
        
        sscanf(buffer2, "%d,%d", &valor_1, &valor_2);
        
        resultado = (int) (factorial(valor_1)/(int)valor_2);
        
        sprintf(mensaje_2, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Factorial: %d", getpid(), valor_1, valor_2, resultado);
        
        write(fd_h2_p[ESCRITURA], mensaje_2, sizeof(mensaje_2));
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (child_pid_2 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }
        
        
        /*Segunda operacion padre-hijo*/

        sprintf(mensaje_2, "%d,%d", atoi(argv[1]), atoi(argv[2]));

        close(fd_p_h2[LECTURA]);
        close(fd_h2_p[ESCRITURA]);
        
        write(fd_p_h2[ESCRITURA], mensaje_2, sizeof(mensaje_2));
        
        read(fd_h2_p[LECTURA], buffer2, sizeof(buffer2));
        
        wait(&status);
        
        if (status == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n", buffer2);
        
    }
    
    
    
    
    /*Crea otro hijo*/
    
    
    child_pid_3 = fork();
    if (!child_pid_3) {
        
        /*Tercer hijo: Combinatoria*/
        
        
        close(fd_p_h3[ESCRITURA]);
        close(fd_h3_p[LECTURA]);
        
        read(fd_p_h3[LECTURA], buffer3, sizeof(buffer3));
        
        if (buffer3 == NULL) {
            exit(EXIT_FAILURE);
        }
        
        sscanf(buffer3, "%d,%d", &valor_1, &valor_2);
        
        resultado = factorial(valor_1) /(factorial(valor_2)*factorial(valor_1-valor_2));
        
        
        sprintf(mensaje_3, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Combinatoria: %d", getpid(), valor_1, valor_2, resultado);
       
        write(fd_h3_p[ESCRITURA], mensaje_3, sizeof(mensaje_3));
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (child_pid_3 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }
        
        
        /*Tercera operacion padre-hijo*/

        sprintf(mensaje_3, "%d,%d", atoi(argv[1]), atoi(argv[2]));
       
        
        close(fd_p_h3[LECTURA]);
        close(fd_h3_p[ESCRITURA]);
        
        write(fd_p_h3[ESCRITURA], mensaje_3, sizeof(mensaje_3));
        
        read(fd_h3_p[LECTURA], buffer3, sizeof(buffer3));
        
        wait(&status);
        
        if (status == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n", buffer3);
        
    }
    
    
    
    
    
    
    
    
    
    
    /*Crea otro hijo*/
    
    
    child_pid_4 = fork();
    if (!child_pid_4) {
        
        /*Cuarto hijo: Suma absolutos*/
        
        
        close(fd_p_h4[ESCRITURA]);
        close(fd_h4_p[LECTURA]);
        
        read(fd_p_h4[LECTURA], buffer4, sizeof(buffer4));
       
        if (buffer4 == NULL) {
            exit(EXIT_FAILURE);
        }
        
        sscanf(buffer4, "%d,%d", &valor_1, &valor_2);
        
        resultado = abs(valor_1) + abs(valor_2);
        
        sprintf(mensaje_4, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Suma_absolutos: %d", getpid(), valor_1, valor_2, resultado);
        
        write(fd_h4_p[ESCRITURA], mensaje_4, sizeof(mensaje_4));
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (child_pid_4 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }
        
        
        /*Cuarta operacion padre-hijo*/

        sprintf(mensaje_4, "%d,%d", atoi(argv[1]), atoi(argv[2]));
        
        
        close(fd_p_h4[LECTURA]);
        close(fd_h4_p[ESCRITURA]);
        
        write(fd_p_h4[ESCRITURA], mensaje_4, sizeof(mensaje_4));
        
        read(fd_h4_p[LECTURA], buffer4, sizeof(buffer4));
        
        wait(&status);
        
        if (status == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n", buffer4);
        
    }
    
    

    
    exit (EXIT_SUCCESS);
}



/****************************************************************/

/*Funciones auxiliares*/

int factorial (int n) {
    int i;
    int fact;

    if (n < 0) {
        return 0;
    }
    
    if (n == 0) {
        return 1;
    }
    
    i = 0;
    fact = 1;

    for (i = 1; i <= n; i++){
       fact = fact * i;
    }
    
   return fact;
}