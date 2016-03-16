# Makefile creado por Javier Rodriguez
# Asignatura: Redes2
# Grupo:
# Practica: Canal IRC
#-Wno-unused-but-set-variable
CC = gcc
CFLAGS = -Wall  -o
CFILES = server.c
EJECUTABLE = server
NOMBREZIP =

all:
	
	$(CC) $(CFLAGS) $(EJECUTABLE) $(CFILES) -lircredes -lirctad -lsoundredes -lpthread

clean:
	rm -f *.o 

zip:
	zip ../$(NOMBREZIP) *
