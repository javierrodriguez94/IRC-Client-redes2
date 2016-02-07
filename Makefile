# Makefile creado por Javier Rodriguez
# Asignatura: Redes2
# Grupo:
# Practica: Canal IRC

CC = gcc
CFLAGS = -Wall -o
CFILES = 
NOMBREZIP =

all:
	
	$(CC) $(CFLAGS) $(CFILES)

clean:
	rm -f *.o 

zip:
	zip ../$(NOMBREZIP) *
