#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){

    /* 
        socket -> création de la prise
            #include sys/socket.h
            int socket(int domaine, int type, int protocole);
                domaine : PF_INET (==> IPv4) ou PF_INET6 (==> IPv6)
                type : SOCK_STREAM (~ TCP) ou SOCK_DGRAM (~ UDP) ... 
                protocole : famille de protocole ==> 0

                retourne le descripteur

        connect -> attente de connection
            #include sys/socket.h
            int accept(int socket, struct sockaddr* adresse, socklen_t longueur);
                socket : la prise définie + haut
                adresse : inforamtion sur le client (adresse)
                longueur : inforamtion sur le client (taille de l'adresse du client)

                retourne un descripteur de d'entrée sortie

        send/receve -> echange
        close -> fermeture
    
    */

    // Creation de la socket
    int ma_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(ma_socket == -1){
        fprintf(stderr, "pb de création de socket\n");
        exit(EXIT_FAILURE);        
    }
    // Resolution de nom


    // connection au serveur
    struct sockaddr_in mon_adresse_serveur;
    memset(&mon_adresse_serveur, 0, sizeof(mon_adresse_serveur));
    mon_adresse_serveur.sin_family = AF_INET;
    mon_adresse_serveur.sin_port = htons(61234);
    mon_adresse_serveur.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if(connect(ma_socket, (struct sockaddr*)&mon_adresse_serveur, sizeof(struct sockaddr_in))){
        fprintf(stderr, "pb de connection\n");
        close(ma_socket);
        exit(EXIT_FAILURE);
    }

    // Echange d'information
    write(ma_socket, "Hello", 6);
        char mon_buffer[256];
    int mon_nb_lus;
    int i;
    mon_nb_lus = read(ma_socket, mon_buffer, 256);
    printf("%d [", mon_nb_lus);
    for(i = 0; i < mon_nb_lus; i++){
        printf("%c", mon_buffer[i]);
    }
    printf("]\n");

    close(ma_socket);

    return 0;
}