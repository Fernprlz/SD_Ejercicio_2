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
#define MAX_THREADS   10
#define MAX_PETICIONES  256

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
int ocupado  = false;

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
    printf("Error: la clave ya existe.\n");
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
    printf("Error: la clave no existe.\n");
    return -1;
  }

  // Mandamos los punteros tal y como vienen, se utilizaran las direcciones que 
  // contienen para guardar el resultado de la funcion
  result = get(&list, key, v1, v2, v3);
  if (result == -1){
    printf("Error: clave no encontrada.\n");
     // esta parte no llega a ejecutarse porque si no se encuentra se sale en la comprobacion de arriba
     // lo cual hace el c??digo algo redundante.
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
    printf("Error: la clave no existe.\n");
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
    printf("Error: la clave no existe.\n");
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

void servicio(int *socket_global){

  int socket_local;
  // request  local
  request req;
  // Resultado del servicio
  int res;

  while(true){ // TODO: ?

    //////////////// Secci??n Cr??tica //////////////

    // Copiando la direccion del socket 
    pthread_mutex_lock(&mutex);

    socket_local = (* (int *) socket_global);
    ocupado = false;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(%m);
    
    /////////// Fin de la Secci??n Cr??tica //////////


    // Esperar a que se desocupe un lugar en el buffer
    pthread_mutex_lock(&mutex);
    while (n_elementos == MAX_PETICIONES){
      pthread_cond_wait(&no_lleno, &mutex);
    }
    read (client_socket, (char *) &req, sizeof(char)); // TODO: De que socket leo?
    
    buffer_peticiones[pos] = req;     // Recibe la request  en el buffer de peticiones
    pos = (pos + 1) % MAX_PETICIONES; // Mueve el puntero de posici??n del buffer de peticiones al siguiente hueco libre
    n_elementos++;
    pthread_cond_signal(&no_vacio);   // Avisa al resto de threads parados por el cond_wait
    pthread_mutex_unlock(&mutex);     // Libera el mutex global
  //fin
////////////////

    // Esperar a recibir peticiones
    while (n_elementos == 0) {
      // Evaluar estado de ejecuci??n del servidor
      if (fin == true) {
        fprintf(stderr,"Finalizando servicio\n");
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
      }
      pthread_cond_wait(&no_vacio, &mutex);
    }

    // Recoger la petici??n del buffer
    req = buffer_peticiones[pos];
    // Cambia el puntero de buffer
    pos = (pos + 1) % MAX_PETICIONES;
    n_elementos--;
    // Se??ales pertinentes a los threads
    pthread_cond_signal(&no_lleno);
    pthread_mutex_unlock(&mutex);
  
    /////////// Fin de la Secci??n Cr??tica //////////
  
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
        perror("C??digo de operaci??n invalido\n.");
        break;
    }
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
    printf("Error creando la lista de tupals.\n");
    return -1;
  }

  // Informacion para la conexi??n
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


  // Inicializaci??n de MUTEX de la cola y la pool 
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mfin, NULL);
  pthread_cond_init(&no_lleno, NULL);
  pthread_cond_init(&no_vacio, NULL);
  pthread_attr_init(&t_attr);
  // No nos importa su valor de retorno, luego los creamos Detached
  pthread_attr_setdetachedstate(&t_attr, PTHREAD_CREATE_DETACHED)

  
  // Creaci??n del socket
  if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
    printf ("SERVER: Error creando el socket\n");
    return -1;
  }

  // Estableciendo valores para las direcciones del socket
  bzero ((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;             // Dominio del socket. 
  server_addr.sin_port = htons(server_port);    // Puerto designado por comando
  server_addr. sin_addr.s_addr = INADDR_ANY;    // Direcci??n asignada automaticamente
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int)); 

  // Enlazado de direcciones al socket
  if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
    printf("SERVER: Error en el bind\n")
    return -1;
  }

  // Esperar conexi??n
  if (listen(server_socket, SOMAXCONN) == -1){
    printf("SERVER: Error en el listen.\n");
    return -1;
  }

  // Bucle de recepci??n de conexiones
  while(True){
    
    client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &socket_size);
    
    if (client_socket == -1){
      printf("SERVER: Error aceptando conexi??n\n")
      return -1;
    }

    // Thread que procesa la petici??n
    if (pthread_create(&thid) != 0){
      close(client_socket);
      printf("SERVER: Error procesando la conexi??n con %i\n", client_socket);
      continue;
    }

    //////////////// Secci??n Cr??tica //////////////
    
    // Copiado de la direcci??n del socket
    pthread_mutex_lock(&m);
    while(ocupado == true){
      pthread_cond_wait(&m, &c);
    }
    ocupado = true;
    pthread_mutex_unlock(&m);

    /////////// Fin de la Secci??n Cr??tica //////////
  }

  // Ejecuci??n del servidor terminada
  pthread_mutex_lock(&mfin);
  fin = true;
  pthread_mutex_unlock(&mfin);

  pthread_mutex_lock(&mutex);
  pthread_cond_broadcast(&no_vacio);
  pthread_mutex_unlock(&mutex);

  // Limpieza
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&no_lleno);
  pthread_cond_destroy(&no_vacio);
  pthread_mutex_destroy(&mfin);

  return 0;
}