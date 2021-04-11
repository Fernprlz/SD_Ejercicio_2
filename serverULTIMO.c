#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "linked_list.h"
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "socket_com.c"
#include <arpa/inet.h>

#ifndef REQUEST_H
  #define REQUEST_H
  #include "request.h"
#endif



////////////////////////////////////////////////////////////////////////////////
////////////////// V A R I A B L E S - G L O B A L E S  ////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Variables de control de capacidad del servidor
#define MAX_THREADS 	10
#define MAX_PETICIONES 	256

// Lista enlzada para guardar tuplas
Linked_list list;

// Variables del buffer de peticiones
request buffer_peticiones[MAX_PETICIONES];
int n_elementos;
int pos = 0;

// MUTEX de condiciones de estado de la cola
pthread_mutex_t mutex;
pthread_cond_t no_lleno;
pthread_cond_t no_vacio;

// MUTEX de control de estado de los threads
pthread_mutex_t mfin;
int fin = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// S E R V I C I O S ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int init(){
  int result = init_list(&list);
  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int exists(char *key){
  return item_exist(list, key);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int set_value(char *key , char *v1, int *v2, float *v3){
  int result;

  if (exists(key) == 0){ 
    printf("Error: la clave ya existe.");
    return -1;
  }

  // Obtenemos el valor de lo que contienen las direcciones v2, v3.
  result = set(&list, key, v1, *v2, *v3); 

  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int get_value(char *key , char *v1, int *v2, float *v3){
  int result;

  if (exists(key) == -1){
    printf("Error: la clave no existe.");
    return -1;
  }

  // Mandamos los punteros tal y como vienen, se utilizaran las direcciones que 
  // contienen para guardar el resultado de la funcion
  result = get(&list, key, v1, v2, v3);
  if (result == -1){
    printf("Error: clave no encontrada.");
     // esta parte no llega a ejecutarse porque si no se encuentra se sale en la comprobacion de arriba
     // lo cual hace el código algo redundante.
    return -1;
  } else {
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int modify_value(char *key , char *v1, int *v2, float *v3){
  int result;

  if (exists(key) == -1){ // TODO Estoy pasando las variables adecuadamente?
    printf("Error: la clave no existe.");
    return -1;
  }

  result = mod(&list, key, v1, v2, v3);

  //TODO: posibles casos de fallo: los valores introducidos son incorrectos.
  /*if (result == -1){
    printf("")
    return -1;
  }*/
  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int delete_key(char *key){
  int result;

  if (exists(key) == -1){ 
    printf("Error: la clave no existe.");
    return -1;
  }

  result = delete(&list, key);

  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int num_items(){
  if (list_size == -1){
    printf("Error: la lista no ha sido inicializada.\n");
  } 
  
  return list_size;
}

////////////////////////////////////////////////////////////////////////////////
//////////////// F U N C I O N - P A R A - T H R E A D S ///////////////////////
////////////////////////////////////////////////////////////////////////////////

void servicio(void ){


  ////////////////////////////
  void tratar_peticion(void * s) {
    int s_local;
    pthread_mutex_lock(&m);
    s_local = (* (int *)s);
    busy = FALSE;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
    /* tratar la petición utilizando el descriptor s_local */
    pthread_exit(NULL);
  }
  ///////////////////////////


  // request  local
  request req;
  
  // Resultado del servicio
  int res;

	while(true){
    //////////////// Sección Crítica //////////////
		pthread_mutex_lock(&mutex);

    // Esperar a recibir peticiones
		while (n_elementos == 0) {
      // Evaluar estado de ejecución del servidor
      if (fin == true) {
				fprintf(stderr,"Finalizando servicio\n");
				pthread_mutex_unlock(&mutex);
				pthread_exit(0);
			}
			pthread_cond_wait(&no_vacio, &mutex);
		}

    // Recoger la petición del buffer
		req = buffer_peticiones[pos];
    // Cambia el puntero de buffer
		pos = (pos + 1) % MAX_PETICIONES;
		n_elementos--;
    // Señales pertinentes a los threads
		pthread_cond_signal(&no_lleno);

		pthread_mutex_unlock(&mutex);
    //////////////////////////////////////////////

		// Procesado de la request
    switch(req -> op){
      case INIT:
        res = init();
        break;
      case SET:
        res = set_value(req -> key, req -> v1, req -> v2, req -> v3);
        break;
      case GET:
        res = get_value(req -> key, req -> v1, req -> v2, req -> v3);
        break;
      case MOD:
        res = modify_value(req -> key, req -> v1, req -> v2, req -> v3);
        break;
      case DEL:
        res = delete_key(req -> key);
        break;
      case EXIST:
        res = exists(req -> key);
        break;
      case ITEMS:
        res = num_items();
        break;
      default:
        res = -1;
        perror("Código de operación invalido");
        break;
    }


		// TODO: Respuesta al socket del cliente 
		
	}
 	pthread_exit(0);
}

////////////////////////////////////////////////////////////////////////////////
///////////////////// E J E C U C I O N - S E R V I D O R //////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  int server_port = argv[1];

  // Lista enlazada para tuplas
  int err = init_list(&list);
  if (err == -1){
    printf("Error creando la lista de tupals.");
    return -1;
  }

  // Informacion para la conexión
  int server_socket, client_socket;
  socklen_t socket_size;
  struct sockaddr_in server_addr, client_addr;
  
  // request  actual
  request req;

	// Threads
	pthread_attr_t t_attr;
	pthread_t thid[MAX_THREADS];
	int error;
	int pos = 0;


  // Inicialización de MUTEX de la cola y la pool TODO: Eliminar mutex de la cola
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&no_lleno, NULL);
	pthread_cond_init(&no_vacio, NULL);
	pthread_mutex_init(&mfin, NULL);

  // Creación de la pool de threads
	pthread_attr_init(&t_attr);
	for (int ii = 0; ii < MAX_THREADS; ii++){
		if (pthread_create(&thid[ii], NULL, (void *) servicio, NULL) !=0){
			perror("Error creando la pool de threads\n");
			return -1;
		}
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Creación del socket
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf ("SERVER: Error creando el socket");
		return -1;
	}

  // Estableciendo valores para las direcciones del socket
  bzero ((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;             // Dominio del socket. 
  server_addr.sin_port = htons(server_port);    // Puerto designado por comando
  server_addr. sin_addr.s_addr = INADDR_ANY;    // Dirección asignada automaticamente

  // Enlazado de direcciones al socket
  if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
    printf("SERVER: Error en el bind")
    return -1;
  }
  
  // Esperar conexión
  listen(server_socket, SOMAXCONN);

  // Bucle de recepción de conexiones
  while(True){
    
    client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &socket_size);
    
    if (client_socket == -1){
      printf("SERVER: Error aceptando conexión")
      return -1;
    }

    // Delegación del procesado a un thread.
    // TODO: Dirigir al thid adecuado de la pool de threads.
    pthread_create(&thid, &attr, servicio, (void *) &client_socket);
    pthread_mutex_lock(&m);
    while(no_lleno == false){// cambiar las variables a las correctas
      pthread_cond_wait(&m, &c);
    } 
    no_lleno = true;
    pthread_mutex_unlock(&m);



  }




	// setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int)); mirar como se usa


  // Bucle de ejecución del servidor
	while (true) {
    // Recepción de un request
		sc = accept(sd, (struct sockaddr *)&client_addr,(socklen_t *)&size);
		if (sc == -1){
			perror("Error en la recepción de un request");
			break;
		}

    // Esperar a que se desocupe un lugar en el buffer
		pthread_mutex_lock(&mutex);
		while (n_elementos == MAX_PETICIONES){
      pthread_cond_wait(&no_lleno, &mutex);
    }
		read ( sc, (char *) &req, sizeof(char));
		
		buffer_peticiones[pos] = req;     // Recibe la request  en el buffer de peticiones
		pos = (pos + 1) % MAX_PETICIONES; // Mueve el puntero de posición del buffer de peticiones al siguiente hueco libre
		n_elementos++;
		pthread_cond_signal(&no_vacio);   // Avisa al resto de threads parados por el cond_wait
		pthread_mutex_unlock(&mutex);     // Libera el mutex global
	}

  // Ejecución del servidor terminada
	pthread_mutex_lock(&mfin);
	fin = true;
	pthread_mutex_unlock(&mfin);

	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&no_vacio);
	pthread_mutex_unlock(&mutex);

  // Recogida de threads
	for (int i = 0; i < MAX_THREADS; i++){
		pthread_join(thid[i], NULL);
  }

  // Limpieza
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&no_lleno);
	pthread_cond_destroy(&no_vacio);
	pthread_mutex_destroy(&mfin);

	return 0;
}