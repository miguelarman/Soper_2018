########################################################
# File: Makefile
#
# Authors: Miguel Arconada Manteca and José Manuel Chacón Aguilera
########################################################
CC = gcc
CFLAGS = -g -Wall -pedantic
FILES = ejercicio2 ejercicio2_solved ejercicio3 ejercicio4 cadena_montaje
#           --> Programas de aprendizaje, que también han necesitado
#                          ser compilados
########################################################
OBJECTSEJERCICIO2 = aleat_num.o ejercicio2.o
OBJECTSEJERCICIO2_SOLVED = semaforos.o aleat_num.o ejercicio2_solved.o
OBJECTSEJERCICIO3 = semaforos.o aleat_num.o ejercicio3.o
OBJECTSEJERCICIO4 = aleat_num.o ejercicio4.o
OBJECTSCADENA_MONTAJE = aleat_num.o cadena_montaje.o
########################################################

all: $(FILES) clear


ejercicio2: $(OBJECTSEJERCICIO2)
	$(CC) $(CFLAGS) -o ejercicio2 $(OBJECTSEJERCICIO2) -lpthread

ejercicio2.o: aleat_num.h ejercicio2.c
	$(CC) $(CFLAGS) -c ejercicio2.c
	
	
ejercicio2_solved: $(OBJECTSEJERCICIO2_SOLVED)
	$(CC) $(CFLAGS) -o ejercicio2_solved $(OBJECTSEJERCICIO2_SOLVED) -lpthread

ejercicio2_solved.o: semaforos.h aleat_num.h ejercicio2_solved.c
	$(CC) $(CFLAGS) -c ejercicio2_solved.c


ejercicio3: $(OBJECTSEJERCICIO3)
	$(CC) $(CFLAGS) -o ejercicio3 $(OBJECTSEJERCICIO3) -lpthread

ejercicio3.o: semaforos.h aleat_num.h ejercicio3.c
	$(CC) $(CFLAGS) -c ejercicio3.c



ejercicio4: $(OBJECTSEJERCICIO4)
	$(CC) $(CFLAGS) -o ejercicio4 $(OBJECTSEJERCICIO4) -lpthread

ejercicio4.o: aleat_num.h ejercicio4.c
	$(CC) $(CFLAGS) -c ejercicio4.c
	

cadena_montaje: $(OBJECTSCADENA_MONTAJE)
	$(CC) $(CFLAGS) -o cadena_montaje $(OBJECTSCADENA_MONTAJE) -lpthread

cadena_montaje.o: aleat_num.h cadena_montaje.c
	$(CC) $(CFLAGS) -c cadena_montaje.c
	


semaforos.o: semaforos.c semaforos.h
	$(CC) $(CFLAGS) -c semaforos.c

aleat_num.o: aleat_num.c aleat_num.h
	$(CC) $(CFLAGS) -c aleat_num.c






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



# Ejecuciones de testing del ejercicio 3
ejercicio3_test:
	@echo Ejecutando ejercicio3
	@./ejercicio3

ejercicio3_test_val:
	@echo Ejecutando ejercicio3 con Valgrind
	@valgrind --leak-check=full ./ejercicio3



# Ejecuciones de testing del ejercicio 4
ejercicio4_test:
	@echo Ejecutando ejercicio4
	@./ejercicio4

ejercicio4_test_val:
	@echo Ejecutando ejercicio4 con Valgrind
	@valgrind --leak-check=full ./ejercicio4


# Ejecuciones de testing del ejercicio cadena de montaje
cadena_montaje_test:
	@echo Ejecutando cadena_montaje
	@rm files/cadena_montaje/*.txt
	@./cadena_montaje "files/cadena_montaje/entrada.txt" "files/cadena_montaje/salida.txt"

cadena_montaje_test_val:
	@echo Ejecutando cadena_montaje con Valgrind
	@rm files/cadena_montaje/*.txt
	@valgrind --leak-check=full ./cadena_montaje "files/cadena_montaje/entrada.txt" "files/cadena_montaje/salida.txt"


# Ejecucion de todos los ejercicios
run:
	@echo ">>>>>>Ejecutando ejercicio2"
	./ejercicio2 2
	@echo ">>>>>>Ejecutando ejercicio2_solved"
	./ejercicio2_solved 2
	@echo ">>>>>>Ejecutando ejercicio3"
	./ejercicio3
	@echo ">>>>>>Ejecutando ejercicio4"
	./ejercicio4
	@echo ">>>>>>Ejecutando cadena_montaje"
	@rm files/cadena_montaje/*.txt
	./cadena_montaje "files/cadena_montaje/entrada.txt" "files/cadena_montaje/salida.txt"


runv:
	@echo ">>>>>>Ejecutando ejercicio2 con valgrind"
	valgrind --leak-check=full ./ejercicio2 2
	@echo ">>>>>>Ejecutando ejercicio2_solved con valgrind"
	valgrind --leak-check=full ./ejercicio2_solved 2
	@echo ">>>>>>Ejecutando ejercicio3 con valgrind"
	valgrind --leak-check=full ./ejercicio3
	@echo ">>>>>>Ejecutando ejercicio4 con valgrind"
	valgrind --leak-check=full ./ejercicio4
	@echo ">>>>>>Ejecutando cadena_montaje con valgrind"
	@rm files/cadena_montaje/*.txt
	valgrind --leak-check=full ./cadena_montaje "files/cadena_montaje/entrada.txt" "files/cadena_montaje/salida.txt"