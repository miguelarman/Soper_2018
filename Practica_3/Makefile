########################################################
# File: Makefile
#
# Authors: Miguel Arconada Manteca and José Manuel Chacón Aguilera
########################################################
CC = gcc
CFLAGS = -g -Wall -pedantic
FILES = ejercicio2
#           --> Programas de aprendizaje, que también han necesitado
#                          ser compilados
########################################################
OBJECTSEJERCICIO2 = semaforos.o aleat_num.o ejercicio2.o
########################################################

all: $(FILES) clear


ejercicio2: $(OBJECTSEJERCICIO2)
	$(CC) $(CFLAGS) -o ejercicio2 $(OBJECTSEJERCICIO2) -lpthread

ejercicio2.o: semaforos.h ejercicio2.c
	$(CC) $(CFLAGS) -c ejercicio2.c
	


semaforos.o: semaforos.c semaforos.h
	$(CC) $(CFLAGS) -c semaforos.c

aleat_num.o: aleat_num.c aleat_num.h
	$(CC) $(CFLAGS) -c aleat_num.c


# Ejercicios de aprendizaje

#ejercicio7:
#	$(CC) $(CFLAGS) ejercicio7.c -o ejercicio7


clear:
	rm -rf *.o 

clean:
	rm -rf *.o $(FILES)





# EJECUCIONES DE PROGRAMAS FIJADAS:

# Ejecuciones de testing del ejercicio 2
ejercicio2_test:
	@echo Ejecutando ejercicio2
	@./ejercicio2

ejercicio2_test_val:
	@echo Ejecutando ejercicio2 con Valgrind
	@valgrind --leak-check=full ./ejercicio2