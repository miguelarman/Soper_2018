/* Defines para la comunicación por las pipes */
#define LECTURA 0
#define ESCRITURA 1
enum {
    PRIMERO,
    ULTIMO,
    MEDIO,
    CARRERAYATERMINADA
};

/* Defines usados para las señales entre procesos */
#define SENALTIEMPORESTANTE SIGRTMIN
#define SENALDATOSCARRERAACTUALIZADOS SIGRTMIN 
#define SENALESTADOCARRERACAMBIA SIGRTMIN+1
#define SENALCABALLOLEEPOSICION SIGRTMIN+2
#define SENALINTERRUPCIONUSUARIO SIGRTMIN+3


/* Defines usados por los mensajes */
enum {
    MENSAJE_CABALLO_A_PRINCIPAL = 1,
    MENSAJE_APOSTADOR_A_GESTOR = 2
};

/* Estado de la carrera para el proceso monitor */
enum {
    SIN_EMPEZAR,
    EMPEZADA,
    TERMINADA
};

/* Otros defines */
enum {
    TRUE,
    FALSE
};

#define MAX_VENTANILLAS 100
#define MAX_NAME 20
#define MAX_APOSTADORES 100 /*!< Apostadores Máximos que gestiona el sistema */
#define MAX_CABALLOS 10 /*!< Caballos Máximos que participan en la carrera */
#define MIN_LONGITUD 1
#define MAX_LONGITUD 1000
#define MIN_DINERO 10
#define MAX_DINERO 10000

#define MAX_APUESTAS 10000 /*!< Apuestas Máximas que el proceso gestor puede admitir */

/* Defines para los semaforos */
#define NUM_SEMAFOROS 2

#define MUTEX_GUARDAR_OFERTA 0
#define MUTEX_BENEFICIOS_CALCULADOS 1