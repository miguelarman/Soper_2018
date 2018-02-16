#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


#define LECTURA 0
#define ESCRITURA 1

int main () {
    
    int fd[2], pipe_status;
    pid_t childpid;

    
    
    pipe_status = pipe(fd);
    if (pipe_status == -1) {
        
        perror(“Error creando la tuberia\n”);
        exit(EXIT_FAILURE);
    }
    
    
    
    if (childpid = fork()) {
        
        if (childpid == -1) {
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        }
        
        if (childpid = fork()) {
            
            if (childpid == -1) {
                perror("Error en el fork");
                exit(EXIT_FAILURE);
            }
            
            if (childpid = fork()) {
                
                if (childpid == -1) {
                    perror("Error en el fork");
                    exit(EXIT_FAILURE);
                }
                
                if (childpid = fork()) {
                    
                    if (childpid == -1) {
                        perror("Error en el fork");
                        exit(EXIT_FAILURE);
                    }
            
                    /*el padre*/
                    
                } else {
                    
                    /*Cuarto hijo*/
                    
                }
                
            } else {
                
                /*Tercer hijo*/
                
            }
            
        } else {
            
            /*Segundo hijo*/
            
        }
        
    } else {
        
        /*Primer hijo*/
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    exit (EXIT_SUCCESS);
}