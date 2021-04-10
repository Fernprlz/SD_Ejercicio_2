#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "lines.h"

#define MAX_LINE 	256

int main(int argc, char *argv[])
{

        int sd;
        struct sockaddr_in server_addr;
        struct hostent *hp;

        if (argc != 3) {
                printf("Usage: client <serverAddress> <serverPort>\n");
                exit(0);
        }

	// the socket is create
        sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sd < 0){
                perror("Error in socket");
                exit(1);
        }

	// Obtain Server address 
        bzero((char *)&server_addr, sizeof(server_addr));
        hp = gethostbyname (argv[1]);
        if (hp == NULL) {
                perror("Error en gethostbyname");
                exit(1);
        }

	memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[2]));

	// Complete.....
	// First: establish the connection

        return (sd);

        return(0);
}

