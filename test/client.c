#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
    // variables
    int la_socket;
    struct sockaddr_in mon_adresse_serveur;
    char request[1024];
    char response[1024];
    char filename[256];
    char start_line[10];
    char end_line[10];
    int start, end;

    // créer la socket
    la_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (la_socket < 0) {
        perror("erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    // configurer l'adresse serveur
    memset(&mon_adresse_serveur, 0, sizeof(mon_adresse_serveur));
    mon_adresse_serveur.sin_family = AF_INET;
    mon_adresse_serveur.sin_port = htons(61234);
    mon_adresse_serveur.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // connexion au serveur
    if (connect(la_socket, (struct sockaddr*)&mon_adresse_serveur, sizeof(mon_adresse_serveur)) < 0) {
        perror("problème de connexion socket");
        exit(EXIT_FAILURE);
    }

    // Boucle pour des requêtes supplémentaires
    while (1) {
        printf("Entrez nom_fichier ligne_debut ligne_fin (ou 'quit' pour quitter): ");
        if (scanf("%255s %9s %9s", filename, start_line, end_line) != 3) {
            printf("Saisie incorrecte. Réessayer.\n");
            continue; // Continue pour une nouvelle saisie
        }

        // Convertir start_line et end_line en entiers
        start = atoi(start_line);
        end = atoi(end_line);

        sprintf(request, "%s %d %d", filename, start, end);
        write(la_socket, request, strlen(request) + 1);

        // Après avoir envoyé la commande "quit"
        if (strcmp(filename, "quit") == 0) {
           char server_response[1024];
            memset(server_response, 0, sizeof(server_response));
            read(la_socket, server_response, sizeof(server_response) - 1);
            printf("Réponse du serveur: %s\n", server_response);
            break; // Quitter la boucle
        }


        // Réinitialisation du buffer de réponse
        memset(response, 0, sizeof(response)); 

        // Lecture de la réponse du serveur
        while (1) {
            ssize_t nb_octets_lus = read(la_socket, response, sizeof(response) - 1);
            if (nb_octets_lus <= 0) {
                break; // Sortie de la boucle si erreur ou fin de données
            }

            response[nb_octets_lus] = '\0'; // Terminer correctement la chaîne de caractères

            if (strstr(response, "FIN_REPONSE")) {
                break; // Fin de la réponse du serveur
            }

            printf("%s", response); // Afficher la réponse

            memset(response, 0, sizeof(response)); // Réinitialiser pour la prochaine lecture
        }
    }

    close(la_socket);
    return 0;
}
