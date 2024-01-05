#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{

    // variables
    int la_socket;
    struct sockaddr_in mon_adresse_serveur;
    char le_buffer[1024];
    int nb_octets_lus;
    char request[1024];

    // créer la socket
    la_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (la_socket < 0)
    {
        perror("erreur création socket");
        exit(EXIT_FAILURE);
    }

    // configurer l'adresse serveur
    memset(&mon_adresse_serveur, 0, sizeof(mon_adresse_serveur));
    mon_adresse_serveur.sin_family = AF_INET;
    mon_adresse_serveur.sin_port = htons(61234);
    mon_adresse_serveur.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // connexion au serveur
    if (connect(la_socket, (struct sockaddr *)&mon_adresse_serveur, sizeof(mon_adresse_serveur)) < 0)
    {
        perror("problème de connexion socket");
        exit(EXIT_FAILURE);
    }

    // Boucle de requêtes
    while (1)
    {
        printf("Entrez le nom du fichier et les numéros de ligne (ou 'quit' pour sortir) : ");
        fgets(request, sizeof(request), stdin);

        // Vérifier si l'utilisateur veut quitter
        if (strncmp(request, "quit", 4) == 0)
        {
            break;
        }

        // Envoyer la requête
        write(la_socket, request, strlen(request) + 1);

        // Recevoir et afficher la réponse
        while ((nb_octets_lus = read(la_socket, le_buffer, 1023)) > 0)
        {
            le_buffer[nb_octets_lus] = '\0';
            printf("%s", le_buffer);
        }
    }

    close(la_socket);
    return 0;
}
