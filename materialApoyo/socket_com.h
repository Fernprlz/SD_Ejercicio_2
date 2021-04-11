#include <unistd.h>

int enviar(int socket, char *buffer, int len);
int recibir(int socket, char *buffer, int len);
size_t leer_linea(int fd, void *buffer, size_t n);