#define MAX_NAME 20
#define MAX_APOSTADORES 100 /*!< Apostadores Máximos que gestiona el sistema */
#define MAX_CABALLOS 10 /*!< Caballos Máximos que participan en la carrera */

typedef struct {
    char[MAX_NAME] nombre; /*!< Nombre del apostador */
    int numero_caballo; /*!< Número del caballo */
    double cuantia; /*!< Cantidad apostada */
    int mtype; /*!< Dato necesario para la cola de mensajes */
} Mensaje_Apostador;

typedef struct {
    int mtype;
    int tirada;
} Mensaje_Tirada_Caballo;

typedef struct {
    
} Datos_Apostador;

typedef struct {
    
} Datos_Caballo;

typedef struct {
    int segundos_restantes; /*!< Segundos restantes para que empiece la carrera */
    int n_caballos; /*!< Número de caballos en el sistema*/
    int n_apostadores; /*!< Número de apoostadores en el sistema*/
    
    Datos_Apostador apostadores[MAX_APOSTADORES]; /*!< Array que apunta a los distintos apostadores*/
    Datos_Caballo caballos[MAX_CABALLOS]; /*!< Array que apunta a los distintos caballos*/
} Memoria_Compartida;