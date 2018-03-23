########################################################
# File: Makefile
#
# Authors: Miguel Arconada Manteca and José Manuel Chacón Aguilera
########################################################
CC = gcc
CFLAGS = -g -Wall -pedantic
FILES = ejercicio2 ejercicio4 ejercicio6a ejercicio6b
# ejercicio7          --> Programas de aprendizaje, que también han necesitado
#                          ser compilados
########################################################

all: $(FILES) clear


ejercicio2:
	$(CC) $(CFLAGS) ejercicio2.c -o ejercicio2

ejercicio4:
	$(CC) $(CFLAGS) ejercicio4.c -o ejercicio4
	
ejercicio6a:
	$(CC) $(CFLAGS) ejercicio6a.c -o ejercicio6a

ejercicio6b:
	$(CC) $(CFLAGS) ejercicio6b.c -o ejercicio6b
	





# Ejercicios de aprendizaje

ejercicio7:
	$(CC) $(CFLAGS) ejercicio7.c -o ejercicio7


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
	

# Ejecuciones de testing del ejercicio 6a
ejercicio6a_test:
	@echo Ejecutando ejercicio6a
	@./ejercicio6a

ejercicio6a_test_val:
	@echo Ejecutando ejercicio6a con Valgrind
	@valgrind --leak-check=full ./ejercicio6a


# Ejecuciones de testing del ejercicio 6b
ejercicio6b_test:
	@echo Ejecutando ejercicio6b
	@./ejercicio6b

ejercicio6b_test_val:
	@echo Ejecutando ejercicio6b con Valgrind
	@valgrind --leak-check=full ./ejercicio6b