/**
 * @brief Ejercicio 8 de la Práctica
 * 
 * En este ejercicio lanzamos tantos procesos hijo como escpecificados
 * en los parámetros de entrada de la función main y además hacemos que
 * estos procesos hijos ejecuten un código distinto al de este programa
 * todo esto lo conseguimos con una combinación de las rutinas fork() y
 * la familia de rutinas exec().
 * 
 * @file ejercicio8.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 8-3-2018
 */
 
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_PATH 256 /*!< Tamaño máximo de la ruta*/


int main(int argc, char **argv){
    char **argumentospadre = NULL;
    char **argumentoshijo = NULL;
    char path[MAX_PATH];
    
    if (argc < 3) {
        exit(EXIT_SUCCESS);
    }
    
    strcpy(path, "/bin/");
    
    if (fork()) {  /* Es el padre */
    
        wait();
    
        if (strcmp(argv[1], "du") == 0) {
            strcpy(path, "/usr/bin/");
        }
        
        
        if (strcmp(argv[argc - 1], "-l") == 0) {
            
            strcat(path, argv[1]);
            
            printf("%s", path);
            
            execl(path, argv[1], NULL);
            
            perror("fallo en execl");
            exit(EXIT_FAILURE);
            
        } else if (strcmp(argv[argc - 1], "-lp") == 0){
            
            execlp(argv[1], argv[1], NULL);
            
            perror("fallo en execlp");
            exit(EXIT_FAILURE);
            
        } else if (strcmp(argv[argc - 1], "-v") == 0){
            
            argumentoshijo = (char **)malloc(sizeof(2 * sizeof(char *)));
            argumentoshijo[0] = (char *)malloc( (strlen(argv[1]) + 1) * sizeof(char));
            strcpy(argumentoshijo[0], argv[1]);
            argumentoshijo[1] = NULL;
            
            strcat(path, argv[1]);
            
            execv(path, argumentoshijo);
            
            perror("fallo en execv");
            exit(EXIT_FAILURE);
            
        } else if (strcmp(argv[argc - 1], "-vp") == 0){
            
            argumentoshijo = (char **)malloc(sizeof(2 * sizeof(char *)));
            argumentoshijo[0] = (char *)malloc((strlen(argv[1]) + 1) * sizeof(char));
            strcpy(argumentoshijo[0], argv[1]);
            argumentoshijo[1] = NULL;
            
            execvp(argv[1], argumentoshijo);
            
            perror("fallo en execvp");
            exit(EXIT_FAILURE);
            
        }
        
    } else {   /* Es un hijo */
    
        argumentospadre = (char **) malloc ((argc) * sizeof(char *));
        if (argumentospadre == NULL) {
            exit (EXIT_FAILURE);
        }
        
    
        argumentospadre[0] = (char *)malloc((strlen(argv[0]) + 1) * sizeof(char));
        if (argumentospadre[0] == NULL) {
            exit (EXIT_FAILURE);
        }
        
        strcpy(argumentospadre[0], argv[0]);
        if (argumentospadre[0] == NULL) {
            exit (EXIT_FAILURE);
        }
    
        for (int i = 1; i < (argc - 1) ; i++){
            argumentospadre[i] = (char *)malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
            if (argumentospadre[i] == NULL){
                exit (EXIT_FAILURE);
            }
            
            strcpy(argumentospadre[i], argv[i + 1]);
            if (argumentospadre[i] == NULL){
                exit (EXIT_FAILURE);
            }
        }
        argumentospadre[argc - 1] = NULL;
        
    
        execvp(argv[0], argumentospadre);
        
        perror("Fallo al volver a ejecutar el padre");
        exit(EXIT_FAILURE);
    
    }
    
    exit(EXIT_SUCCESS);
}