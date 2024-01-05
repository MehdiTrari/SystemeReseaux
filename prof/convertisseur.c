#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(){

    const char* mon_addresse_en_chaine = "127.0.0.1";
    in_addr_t mon_addresse = inet_addr(mon_addresse_en_chaine);
    printf("mon adresse en chaine %s => %x \n", mon_addresse_en_chaine, mon_addresse);

    struct in_addr mon_addresse_structure;
    mon_addresse_structure.s_addr = mon_addresse;

    printf("mon adresse en chaine %s => %x \n", inet_ntoa(mon_addresse_structure), mon_addresse_structure.s_addr);

    /* 8 champs de 2 octets chacun écrit en hexadecimal */
    struct in6_addr mon_addresse_ipv6;
    inet_pton(AF_INET6, "15e0:1d59:16A0:c161:B255:ffff:255.255.255.255", &mon_addresse_ipv6);
    char mon_addresse_en_chaine_ipv6[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &mon_addresse_ipv6, mon_addresse_en_chaine_ipv6, INET6_ADDRSTRLEN);

    printf("mon adresse en chaine => %s sur au plus %d caractères (%ld) \n",  mon_addresse_en_chaine_ipv6, INET6_ADDRSTRLEN, strlen(mon_addresse_en_chaine_ipv6));
    /* 32 caractères pour les champs
        7 caractères pour les délimiteurs de champs (les ':')
        1 caractère pour la fin de lignes (le '\0')
        reste 6 carectères pour une notation autorisée "non standard" avec une ipv4 sur la fin de l'adresse.
     */

    return 0;
}