/**
 * @author Marek Kovalčík, xkoval14@stud.fit.vutbr.cz
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define BUFFER 1024

//Globální proměnné
int hostUsed = -1;
int portUsed = -1;
bool nUsed    = false;
bool fUsed    = false;
bool lUsed    = false;
char *prepinac = "n";
char *host = NULL;
char *login = NULL;
int port = -1;
int nORlORf = 0;
int pocetPokusu = 5;

int count = 0;
int bytestx, bytesrx;
socklen_t serverlen;
struct hostent *server;
struct sockaddr_in server_address;
char buf[BUFFER];

// Deklarace funkcí
bool parseArguments(int, char**);

// Hlavní funkce programu
int main(int argc, char **argv){
    assert(parseArguments(argc, argv) == true);

    // 1. Vytvoření socketu
    int client_socket;

    // 2. ziskani adresy serveru pomoci DNS
    if ((server = gethostbyname(host)) == NULL) {
        fprintf(stderr,"ERROR: Neexistujici host %s\n", host);
        exit(EXIT_FAILURE);
    }
    // 3. nalezeni IP adresy serveru a inicializace struktury server_address
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);
    // tiskne informace o vzdalenem soketu
    //printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    // Vytvoření socketu
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    // 4. Vytvoření spojení se serverem
    // TODO ... nelze se připojit na evu
    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0){
        fprintf(stderr, "ERROR: Nelze se pripojit k serveru %s\n", host);
        exit(EXIT_FAILURE);
    }

    // 5. Odeslání socketu
    strcat(buf, prepinac);
    strcat(buf, login);
    //printf("Send to server: %s\n",buf);
    bytestx = send(client_socket, buf, strlen(buf), 0);
    if (bytestx < 0)
        perror("ERROR: sendto");

    // přijímání socketu pro -n, -f
    if(lUsed == false) {
        // 6. Přijetí socketu
        //while(pocetPokusu > 0) {
            bytesrx = recv(client_socket, buf, BUFFER, 0);
            //if ()
            //pocetPokusu--;
        //}
        if (bytesrx < 0)
            perror("ERROR: recvfrom");
        printf("%s", buf);
    // Přijímání socketu a odeslání odpovědi o doručení
    }else{
        // TODO potvrzování ve whilu
        while(1) {
            memset(buf, 0, BUFFER);
            bytesrx = recv(client_socket, buf, BUFFER, 0);
            if (bytesrx < 0) {
                perror("ERROR: recvfrom");
                break;
            }
            if(strcmp(buf, "end_connection\n") == 0)
                break;

            count++;
            //printf("Echo from server: %s", buf);
            printf("%s\n", buf);
            //memset(buf, 0, BUFFER);
            // TODO send to server
            bytestx = send(client_socket, "OK", strlen(buf), 0);
            if (bytestx < 0){
                perror("ERROR: odeslani potvrzovaciho socketu");
                break;
            }
        }
        if(count==0)
            fprintf(stderr, "INFO: pro prefix '%s' nebylo nic nalezeno\n", login);
    }

    close(client_socket);
    //printf("\nIPK-CLIENT: host=%s | port=%d | n=%d | f=%d | l=%d | login=%s\n", host, port, nUsed, fUsed, lUsed, login);
    return 0;
}

/*
 * Funkce zkontroluje a rozparsuje argumenty
 */
bool parseArguments(int argc, char **argv){
    if(argc > 7){
        fprintf(stderr, "ERROR: Spatne zadane parametry. Nespravny pocet argumentu!\n");
        exit(EXIT_FAILURE);
    }
    int c;
    while ((c = getopt (argc, argv, ":h:p:n:f:l")) != -1)
        switch (c){
            case 'h': hostUsed = (int)atol(optarg);
                host = optarg;
                break;
            case 'p': portUsed = (int)atol(optarg);
                //if ((port = atoi(optarg)) <= 0) {
                if ((port = strtod(optarg, NULL)) <= 0) {
                    fprintf(stderr, "ERROR: Spatne zadane parametry. PORT musi byt kladne cislo > 0 a < 65536\n");
                    exit(EXIT_FAILURE);
                }else if(port > 65535){
                    fprintf(stderr, "ERROR: Spatne zadane parametry. PORT musi byt kladne cislo > 0 a < 65536\n");
                    exit(EXIT_FAILURE);
                }else
                    break;
            case 'n':
                nUsed = true;
                nORlORf++;
                prepinac = "n";
                break;
            case 'f':
                fUsed = true;
                nORlORf++;
                prepinac = "f";
                break;
            case 'l':
                lUsed = true;
                nORlORf++;
                prepinac = "l";
                break;
            case '?':
                fprintf(stderr, "ERROR: Neplatny prepinac\n");
                exit(EXIT_FAILURE);
            default:
                break;
        }
    if(argc == 6)
        login = "";
    else
        login = argv[6];
    if (hostUsed == -1) {
        fprintf(stderr,"ERROR: Spatne zadane parametry. -h je povinny argument\n");
        exit(EXIT_FAILURE);
    }
    if (portUsed == -1) {
        fprintf(stderr,"ERROR: Spatne zadane parametry. -p je povinny argument\n");
        exit(EXIT_FAILURE);
    }
    if(nORlORf != 1){
        fprintf(stderr, "ERROR: Spatne zadane parametry. Prave jeden z [  -n LOGIN|-f LOGIN|-l [PREFIX]  ] musi byt aktivni.\n\t-n, -f ma povinny parametr LOGIN\n");
        exit(EXIT_FAILURE);
    }
    return true;
}