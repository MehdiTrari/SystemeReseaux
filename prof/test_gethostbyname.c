#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


void afficher_hostent(const struct hostent* ceci){
    int i;
    if(ceci == NULL){
        printf("echec de resolution\n");
        return;
    }
    printf("nom : %s\n", ceci->h_name);
    if(ceci->h_addrtype == AF_INET){
        printf("\tAdresse en ip V4\n");
    }else if(ceci->h_addrtype == AF_INET6){
        printf("\tAdresse en ip V6\n");
    }else{
        printf("\tAdresse de type inconnu");
    }
    for(i = 0; ceci->h_aliases[i] != NULL; i++){
        printf("\talias : %s\n", ceci->h_aliases[i]);
    }
    if(ceci->h_addrtype == AF_INET){
        for(i = 0; ceci->h_addr_list[i] != NULL; i++){  
            printf("\tadresse %d : %s\n", i, inet_ntoa(*((struct in_addr*)ceci->h_addr_list[i])));
        }
    }else if(ceci->h_addrtype == AF_INET6){       
        char mon_addresse_en_chaine_ipv6[INET6_ADDRSTRLEN]; 
        for(i = 0; ceci->h_addr_list[i] != NULL; i++){  
            inet_ntop(AF_INET6, inet_ntoa(*((struct in_addr*)ceci->h_addr_list[i])), mon_addresse_en_chaine_ipv6, INET6_ADDRSTRLEN);
            printf("\tadresse %d : %s\n", i, mon_addresse_en_chaine_ipv6);
        }
    }
    printf("----------\n");

}

int main(int argc, char** argv){
    int i;
    struct hostent* mon_entree_hote;
    if(argc == 1){
        printf("USAGE :\n\t./gethostbyname.exe nom [nom ...]\n");
        printf("EXEMPLE :\n\t./gethostbyname.exe fges.fr www.edn.fr bing.com www.bing.fr www.bing.com loopsofzen.co.uk\n");
    }

    // gethostbyname // obsolete
    for(i = 1; i < argc; i++){
        mon_entree_hote = gethostbyname(argv[i]); /* basic */
        afficher_hostent(mon_entree_hote);
    }
    
    return 0;
}