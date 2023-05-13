CC = gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lpthread

# los ejecutables
all: server
.PHONY: all


server: proxy.c server.c list_utils.c lines.c
	$(CC) $(CFLAGS) -g $^ -o $@




#para limpiar los ficheros generados
clean:
	rm -f server

.PHONY: clean


# -g para que genere informacion para depuracion
# -p para que no de error si ya existe
# -f para que no pregunte si se quiere sobreescribir
# -r para que borre directorios no vacios
# -c para que genere archivos de objeto .o 
# -o para indicar el nombre del fichero que se va a generar
# $^ es una variable que contiene todos los ficheros que dependen de la receta
# $< es una variable que contiene el primer fichero que depende de la receta
# $@ es una variable que contiene el nombre del fichero que se esta generando