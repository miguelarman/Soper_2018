#include "defines.h"

typedef struct {
    char nombre[MAX_NAME]; /*!< Nombre del apostador */
    int numero_caballo; /*!< Número del caballo */
    double cuantia; /*!< Cantidad apostada */
    int mtype; /*!< Dato necesario para la cola de mensajes */
} Mensaje_Apostador;

typedef struct {
    int mtype;
    int tirada;
    int id_caballo;
} Mensaje_Tirada_Caballo;

typedef struct {
    int posicion;
    int ultima_tirada;
    double total_apostado;
    double cotizacion;
} Datos_Caballo;

typedef struct {
    char nombre[MAX_NAME]; /*!< Nombre del apostador */
    int numero_caballo; /*!< Número del caballo */
    double cuantia; /*!< Cantidad apostada */
    int ventanilla;
    double cotizacion_previa;
} Apuesta;

typedef struct {
    char nombre[MAX_NAME];
    double cantidad_apostada;
    double beneficios;
    double dinero_restante;
} Datos_Apostador;

typedef struct {
    int segundos_restantes; /*!< Segundos restantes para que empiece la carrera */
    int n_caballos; /*!< Número de caballos en el sistema*/
    int n_apostadores; /*!< Número de apostadores en el sistema*/
    int n_apuestas; /*!< Número de apuestas registradas hasta el momento */
    Datos_Apostador apostadores[MAX_APOSTADORES]; /*!< Array que apunta a los distintos apostadores*/
    Datos_Caballo caballos[MAX_CABALLOS]; /*!< Array que apunta a los distintos caballos*/
    Apuesta historial_apuestas[MAX_APUESTAS]; /*!< Lista de apuestas leidas en las ventanillas */
    int top_apostadores[10]; /*!< Lista de los índices de los apostadores con más beneficios */
} Memoria_Compartida;