#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

#define LECTURA 0
#define ESCRITURA 1

#define BUFFERSIZE 128
#define SIZE 512



int factorial (int n) {
    if (n < 0) {
        return -1;
    }
    if (n == 0) {
        return 1;
    }
    
    return n * factorial(n);
}




int main (int argc, char **argv) {
    
    int pipe_status;
    pid_t childpid1, childpid2, childpid3, childpid4;
    char buffer[BUFFERSIZE];
    char mensaje[SIZE];
    int status;
    int valor1, valor2, resultado;
    
    if (argc < 3) {
        return (EXIT_FAILURE);
    }
    
    
    
    
    
    /*Decaración de las 8 pipes necesarias*/
    int fdp_h1[2], fdh1_p[2];
    int fdp_h2[2], fdh2_p[2];
    int fdp_h3[2], fdh3_p[2];
    int fdp_h4[2], fdh4_p[2];
    
    /*----------------------*/
    
    /* Pipes Hijo1 y Padre*/
    
    pipe_status = pipe(fdp_h1);
    if (pipe_status == -1) {
        
        perror("error creando la tuberia de padre al hijo 1\n");

        exit(EXIT_FAILURE);
    }
    
   
    pipe_status = pipe(fdh1_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 1 al padre\n");
        exit(EXIT_FAILURE);
    }
    

    /*---------------------*/

    /* Pipes Hijo2 y Padre*/

    pipe_status = pipe(fdp_h2);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia de padre al hijo 2\n");
        exit(EXIT_FAILURE);
    }
    
    pipe_status = pipe(fdh2_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 2 al padre\n");
        exit(EXIT_FAILURE);
    }
    
    /*------------------------------*/

    /*Pipes entre el hijo 3 y el padre*/
    
    pipe_status = pipe(fdp_h3);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia de padre al hijo 3\n");
        exit(EXIT_FAILURE);
    }
    
    pipe_status = pipe(fdh3_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 3 al padre\n");
        exit(EXIT_FAILURE);
    }
    
    /*-------------------------------*/
    
    /*Pipes entre el hijo 4 y el padre*/
    
    pipe_status = pipe(fdp_h4);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia de padre al hijo 4\n");
        exit(EXIT_FAILURE);
    }
    
    pipe_status = pipe(fdh4_p);
    if (pipe_status == -1) {
        
        perror("Error creando la tuberia del hijo 4 al padre\n");
        exit(EXIT_FAILURE);
    }
   
    /*--------------------------------------------------------------------------------------------*/
    
    /*Creación de hijos para los cálculos*/
    
    childpid1 = fork();
    if (!childpid1) {
        
        /*Primer hijo: potencia*/
        
        
        close(fdp_h1[ESCRITURA]);
        close(fdh1_p[LECTURA]);
        
        read(fdp_h1[LECTURA], buffer, sizeof(buffer));
        
        if (buffer == NULL) {
            exit(EXIT_FAILURE);
        }
        
        sscanf(buffer, "%d,%d", &valor1, &valor2);
        
        resultado = pow(valor1, valor2);
        
        sprintf(mensaje, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Potencia: %d", getpid(), valor1, valor2, resultado);
        
        write(fdh1_p[ESCRITURA], mensaje, sizeof(mensaje));
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (childpid1 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }
        
        
        /*Primera operacion padre-hijo*/

        sprintf(mensaje, "%d,%d", atoi(argv[1]), atoi(argv[2]));
        
        close(fdp_h1[LECTURA]);
        close(fdh1_p[ESCRITURA]);
        
        write(fdp_h1[ESCRITURA], mensaje, sizeof(mensaje));
        
        read(fdh1_p[LECTURA], buffer, sizeof(buffer));
        
        wait(&status);
        
        if (status == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n", buffer);
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    /*Crea otro hijo*/
    
    childpid2 = fork();
    if (!childpid2) {
        
        /*Segundo hijo: factorial*/
        
        close(fdp_h2[ESCRITURA]);
        close(fdh2_p[LECTURA]);
        
        read(fdp_h2[LECTURA], buffer, sizeof(buffer));
        
        sscanf(buffer, "%d,%d", &valor1, &valor2);
        
        resultado = factorial(valor1)/valor2;
        
        sprintf(mensaje, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Factorial: %d", getpid(), valor1, valor2, resultado);
        
        write(fdh2_p[ESCRITURA], mensaje, sizeof(mensaje));
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (childpid2 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }
        
        
        
        /*Segunda operacion padre-hijo*/

        sprintf(mensaje, "%d,%d", atoi(argv[1]), atoi(argv[2]));
        
        close(fdp_h2[LECTURA]);
        close(fdh2_p[ESCRITURA]);
        
        write(fdp_h2[ESCRITURA], mensaje, sizeof(mensaje));
        
        read(fdh2_p[LECTURA], buffer, sizeof(buffer));
        
        wait(&status);
        
        if (status == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n", buffer);
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    /*Crea otro hijo*/
    
    
    childpid3 = fork();
    if (!childpid3) {
        
        /*Tercer hijo: combinatoria*/
        
        sprintf(mensaje, "%d,%d", atoi(argv[1]), atoi(argv[2]));
        
        close(fdp_h3[ESCRITURA]);
        close(fdh3_p[LECTURA]);
        
        read(fdp_h3[LECTURA], buffer, sizeof(buffer));
        
        if (buffer == NULL) {
            exit(EXIT_FAILURE);
        }
        
        sscanf(buffer, "%d,%d", &valor1, &valor2);
        
        resultado = factorial(valor1) / (factorial(valor2) * factorial(valor1 - valor2));
        
        sprintf(mensaje, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Combinatoria: %d", getpid(), valor1, valor2, resultado);
        
        write(fdh3_p[ESCRITURA], mensaje, sizeof(mensaje));
        
        exit(EXIT_SUCCESS);
        
        
    } else {
        if (childpid3 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }

        /*tercera operacion padre-hijo*/

        close(fdp_h3[LECTURA]);
        close(fdh3_p[ESCRITURA]);

        write(fdp_h3[ESCRITURA], mensaje, sizeof(mensaje));

        read(fdh3_p[LECTURA], buffer, sizeof(buffer));

        wait(&status);

        if (status == EXIT_FAILURE) {
           exit(EXIT_FAILURE);
        }

        printf("%s\n", buffer);
        
        
        
    }
    
    
    
    
    
    
    
    
    
    
    /*Crea otro hijo*/
    
    
    childpid4 = fork();
    if (!childpid4) {
        
        /*Cuarto hijo: valor absoluto*/
        
        sprintf(mensaje, "%d,%d", atoi(argv[1]), atoi(argv[2]));
        
        close(fdp_h4[ESCRITURA]);
        close(fdh4_p[LECTURA]);
        
        read(fdp_h4[LECTURA], buffer, sizeof(buffer));
        
        if (buffer == NULL) {
            exit(EXIT_FAILURE);
        }
        
        sscanf(buffer, "%d,%d", &valor1, &valor2);
        
        resultado = (int) (abs(valor1) + abs(valor2));
        
        sprintf(mensaje, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Suma_absolutos: %d", getpid(), valor1, valor2, resultado);
        
        write(fdh4_p[ESCRITURA], mensaje, sizeof(mensaje));
        
        exit(EXIT_SUCCESS);
        
        
    } else {
        if (childpid4 == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }

        /*Cuarta operacion padre-hijo*/

        close(fdp_h4[LECTURA]);
        close(fdh4_p[ESCRITURA]);

        write(fdp_h4[ESCRITURA], mensaje, sizeof(mensaje));

        read(fdh4_p[LECTURA], buffer, sizeof(buffer));

        wait(&status);

        if (status == EXIT_FAILURE) {
           exit(EXIT_FAILURE);
        }

        printf("%s\n", buffer);
        
        
        
    }
    
    

    
    exit (EXIT_SUCCESS);
}