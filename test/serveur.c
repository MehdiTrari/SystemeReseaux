#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

void *gerer_client(void *client_socket) {
    int la_socket = *(int*)client_socket;
    free(client_socket);

    char buffer[1024];
    int n;

    // Boucle pour traiter plusieurs requêtes du même client
    while(1) {
        n = read(la_socket, buffer, 1023);
        if (n <= 0) {
            // Si la lecture échoue ou si le client ferme la connexion
            break;
        }

        buffer[n] = '\0';

        // Vérifier un signal de fin de connexion, par exemple "quit"
        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        // Analyser la requête
        char filename[256];
        int start_line, end_line;
        sscanf(buffer, "%255s %d %d", filename, &start_line, &end_line);

        // Ouvrir et traiter le fichier
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            write(la_socket, "Erreur lors de l'ouverture du fichier", 36);
        } else {
            int current_line = 1;
            int ch;
            size_t index = 0;
            while ((ch = fgetc(file)) != EOF) {
                // Ajouter le caractère lu au buffer s'il ne s'agit pas d'un saut de ligne
                if (ch != '\n') {
                    buffer[index++] = ch;
                }

                // Vérifier si nous avons atteint la fin d'une ligne ou du fichier
                if (ch == '\n' || index == sizeof(buffer) - 1 || feof(file)) {
                    buffer[index] = '\0'; // Terminer la chaîne
                    if (current_line >= start_line && current_line <= end_line) {
                        write(la_socket, buffer, index);
                        write(la_socket, "\n", 1); // S'assurer que la fin de la ligne est transmise
                    }
                    index = 0; // Réinitialiser l'index pour la prochaine ligne
                    if (ch == '\n') {
                        current_line++;
                    }
                }
            }

        // Fermer le fichier après avoir traité la requête
        if (file != NULL) {
            fclose(file);
        }
    }
    }
    close(la_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;

    // Creation socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse serv
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(61234);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Lier la socket
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(server_socket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1) {
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        if(client_socket < 0) {
            perror("accept");
            continue;
        }

        pthread_t thread;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;
        if(pthread_create(&thread, NULL, gerer_client, pclient) != 0) {
            perror("pthread_create");
            continue;
        }
        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}