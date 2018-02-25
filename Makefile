#
# Předmět  :   IPP 2018
# Soubor   :   Makefile
# Projekt  :   Klient-server pro získání informace o uživatelích
# Varianta :   1
# Autor    :   xkoval14, Marek Kovalčík
#

CLIENT=ipk-client
SERVER=ipk-server
CLIENTFILES=ipk-client.c # případné další osubory
SERVERFILES=ipk-server.c # případné další osubory
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
CC=gcc
RM=rm -f

all : $(CLIENT)
$(CLIENT) : $(CLIENTFILES)
		$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENTFILES) -lm

all : $(SERVER)
$(SERVER) : $(SERVERFILES)
		$(CC) $(CFLAGS) -o $(SERVER) $(SERVERFILES) -lm

clean :
	$(RM) *.o $(CLIENT) $(SERVER)