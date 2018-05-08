#ifndef MEMORIA_COMPARTIDA_H
#define MEMORIA_COMPARTIDA_H

#include <sys/ipc.h>
#include <sys/shm.h>

int reservashm(int size, int key);

#endif