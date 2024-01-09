#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

volatile int server_running = 1;

void *gerer_client(void *client_socket) {
    int la_socket = *(int*)client_socket;
    free(client_socket);

    printf("Client connecté. En attente de requêtes...\n");

    char buffer[1024];
    int n;

    while (server_running) {
        printf("En attente d'une requête...\n");
        memset(buffer, 0, sizeof(buffer));

        n = read(la_socket, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            printf("Erreur de lecture ou client déconnecté.\n");
            break;
        }

        buffer[n] = '\0';

        // Dans la fonction gerer_client
        if (strcmp(buffer, "quit") == 0) {
            printf("Signal 'quit' reçu. Déconnexion du client.\n");
           write(la_socket, "QUIT_ACK", strlen("QUIT_ACK"));
            break;
        }


        printf("Requête reçue : %s\n", buffer);

        char filename[256];
        int start_line, end_line;
        sscanf(buffer, "%255s %d %d", filename, &start_line, &end_line);

        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            printf("Erreur lors de l'ouverture du fichier %s\n", filename);
            write(la_socket, "Erreur lors de l'ouverture du fichier\n", 37);
            continue;
        } else {
            printf("Fichier %s ouvert avec succès\n", filename);
        }

        // Compter le nombre total de lignes dans le fichier
        int total_lines = 0;
        char c;
        while ((c = fgetc(file)) != EOF) {
            if (c == '\n') {
                total_lines++;
            }
        }
        rewind(file); // Remettre le curseur au début du fichier

        if (start_line > total_lines || end_line < start_line) {
            char *error_msg = "Requête invalide : ligne de début ou de fin non valide.\n";
            write(la_socket, error_msg, strlen(error_msg));
            fclose(file);
            continue;
        }

        int current_line = 1;
        int ch;
        size_t index = 0;
        while ((ch = fgetc(file)) != EOF) {
            if (ch != '\n') {
                buffer[index++] = ch;
            }

            if (ch == '\n' || index == sizeof(buffer) - 1 || feof(file)) {
                buffer[index] = '\0';
                if (current_line >= start_line && current_line <= end_line) {
                    write(la_socket, buffer, strlen(buffer));
                    write(la_socket, "\n", 1);
                }
                index = 0;
                if (ch == '\n') {
                    current_line++;
                }
            }
        }

        if (current_line <= end_line) {
            char *end_error_msg = "Requête invalide : fin de fichier atteinte avant 'end_line'.\n";
            write(la_socket, end_error_msg, strlen(end_error_msg));
        } else {
            // Envoi du marqueur de fin de réponse
            write(la_socket, "FIN_REPONSE", strlen("FIN_REPONSE"));
        }

        if (file != NULL) {
            fclose(file);
        }
    }
    close(la_socket);
    printf("Connexion avec le client terminée.\n");
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

    printf("Serveur démarré. En attente de connexions...\n");
    while(server_running) {
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
    printf("serveur stoppé.\n");
    return 0;
}