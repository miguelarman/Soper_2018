########################################################
# File: Makefile
#
# Authors: Miguel Arconada Manteca and José Manuel Chacón Aguilera
########################################################
CC = gcc
CFLAGS = -g -Wall -pedantic
FILES = ejercicio2 ejercicio2_solved ejercicio3
#           --> Programas de aprendizaje, que también han necesitado
#                          ser compilados
########################################################
OBJECTSEJERCICIO2 = semaforos.o aleat_num.o ejercicio2.o
OBJECTSEJERCICIO2_SOLVED = semaforos.o aleat_num.o ejercicio2_solved.o
OBJECTSEJERCICIO3 = semaforos.o aleat_num.o ejercicio3.o
########################################################

all: $(FILES) clear


ejercicio2: $(OBJECTSEJERCICIO2)
	$(CC) $(CFLAGS) -o ejercicio2 $(OBJECTSEJERCICIO2) -lpthread

ejercicio2.o: semaforos.h ejercicio2.c
	$(CC) $(CFLAGS) -c ejercicio2.c
	
	
ejercicio2_solved: $(OBJECTSEJERCICIO2_SOLVED)
	$(CC) $(CFLAGS) -o ejercicio2_solved $(OBJECTSEJERCICIO2_SOLVED) -lpthread

ejercicio2_solved.o: semaforos.h ejercicio2_solved.c
	$(CC) $(CFLAGS) -c ejercicio2_solved.c


ejercicio3: $(OBJECTSEJERCICIO3)
	$(CC) $(CFLAGS) -o ejercicio3 $(OBJECTSEJERCICIO3) -lpthread

ejercicio3.o: semaforos.h ejercicio3.c
	$(CC) $(CFLAGS) -c ejercicio3.c
	


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
	@./ejercicio2 2

ejercicio2_test_val:
	@echo Ejecutando ejercicio2 con Valgrind
	@valgrind --leak-check=full ./ejercicio2 2
	
	
# Ejecuciones de testing del ejercicio 2 solved
ejercicio2_solved_test:
	@echo Ejecutando ejercicio2_solved
	@./ejercicio2_solved 2

ejercicio2_solved_test_val:
	@echo Ejecutando ejercicio2_solved con Valgrind
	@valgrind --leak-check=full ./ejercicio2_solved 2
	


# Ejecucion de todos los ejercicios
run:
	@echo ">>>>>>Ejecutando ejercicio2"
	./ejercicio2 2
	@echo ">>>>>>"
	@echo "\n\n\n"
	@echo ">>>>>>Ejecutando ejercicio2_solved"
	./ejercicio2_solved 2


runv:
	@echo ">>>>>>Ejecutando ejercicio2"
	valgrind --leak-check=full ./ejercicio2 2
	@echo ">>>>>>Ejecutando ejercicio2_solved"
	valgrind --leak-check=full ./ejercicio2_solved 2