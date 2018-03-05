/**
 * @author Marek Kovalčík, xkoval14@stud.fit.vutbr.cz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER 1024
#define SERVER_FILE "/etc/passwd"

//Globální proměnné
int portUsed = -1;
int port_number = -1;
int rc;
int welcome_socket;
struct sockaddr_in6 sa;
struct sockaddr_in6 sa_client;
char str[INET6_ADDRSTRLEN];
int port_number;

// Deklarace funkcí
bool parseArguments(int, char**);
char* parseData(char tmpbuff[BUFFER]);

int main(int argc, char **argv) {
    parseArguments(argc, argv);
    printf("Port: %d \t PortUsed: %d\n", port_number, portUsed);

    socklen_t sa_client_len=sizeof(sa_client);
    if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    memset(&sa,0,sizeof(sa));
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(port_number);

    if ((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0)
    {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
    if ((listen(welcome_socket, 1)) < 0)
    {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);
        if (comm_socket > 0)
        {
            if(inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))) {
                printf("INFO: New connection:\n");
                printf("INFO: Client address is %s\n", str);
                printf("INFO: Client port is %d\n", ntohs(sa_client.sin6_port));
            }

            char buff[BUFFER];
            int res = 0;
            for (;;)
            {
                res = recv(comm_socket, buff, BUFFER,0);
                if (res <= 0)
                    break;
                printf("Received from client: %s\n", buff);

                // TODO ... parse and send data to client
                char *tmpRetezec = parseData(buff);
                strcpy(buff, tmpRetezec);

                send(comm_socket, buff, strlen(buff), 0);
                printf("Send to client: %s\n", buff);
            }
        }
        else
        {
            printf(".");
        }
        printf("Connection to %s closed\n",str);
        close(comm_socket);
    }
}
/*
 * Funkce pro zpracování dat na serveru
 */
char* parseData(char tmpbuff[BUFFER]){

    strcat(tmpbuff, "XXXXXXXXXX");
    return tmpbuff;
}

/*
 * Funkce pro zpracování argument
 */
bool parseArguments(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "ERROR: Spatne zadane parametry. Nespravny pocet argumentu!\n");
        exit(EXIT_FAILURE);
    }
    int c;
    while ((c = getopt (argc, argv, "p:")) != -1)
        switch (c){
            case 'p': portUsed = (int)atol(optarg);
                //if( isdigit(optarg) ) {
                    if ((port_number = atoi(optarg)) < 0) {
                        fprintf(stderr, "ERROR: Spatne zadane parametry. PORT musi byt kladne cislo\n");
                        exit(EXIT_FAILURE);
                    }
                /*}
                else{
                    fprintf(stderr, "ERROR: Spatne zadane parametry. PORT musi byt kladne cislo\n");
                    exit(EXIT_FAILURE);
                }*/
                break;
            case '?':
                fprintf(stderr, "ERROR: Spatne zadane parametry.\n");
                exit(EXIT_FAILURE);
        }
    return true;
}