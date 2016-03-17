# Makefile creado por Javier Rodriguez
# Asignatura: Redes2
# Grupo:
# Practica: Canal IRC
#-Wno-unused-but-set-variable
CC = gcc
CFLAGS = -Wall  -o
CFILES = src/server.c
EJECUTABLE = server
NOMBREZIP = G-2361-01-P1

all:
	
	$(CC) $(CFLAGS) $(EJECUTABLE) $(CFILES) -lircredes -lirctad -lsoundredes -lpthread

clean:
	rm -f *.o 

comprimir:
	tar -czvf ../$(NOMBREZIP) *

g_autores:
	echo "G - CCCC - NN - P1" >> ../autores.txt
	echo "266811#Arribas Jara, Fernando" >> ../autores.txt
	echo "282917#Rodríguez Inés, Javier" >> ../autores.txt
empaquetar:
	make comprimir
	make g_autores
