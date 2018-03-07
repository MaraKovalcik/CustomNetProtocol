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

#define BUFFER 100
#define SERVER_FILE "/etc/passwd"
#define TEST_FILE "test"

FILE *pFile;
char *line = NULL;
size_t len = 0;
ssize_t read_line;
char * pch;
int position = 0;

void userInformation(char *login);
void userHomeDirectory(char *login);
void listOfUsers(char *prefix);

int main(){

    if((pFile = fopen(TEST_FILE, "r")) == NULL){
        fprintf(stderr, "Soubor %s se nepodarilo otevrit\n", SERVER_FILE);
        exit(1);
    }

    //userInformation("xkoval14");
    //userHomeDirectory("xkoval14");
    listOfUsers("xkov");

    fclose(pFile);
    if (line) free(line);
    return 0;
}

// Vypsání seznamu všech uživatelů s možným prefixem pro vypisování
void listOfUsers(char *prefix){
    int count = 0;
    while ((read_line = getline(&line, &len, pFile)) != -1) {
        pch = strtok (line,":");
        if(strncmp (pch, prefix, strlen(prefix)) == 0){
            count++;
            printf ("%s\n", pch);
        }
    }
    if(count == 0)
        fprintf(stderr, "Nic nenalezeno pro prefix '%s'\n", prefix);
}

// Vypsání domovského adresáře uživatele
void userHomeDirectory(char *login){
    int count = 0;
    while ((read_line = getline(&line, &len, pFile)) != -1) {
        pch = strtok (line,":");
        if(strcmp(login, pch) == 0) {
            while (pch != NULL) {
                position++;
                if (position == 6) {
                    count ++;
                    printf("%s ", pch);
                }
                pch = strtok(NULL, ":");
            }
            position = 0;
            printf("\n");
        }
    }
    if(count == 0)
        fprintf(stderr, "Nic nenalezeno pro login '%s'\n", login);
}

// Vypsání informací o uživateli
void userInformation(char *login){
    int count = 0;
    while ((read_line = getline(&line, &len, pFile)) != -1) {
        pch = strtok (line,":");
        if(strcmp(login, pch) == 0) {
            while (pch != NULL) {
                position++;
                if (position == 5) {
                    count++;
                    printf("%s ", pch);
                }
                pch = strtok(NULL, ":");
            }
            position = 0;
            printf("\n");
        }
    }
    if(count == 0)
        fprintf(stderr, "Nic nenalezeno pro login '%s'\n", login);
}
