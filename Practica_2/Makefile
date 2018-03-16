########################################################
# File: Makefile
#
# Authors: Miguel Arconada Manteca and José Manuel Chacón Aguilera
########################################################
CC = gcc
CFLAGS = -g -Wall -pedantic
FILES = ejercicio2 ejercicio4
# ---          --> Programas de aprendizaje, que también han necesitado
#                          ser compilados
########################################################

all: $(FILES) clear


ejercicio2:
	$(CC) $(CFLAGS) ejercicio2.c -o ejercicio2

ejercicio4:
	$(CC) $(CFLAGS) ejercicio4.c -o ejercicio4
	





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

	
# Ejecuciones de testing del ejercicio 4
ejercicio4_test:
	@echo Ejecutando ejercicio4
	@./ejercicio4

ejercicio4_test_val:
	@echo Ejecutando ejercicio4 con Valgrind
	@valgrind --leak-check=full ./ejercicio4
	
