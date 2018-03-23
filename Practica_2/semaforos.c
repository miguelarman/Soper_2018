#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "semaforos.h"


/***************************************************************
 * Nombre:
 * Inicializar_Semaforo.
 * Descripcion:
 * Inicializa los semaforos indicados.
 * Entrada:
 *    int semid: Identificador del semaforo.
 *    unsigned short *array: Valores iniciales.
 * Salida:
 *    int: OK si todo fue correcto, ERROR en caso de error.
************************************************************/
int Inicializar_Semaforo(int semid, unsigned short *array) {
    int size = 0;
    /*Me falta encontrar de alguna manera el size del array que me pasan*/
    semctl (sem_id, size, SETALL, array);
    return OK;
}


/***************************************************************
 * Nombre: Borrar_Semaforo.
 * Descripcion: Borra un semaforo.
 * Entrada: 
 *    int semid: Identificador del semaforo.
 * Salida:
 *    int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int Borrar_Semaforo(int semid) {
    /*Me falta aqui el N_SEMAFOROS*/
    semctl (sem_id, N_SEMAFOROS, IPC_RMID, 0);
    return OK;
}


/***************************************************************
 * Nombre: Crear_Semaforo.
 * Descripcion: Crea un semaforo con la clave y el tamaño especificado. Lo inicializa a 0.
 * Entrada:
 *    key_t key: Clave precompartida del semaforo.
 *    int size: Tamaño del semaforo.
 * Salida:
 *    int *semid: Identificador del semaforo.
 *    int: ERROR en caso de error,  0 si ha creado el semaforo, 1 si ya estaba creado.
**************************************************************/
int Crear_Semaforo(key_t key, int size, int *semid) {
    unsigned short *arg.array;
    sem_id = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    
    if((sem_id == -1) && (errno == EEXIST)){
        sem_id=semget(key,size,SHM_R|SHM_W);
    }
    
    if(sem_id == -1){
        perror("semget");
        exit(errno);
    }
    
    /********Inicialización************/
    arg.array = (unsigned short *)malloc(sizeof(short)*size);
    for (i = 0; i < size; i++){
        arg.array [i] = 0;
    }
    semctl (sem_id, size, SETALL, arg);
}


/**************************************************************
 * Nombre:Down_Semaforo
 * Descripcion: Baja el semaforo indicado
 * Entrada:
 *    int semid: Identificador del semaforo.
 *    int num_sem: Semaforo dentro del array.
 *    int undo: Flag de modo persistente pese a finalización abrupta.
 * Salida:
 *    int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int Down_Semaforo(int id, int num_sem, int undo) {
    
    struct sembuf sem_oper;
    
    sem_oper.sem_op = -1; /* Disminuir en 1 el valor del semáforo */
    sem_oper.sem_flg = undo;
    sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo num_sem de la lista */
    
    retorno = semop (id, &sem_oper, 1);
    
    if (retorno != 0) {
        return ERROR;
    }
    
    return OK;
    
}


/***************************************************************
 * Nombre: DownMultiple_Semaforo
 * Descripcion: Baja todos los semaforos del array indicado por active.
 * Entrada:
 *    int semid: Identificador del semaforo.
 *    int size: Numero de semaforos del array.
 *    int undo: Flag de modo persistente pese a finalización abrupta.
 *    int *active: Semaforos involucrados.
 * Salida:
 *    int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int DownMultiple_Semaforo(int id,int size,int undo,int *active) {
    
    int i;
    int retorno;
    
    /*cantidad_de_semaforos creo que es size*/
    // for (i = 0; i < cantidad_de_semaforos; i++) {
    for (i = 0; i < size; i++) {
        retorno = Down_Semaforo(id, active[i], undo);
        
        if (retorno == ERROR) {
            return ERROR;
        }
    }
    
    return OK;
}





/**************************************************************
 * Nombre: Up_Semaforo
 * Descripcion: Sube el semaforo indicado
 * Entrada:
 *    int semid: Identificador del semaforo.
 *    int num_sem: Semaforo dentro del array.
 *    int undo: Flag de modo persistente pese a finalizacion abupta.
 * Salida:
 *    int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int Up_Semaforo(int id, int num_sem, int undo) {
    
    struct sembuf sem_oper;
    
    sem_oper.sem_op = 1; /* Aumentar en 1 el valor del semáforo */
    sem_oper.sem_flg = undo;
    sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo num_sem de la lista */
    
    retorno = semop (id, &sem_oper, 1);
    
    if (retorno != 0) {
        return ERROR;
    }
    
    return OK;
}
    

/***************************************************************
 * Nombre: UpMultiple_Semaforo
 * Descripcion: Sube todos los semaforos del array indicado por active.
 * Entrada:
 *    int semid: Identificador del semaforo.
 *    int size: Numero de semaforos del array.
 *    int undo: Flag de modo persistente pese a finalización abrupta.
 *    int *active: Semaforos involucrados.
 * Salida:
 *    int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int UpMultiple_Semaforo(int id, int size, int undo, int *active) {
    
    int i;
    int retorno;
    while(active[i]=!NULL){
        i++;
        
    }
    
    /*cantidad_de_semaforos creo que es size*/
    // for (i = 0; i < cantidad_de_semaforos; i++) {
    for (i = 0; i < size; i++) {
        retorno = Up_Semaforo(id, active[i], undo);
        
        if (retorno == ERROR) {
            return ERROR;
        }
    }
    
    return OK;
}