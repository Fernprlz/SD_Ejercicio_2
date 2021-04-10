#include <stdio.h>
#include "claves.c"
#include <sys/types.h>
#include <sys/socket.h>


int main(int argc, char **argv) {


	///////////////////////////////////////////////////////////////////////////////
	int socket_desc; //?

	// TODO Modificar para
	if (IP_TUPLAS == NULL){
		printf("Error: IP del servidor no definida como variable de entorno.\n");
		exit(0);

	if (PORT_TUPLAS == NULL){
		printf("Error: Puerto del Socket del servidor no definida como variable de entorno.\n");
		exit(0);
	}

	// Retrieve environment variables, check for errors TODO
	struct sockaddr_in server_addr = getenv(IP_TUPLAS);	// Server IP
	struct hostent *hp = gethostbyname(getenv(PORT_TUPLAS)); // Host information

	socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Limpiar la estructura para guardar los datos del socket
	bzero((char *) &server_addr, sizeof(server_addr));
	hp = gethostbyname (argv[1]);

	memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4200);

	// se establece la conexión
	connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	// recibe la respuesta
	tm now;
	bzero(now, sizeof(tm));

	recibir(sd, date, 256);
	printf(“%s”, date);
	close (sd);
	exit(0);

	///////////////////////////////////////////////////////////////////////////////


}
