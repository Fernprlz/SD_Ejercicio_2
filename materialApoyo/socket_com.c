#include <unistd.h>
#include <errno.h>
#include "socket_comm.h"

int enviar(int socket, char * buffer, int len){
	int recibido;
	int enviado = len;

	do {	
		recibido = write(socket, buffer, len);
		enviado = enviado - recibido;
		buffer = buffer + recibido;
	} while ((enviado > 0) && (recibido >= 0));

	if (r < 0){
		printf("Error enviando los datos.")
		return (-1);
	} else {
		return(0);	
	}
}

int recibir(int socket, char *buffer, int len){
	int recibido;
	int enviado = len;

	do {	
		recibido = read(socket, buffer, l);
		enviado = enviado - recibido ;
		buffer = buffer + recibido;
	} while ((enviado > 0) && (recibido >= 0));

	if (recibido < 0){
		printf("Error recibiendo los datos.")
		return (-1);   
	} else{
		return(0);	
	}
}



ssize_t leer_linea(int fd, void *buffer, size_t n){
	ssize_t leido; // Bytes recibidos por el último write
	size_t por_leer; // Bytes por leer.
	char *buf;
	char ch;


	if (n <= 0 || buffer == NULL) { 
		errno = EINVAL;
		return -1; 
	}

	buf = buffer;
	por_leer = 0;	

	for (;;) {
		leido = read(fd, &ch, 1);	/* read a byte */

		if (leido == -1) {	
			if (errno == EINTR){
				/* interrupted -> restart read() */
				continue;
			} else{
				return -1;		/* some other error */
			} 
		} else if (leido == 0) {	/* EOF */
			if (por_leer == 0){	/* no byres read; return 0 */
				return 0;
			} else{
				break;
			}
		} else {			/* leido must be 1 if we get here*/
			if (ch == '\n'){
				break;
			}

			if (ch == '\0'){
				break;
			}

			if (por_leer < n - 1) {		/* discard > (n-1) bytes */
				por_leer++;
				*buf++ = ch; 
			}
		} 
	}

	*buf = '\0';
	return por_leer;
}
