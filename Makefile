# Makefile creado por Javier Rodriguez
# Asignatura: Redes2
# Grupo:
# Practica: Canal IRC

CC = gcc
CFLAGS = -Wall -o
CFILES = server.c
EJECUTABLE = server
NOMBREZIP =

all:
	
	$(CC) $(CFLAGS) $(EJECUTABLE) $(CFILES)

clean:
	rm -f *.o 

zip:
	zip ../$(NOMBREZIP) *
