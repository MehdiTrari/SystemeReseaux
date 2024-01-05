#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

struct addrinfo* mes_infos_d_adressages;
struct sockaddr_in* mon_adresse_socket_ipv4;
struct sockaddr_in6* mon_adresse_socket_ipv6;
char mon_addresse_en_chaine_ipv4[INET_ADDRSTRLEN];
char mon_addresse_en_chaine_ipv6[INET6_ADDRSTRLEN];

void afficher_addrinfo(struct addrinfo* ceci){
    switch(ceci->ai_addr->sa_family){
        case AF_INET :
            mon_adresse_socket_ipv4 = (struct sockaddr_in*)(ceci->ai_addr);
            inet_ntop(AF_INET, &(mon_adresse_socket_ipv4->sin_addr), mon_addresse_en_chaine_ipv4, INET_ADDRSTRLEN);
            printf("\tipv4 %s\n", mon_addresse_en_chaine_ipv4);
            break;
        case AF_INET6 :
            mon_adresse_socket_ipv6 = (struct sockaddr_in6*)(ceci->ai_addr);
            inet_ntop(AF_INET6, &(mon_adresse_socket_ipv6->sin6_addr), mon_addresse_en_chaine_ipv6, INET6_ADDRSTRLEN);
            printf("\tipv6 %s\n", mon_addresse_en_chaine_ipv6);
            break;
        default:
            printf("%s\n", "protocole inconnu");
    }
}

int main(int argc, char** argv){
    int i;
    if(argc == 1){
        printf("USAGE :\n\t./test_getaddrinfo.exe nom [nom ...]\n");
        printf("EXEMPLE :\n\t./test_getaddrinfo.exe fges.fr www.edn.fr bing.com www.bing.fr www.bing.com loopsofzen.co.uk\n");
    }

    /**
     * int		getaddrinfo (const char * node, const char * service,
			     const struct addrinfo * hints, struct addrinfo ** res);
     * node : "l'url" qui dont on souhaite résoudre le nom
        ou NULL si on veut des infos sur la machine courant (loopback)
     * service : un nom de fichier faisant la correspondance entre un nom intelligible   et un port/protocole. => on mettra NULL.
     * hints : un filtre pour les informations : ai_family, ai_socktype, et ai_protocol => on mettra NULL.
     *  res l'adresse où écrire le résultat.
    */
    
    /* + moderne */


    struct addrinfo* iter;
    /* on peut utiliser un "filtre" une sorte de modèle
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
    */
    for(i = 1; i < argc; i++){

        if(getaddrinfo(argv[i], NULL, /* &hints*/ NULL, &mes_infos_d_adressages) != 0){
            printf("Erreur : getaddrinfo:\n");
            exit(EXIT_FAILURE);
        }
        printf("--------------\n%s\n", argv[i]);
        for(iter = mes_infos_d_adressages; iter != NULL; iter = iter->ai_next){
            afficher_addrinfo(iter);
        }

        freeaddrinfo(mes_infos_d_adressages);
    }
    
    return 0;
}