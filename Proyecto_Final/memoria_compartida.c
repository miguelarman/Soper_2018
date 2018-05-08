#include "memoria_compartida.h"


int reservashm(int size, int key) {
    int shmid_reserva;
    shmid_reserva = shmget (key, size, IPC_CREAT | IPC_EXCL |SHM_R | SHM_W);
    
    if(shmid_reserva == -1) {
        shmid_reserva = shmget(key, size, 0);
    }
    
    return shmid_reserva;
}