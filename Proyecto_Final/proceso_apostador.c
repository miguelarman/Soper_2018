#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NAME 20 /*!< Número maximo de caracteres para el nombre de los apostadores*/
#define KEY 1300 /*!< KEY necesaria para el ftok */
#define FILEKEY "/bin/bash" /*!< FILEKEY necesario para ftok */

typedef struct {
    char nombre[MAX_NAME];
    int numero_caballo;
    double cuantia;
    int mtype;
}Mensaje_Apostador;

int aleat_num(int inf, int sup);

int main(int argc, char **argv){
    Mensaje_Apostador *msg = NULL;
    if (argc < 4){
        perror("No suficientes parámetros");
        exit(EXIT_FAILURE);
    }
    int n_apostadores = atoi(argv[1]);
    int apuesta_maxima = atoi(argv[2]);
    int n_caballos = atoi(argv[3]);
    char nombre_apostador[MAX_NAME];
    int caballo_aleat;
    int apuesta_aleat;
    int apostador_aleat;
    int i = 0;
    
    for (i = 0; i < n_apostadores; i++){
        apostador_aleat = aleat_num(1, n_apostadores);
        sprintf(nombre_apostador, "Apostador_%d", i);
        caballo_aleat = aleat_num(1, n_caballos);
        apuesta_aleat = aleat_num(0, apuesta_maxima);
        
        msg->nombre = nombre_apostador;
        msg.numero_caballo = caballo_aleat;
        msg.cuantia = apuesta_aleat;
        
        /*
         //TODO: gestion de la memoria
         // Calcula la key para la memoria compartida y los semaforos 
        key = ftok(FILEKEY, KEY);
        if (key == -1) {
            perror("Error al usar ftok");
            //liberamos memoria y mas cosas ***************************************
            exit(EXIT_FAILURE);
        }
         
         
             
        //Crea la memoria compartida 
        shmid = reservashm(sizeof(Mensaje_Apostador*), key);
        if (shmid == -1) {
            perror("Error al crear la memoria compartida");
            exit(EXIT_FAILURE);
        }
        
        
        //Consigue la memoria compartida
        memoria_compartida = shmat (shmid, (char *)0, 0);
        if (datos == (void *) -1) {
            perror("Error al conseguir la memoria compartida en el proceso apostador");
            exit(EXIT_FAILURE);
        }
        
        
        */
        printf("%s\nCaballo: %d\nCuantía: %d\n", msg->nombre, msg->numero_caballo, msg->cuantia);
        
        sleep(1);
    }
    
    exit(EXIT_SUCCESS);
}

int aleat_num(int inf, int sup) {
  
  int dif;
  
  if (inf > sup) return inf - 1;
  
  dif = sup - inf;
  
  return inf + (int)((dif + 1) * (rand() / (RAND_MAX + 1.0)));

}