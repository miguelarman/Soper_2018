/**
 * @brief Ejercicio 3 de la Práctica
 * 
 * En este ejercicio es de aprendizaje
 * 
 * @file ejercicio3.c
 * @author José Manuel Chacón Aguilera y Miguel Arconada Manteca
 * @date 6-4-2018
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>



void captura (int sennal) {
    printf ("Capturada la señal %d \n", sennal);
    fflush (NULL);
    return;
}


int main (int argc, char *argv [], char *env []) {
    
    if (signal (SIGINT, captura) == SIG_ERR){
        puts ("Error en la captura");
        exit (1);
    }
    
    while (1);
    exit (0);
}