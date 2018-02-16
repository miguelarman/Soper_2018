#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int main(void) {
    int fd[2], nbytes, pipe_status;
    pid_t childpid;
    char string[] = "Hola a todos!\n";
    char readbuffer[80];
    
    
    pipe_status=pipe(fd);
    if(pipe_status=-1) {
        perror(“Error creando la tuberia\n”);
        exit(EXIT_FAILURE);
        
    }
    
    if((childpid = fork()) == -1){
        perror("fork");
        exit(EXIT_FAILURE);
        
    }
    
    
    if(childpid == 0){
        /* Cierre del descriptor de entrada en el hijo */
        close(fd[0]);
        
        /* Enviar el saludo vía descriptor de salida */
        write(fd[1], string, strlen(string));
        exit(0);
        
    } else {
        /* Cierre del descriptor de salida en el padre */
        close(fd[1]);
        
        /* Leer algo de la tubería... el saludo! */
        nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
        printf("He recibido el string: %s", readbuffer);
        
    }
    
    return(0);
}