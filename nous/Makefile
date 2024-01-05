CC = gcc
CFLAGS = -Wall -g

all: client serveur

client: client.o
	$(CC) $(CFLAGS) -o client client.o

serveur: serveur.o
	$(CC) $(CFLAGS) -o serveur serveur.o -pthread

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

serveur.o: serveur.c
	$(CC) $(CFLAGS) -c serveur.c

clean:
	rm -f *.o client serveur
