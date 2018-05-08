########################################################
# File: Makefile
#
# Authors: Miguel Arconada Manteca and José Manuel Chacón Aguilera
########################################################
CC = gcc
CFLAGS = -g -Wall -pedantic
FILES = proceso_principal proceso_monitor proceso_gestor_apuestas proceso_apostador
########################################################
OBJECTSPROCESOPRINCIPAL = aleat_num.o semaforos.o memoria_compartida.o proceso_principal.o
OBJECTSPROCESOAPOSTADOR = aleat_num.o semaforos.o memoria_compartida.o proceso_apostador.o
OBJECTSPROCESOGESTORAPUESTAS = aleat_num.o semaforos.o memoria_compartida.o proceso_gestor_apuestas.o
OBJECTSPROCESOMONITOR = aleat_num.o semaforos.o memoria_compartida.o proceso_monitor.o
########################################################

all: $(FILES) clear

proceso_principal: $(OBJECTSPROCESOPRINCIPAL)
	$(CC) $(CFLAGS) -o proceso_principal $(OBJECTSPROCESOPRINCIPAL) -lpthread

proceso_principal.o: aleat_num.h semaforos.h estructuras.h memoria_compartida.h proceso_principal.c
	$(CC) $(CFLAGS) -c proceso_principal.c



proceso_apostador: $(OBJECTSPROCESOAPOSTADOR)
	$(CC) $(CFLAGS) -o proceso_apostador $(OBJECTSPROCESOAPOSTADOR) -lpthread

proceso_apostador.o: aleat_num.h semaforos.h estructuras.h memoria_compartida.h proceso_apostador.c
	$(CC) $(CFLAGS) -c proceso_apostador.c



proceso_gestor_apuestas: $(OBJECTSPROCESOGESTORAPUESTAS)
	$(CC) $(CFLAGS) -o proceso_gestor_apuestas $(OBJECTSPROCESOGESTORAPUESTAS) -lpthread

proceso_gestor_apuestas.o: aleat_num.h semaforos.h estructuras.h memoria_compartida.h proceso_gestor_apuestas.c
	$(CC) $(CFLAGS) -c proceso_apostador.c



proceso_monitor: $(OBJECTSPROCESOMONITOR)
	$(CC) $(CFLAGS) -o proceso_monitor $(OBJECTSPROCESOMONITOR) -lpthread

proceso_monitor.o: aleat_num.h semaforos.h estructuras.h  memoria_compartida.h proceso_monitor.c
	$(CC) $(CFLAGS) -c proceso_monitor.c
	
	


semaforos.o: semaforos.c semaforos.h
	$(CC) $(CFLAGS) -c semaforos.c

aleat_num.o: aleat_num.c aleat_num.h
	$(CC) $(CFLAGS) -c aleat_num.c

memoria_compartida.o: memoria_compartida.c memoria_compartida.h
	$(CC) $(CFLAGS) -c memoria_compartida.c






clear:
	rm -rf *.o 

clean:
	rm -rf *.o $(FILES)

doxyfile:
	doxygen -g
	
doxygen:
	doxygen Doxyfile




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
