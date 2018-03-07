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
#include <time.h>

#define BUFFER 1024
#define SERVER_FILE "/etc/passwd"
#define TEST_FILE "test"
//#define TEST_FILE "test2"

//Globální proměnné
int portUsed = -1;
int port_number = -1;
int rc;
int welcome_socket;
struct sockaddr_in6 sa;
struct sockaddr_in6 sa_client;
char str[INET6_ADDRSTRLEN];
int port_number;
bool nUsed = false, lUsed = false, fUsed = false;

/*FILE *pFile;
const size_t line_size = 300;
//char* line_of_file = malloc(line_size);
char *line = NULL;
size_t len = 0;
ssize_t read_line;
char * pch;
int position = 0;*/

char tmpBuffer[BUFFER];

// Deklarace funkcí
bool parseArguments(int, char**);
char* parseData(char tmpbuff[BUFFER], int);

void userInformation(char *login);
char* userHomeDirectory(char *login);
char* listOfUsers(char *prefix);

int main(int argc, char **argv) {
    parseArguments(argc, argv);
    //printf("Port: %d \t PortUsed: %d\n", port_number, portUsed);

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
                memset(buff, 0, sizeof(buff));

                res = recv(comm_socket, buff, BUFFER,0);
                if (res <= 0)
                    break;
                printf("Received from client: %s\n", buff);

                // TODO ... parse and send data to client
                char *tmpRetezec = NULL;
                tmpRetezec = parseData(buff, comm_socket);

                strcpy(buff, tmpRetezec);
                if(lUsed == false) {
                    send(comm_socket, buff, strlen(buff), 0);
                    printf("Send to client: %s\n", buff);
                }
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
char* parseData(char buff[BUFFER], int comm_socket){
    FILE *pFile;
    char *line = NULL;
    size_t len = 0;
    ssize_t read_line;
    char * pch;
    int position = 0;

    char login[BUFFER];
    int count = 0;
    nUsed = false; lUsed = false; fUsed = false;
    if(buff[0] == 'n')   nUsed = true;
    if(buff[0] == 'l')   lUsed = true;
    if(buff[0] == 'f')   fUsed = true;
    for(int i = 0; i<BUFFER; i++)
        login[i] = buff[i+1];

    strcpy(buff, "");

    if((pFile = fopen(TEST_FILE, "r")) == NULL){
        fprintf(stderr, "Soubor %s se nepodarilo otevrit\n", SERVER_FILE);
        exit(1);
    }

    // Vypisování informací o uživateli
    if(nUsed){
        printf("Bude se vypisovat plne jmeno uzivatele: %s\n", login);
        while ((read_line = getline(&line, &len, pFile)) != -1) {
            pch = strtok (line,":");
            if(strcmp(login, pch) == 0) {
                while (pch != NULL) {
                    position++;
                    if (position == 5) {
                        count++;
                        strcat(buff, pch);
                        strcat(buff, "\n");
                    }
                    pch = strtok(NULL, ":");
                }
                position = 0;
            }
        }
    // Vypisování domovského adresáře uživatele
    }else if(fUsed){
        printf("Budou se vypisovat informace o domovskem adresari uzivatele: %s\n", login);
        while ((read_line = getline(&line, &len, pFile)) != -1) {
            pch = strtok (line,":");
            if(strcmp(login, pch) == 0) {
                while (pch != NULL) {
                    position++;
                    if (position == 6) {
                        count ++;
                        strcat(buff, pch);
                        strcat(buff, "\n");
                    }
                    pch = strtok(NULL, ":");
                }
                position = 0;
            }
        }
    // Vypisování listu uživatelů s možným prefixem
    }else if(lUsed){
        int res = 0;
        printf("Budou se vypisovat login vsech uzivatelu hledanych s prefixem: %s\n", login);
        while ((read_line = getline(&line, &len, pFile)) != -1) {
            pch = strtok (line,":");
            memset(buff, 0, BUFFER);
           // strcpy(buff, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
            if(strncmp (pch, login, strlen(login)) == 0){
                count++;
                // TODO smazat buff
                //memset(buff, 0, BUFFER);
                //strcpy(buff, pch);
                strcpy(buff, pch);
                //strcat(buff, "\n");
                // odeslání jednoho loginu na klienta
                send(comm_socket, buff, strlen(buff), 0);
                printf("Send to client: %s", buff);
                // přijetí potvrzující zprávy z klienta
                res = recv(comm_socket, buff, BUFFER,0);
                if (res <= 0) {
                    printf("\tNeprisla odpoved od klienta, odesilani se ukoncuje\n");
                    break;
                }
                else
                    printf("\tKlientovi socket dobre dosel\n");
            }
        }
        // Odeslání socketu o ukončení spojení
        strcpy(buff, "end_connection\n");
        send(comm_socket, buff, strlen(buff), 0);
        printf("Send to client: %s", buff);
    }

    if(count == 0) {
        strcpy(buff, "INFO: Nic nebylo nalezeno pro '");
        strcat(buff, login);
        strcat(buff, "'\n");
    }

    fclose(pFile);
    if (line) free(line);

    return buff;
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