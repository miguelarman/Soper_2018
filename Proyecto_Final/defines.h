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
#define SENALCARRERAEMPEZADA SIGRTMIN+1
#define SENALCABALLOLEEPOSICION SIGRTMIN+2

/* Defines usados por los mensajes */
enum {
    MENSAJE_CABALLO_A_PRINCIPAL = 1
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