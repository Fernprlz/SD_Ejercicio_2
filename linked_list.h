# include <stdio.h>
# include <stdlib.h>
# include <string.h>

#ifndef REQUEST_H
  #define REQUEST_H
  #include "request.h"
#endif


////////////////////////////////////////////////////////////////////////////////
/////////////////////////// V A R I A B L E S  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Node {
  char key[MAXSIZE];

  char v1[MAXSIZE];
  int v2;
  float v3;

  struct Node *next;
};

typedef struct Node *Linked_list;

int list_size = -1;

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// F U N C I O N E S  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int init_list (Linked_list *l) {
  *l = NULL;
  list_size = 0;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int set(Linked_list *l, char *key , char *v1, int v2, float v3){
  // Adición de un nuevo nodo
  struct Node *ptr ;
  // Reserva de espacio en memoria para el nuevo nodo
  ptr = ( struct Node *) malloc ( sizeof ( struct Node ));

  if (ptr == NULL) {
    printf("Error añadiendo el elemento al vector: Memoria insuficiente.\n");
    return -1;
  }

  // Casos: Lista vacía o no
  if (*l == NULL) {
    strcpy (ptr -> key, key);
    strcpy (ptr -> v1, v1);
    ptr -> v2 = v2;
    ptr -> v3 = v3;
    ptr -> next = NULL ;
    *l = ptr;
  } else {
    // insert in head
    strcpy (ptr -> key , key);
    strcpy (ptr -> v1, v1);
    ptr -> v2 = v2;
    ptr -> v3 = v3;
    ptr -> next = *l;
    *l = ptr;
  }
  list_size++;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int get(Linked_list *l, char *key , char *v1, int *v2, float *v3){
  Linked_list aux;
  aux = *l;
  // Busqueda en la lista auxiliar, copia de la original
  while (aux != NULL) {
    if (strcmp (aux -> key , key) == 0){
      strcpy (v1, (char *) aux -> v1);
      *v2 = aux -> v2;
      *v3 = aux -> v3;
      // Encontrado
      return 0;
    } else {
      // Siguiente elemento
      aux = aux -> next;
    }
  }
  // No Encontrado
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int mod(Linked_list *l, char *key , char *v1, int *v2, float *v3){
  // Buscar el nodo con la clave deseada
  Linked_list aux;
  aux = *l;
  while (aux != NULL) {
    if (strcmp (aux -> key , key) == 0){
      strcpy (aux -> v1, v1);
      aux -> v2 = *v2;
      aux -> v3 = *v3;
      // Encontrado
      return 0;
    } else {
      // Siguiente elemento
      aux = aux -> next;
    }
  }
  // No encontrado
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int printList (Linked_list l){
  Linked_list aux;
  aux = l;
  while (aux != NULL ){
    printf ("Key = \"%s\"\nV1 = \"%s\"\nV2 = %d\nV3 = %f\n", aux ->key, aux -> v1, aux -> v2, aux -> v3);
    printf("----------------------\n");
    aux = aux -> next ;
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * Las comprobaciones sobre la existencia de la clave se hacen en servidor.c TODO?
 *
 */
int delete (Linked_list *l, char *key){
  Linked_list aux , back ;

  // Lista vacía
  if (*l == NULL ){
   return 0;
  }

  // Borrar el primer nodo
  if (strcmp (key , (*l) -> key) == 0){
    aux = *l;
    *l = (*l) -> next ;
    free(aux);
    list_size--;
    return 0;
  }

  // Borrar cualquier otro nodo
  aux = *l;
  back = *l;
  while (aux != NULL ) {
    if (strcmp (aux -> key , key)==0) {
      back -> next = aux -> next ;
      free (aux);
      list_size--;
      break;
    } else {
      back = aux;
      aux = aux -> next ;
    }
  }
  // No encontrado
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int item_exist(Linked_list l, char *key){
  Linked_list aux = l;
  while (aux != NULL) {
    if (strcmp (aux -> key , key) == 0){
      return 0;
    } else {
    // Siguiente elemento
    aux = aux -> next;
    }
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int destroy (Linked_list *l){
  Linked_list aux;
  while (*l != NULL ){
    aux = *l;
    *l = aux -> next ;
    free (aux);
    list_size = 0;
  }
  return 0;
}
