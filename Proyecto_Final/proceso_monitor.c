#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "memoria_compartida.h"
#include "estructuras.h"

int estado_carrera = SIN_EMPEZAR;

int main (int argc, char **argv) {

    while(1) {
        if (estado_carrera == SIN_EMPEZAR) {
            /* TODO */
            /* Antes de la carrera:
            o Segundos que faltan para que comience
            o Estado de las apuestas
             Cotización de cada caballo */
        } else if (estado_carrera == EMPEZADA) {
            /* TODO */
            /* Durante la carrera:
            o Posición de los caballos
            o Por cada caballo, su última tirada */
        } else if (estado_carrera == TERMINADA) {
            /* TODO */
            /* Finalizada la carrera:
            o Resultado de la carrera
            o Resultados de las apuestas:
             Listado ordenado de los 10 apostadores con más beneficios.
            
            
            Además, este proceso generará, al ser notificado de su finalización, un report
            completo de la carrera por pantalla. El cual incluirá:
            - Listado de apuestas realizadas. Una línea por apuesta en la que se
            indicará: el apostador, ventanilla que gestiona la apuesta, el caballo, la
            cotización del caballo (justo antes de la apuesta) y la cantidad apostada.
            Este listado debe preservar el orden en el que se registraron las
            apuestas en el sistema.
            - Resultado de la carrera. Posición de finalización de los caballos.
            - Resultados de las apuestas. Listado de apostadores en el que se incluye:
            nombre del apostador, cantidad apostada, beneficios obtenidos y dinero
            que le queda */
        }
    }
    
        
    exit(EXIT_SUCCESS);
}