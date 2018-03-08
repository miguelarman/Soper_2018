########################################################
# File: Makefile
#
# Authors: Miguel Arconada Manteca and José Manuel Chacón Aguilera
########################################################
CC = gcc
CFLAGS = -g -Wall -pedantic -ansi
FILES = ejercicio4a ejercicio4b ejercicio5a ejercicio5b ejercicio6 ejercicio8 ejercicio9 ejercicio12a ejercicio12b ejercicio13
# ejercicio11          --> Programas de aprendizaje, que también han necesitado
#                          ser compilados
########################################################

all: $(FILES) clear

ejercicio4a:
	$(CC) $(CFLAGS) ejercicio4a.c -o ejercicio4a 

ejercicio4b:
	$(CC) $(CFLAGS) ejercicio4b.c -o ejercicio4b
	
ejercicio5a:
	$(CC) $(CFLAGS) ejercicio5a.c -o ejercicio5a
	
ejercicio5b:
	$(CC) $(CFLAGS) ejercicio5b.c -o ejercicio5b
	
ejercicio6:
	$(CC) $(CFLAGS) ejercicio6.c -o ejercicio6
	
ejercicio8:
	$(CC) $(CFLAGS) ejercicio8.c -o ejercicio8
	
ejercicio9:
	$(CC) $(CFLAGS) ejercicio9.c -o ejercicio9 -lm
	
ejercicio11:
	$(CC) $(CFLAGS) ejercicio11.c -o ejercicio11 -lpthread
	
ejercicio12a:
	$(CC) $(CFLAGS) ejercicio12a.c -o ejercicio12a -lm
	
ejercicio12b:
	$(CC) $(CFLAGS) ejercicio12b.c -o ejercicio12b -lpthread -lm
	
ejercicio13:
	$(CC) $(CFLAGS) ejercicio13.c -o ejercicio13 -lpthread
	





clear:
	rm -rf *.o 

clean:
	rm -rf *.o $(FILES)





# EJECUCIONES DE PROGRAMAS FIJADAS:

# Ejecuciones de testing del ejercicio 4a
ejercicio4a_test:
	@echo Ejecutando ejercicio4a
	@./ejercicio4a

ejercicio4a_test_val:
	@echo Ejecutando ejercicio4a con Valgrind
	@valgrind --leak-check=full ./ejercicio4a
	
	
# Ejecuciones de testing del ejercicio 4b
ejercicio4b_test:
	@echo Ejecutando ejercicio4b
	@./ejercicio4b

ejercicio4b_test_val:
	@echo Ejecutando ejercicio4b con Valgrind
	@valgrind --leak-check=full ./ejercicio4b
	
	
# Ejecuciones de testing del ejercicio 5a
ejercicio5a_test:
	@echo Ejecutando ejercicio5a
	@./ejercicio5a

ejercicio5a_test_val:
	@echo Ejecutando ejercicio5a con Valgrind
	@valgrind --leak-check=full ./ejercicio5a
	
	
# Ejecuciones de testing del ejercicio 5b
ejercicio5b_test:
	@echo Ejecutando ejercicio5b
	@./ejercicio5b

ejercicio5b_test_val:
	@echo Ejecutando ejercicio5b con Valgrind
	@valgrind --leak-check=full ./ejercicio5b
	
	
# Ejecuciones de testing del ejercicio 6	
ejercicio6_test:
	@echo Ejecutando ejercicio6
	@./ejercicio6

ejercicio6_test_val:
	@echo Ejecutando ejercicio6 con Valgrind
	@valgrind --leak-check=full ./ejercicio6
	
	
# Ejecuciones de testing del ejercicio 8	
ejercicio8_test:
	@echo Ejecutando ejercicio8
	@./ejercicio8

ejercicio8_test_val:
	@echo Ejecutando ejercicio8 con Valgrind
	@valgrind --leak-check=full ./ejercicio8
	
	
# Ejecuciones de testing del ejercicio 9
ejercicio9_test:
	@echo Ejecutando ejercicio9
	@./ejercicio9

ejercicio9_test_val:
	@echo Ejecutando ejercicio9 con Valgrind
	@valgrind --leak-check=full ./ejercicio9
	
	
# Ejecuciones de testing del ejercicio 11
ejercicio11_test:
	@echo Ejecutando ejercicio11
	@./ejercicio11

ejercicio11_test_val:
	@echo Ejecutando ejercicio11 con Valgrind
	@valgrind --leak-check=full ./ejercicio11
	
	
# Ejecuciones de testing del ejercicio 12a
ejercicio12a_test:
	@echo Ejecutando ejercicio12a
	@./ejercicio12a

ejercicio12a_test_val:
	@echo Ejecutando ejercicio12a con Valgrind
	@valgrind --leak-check=full ./ejercicio12a
	
	
# Ejecuciones de testing del ejercicio 12b	
ejercicio12b_test:
	@echo Ejecutando ejercicio12b
	@./ejercicio12b

ejercicio12b_test_val:
	@echo Ejecutando ejercicio12b con Valgrind
	@valgrind --leak-check=full ./ejercicio12b
	
	
# Ejecuciones de testing del ejercicio 13
ejercicio13_test:
	@echo Ejecutando ejercicio13
	@./ejercicio13

ejercicio13_test_val:
	@echo Ejecutando ejercicio13 con Valgrind
	@valgrind --leak-check=full ./ejercicio13
