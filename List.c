#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

typedef struct Node Node;

struct Node {
    void *data;
    Node *next;
    Node *prev;
};

typedef struct List List;

struct List {
    Node *head;
    Node *tail;
    Node *current;
};

Node *createNode(const void * data) {
    Node *nuevoNodo = (Node *) malloc(sizeof(Node));
    if(nuevoNodo == NULL) exit(EXIT_FAILURE);
    nuevoNodo->data = data;
    nuevoNodo->prev = NULL;
    nuevoNodo->next = NULL;
    return nuevoNodo;
}

List *createList() {
    List *list = (List *) malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    return list;
}

void *first(List *list){
    if(list->head && list->head->data){
    list->current=list->head;
    }
    return (void *)list->current->data;
}

void *next(List * list) {

  if(list->current == NULL) return NULL;
  if(list->current->next == NULL) return NULL;
  list->current = list->current->next;
  return (void *)list->current->data;

}

void *last(List *list){
    if(list->tail && list->tail->data) list->current =list->tail;
    return (void *)list->current->data;
}

void pushBack(List * list, void * data) {
    list->current = list->tail;
    if(list->current==NULL) pushFront(list,data);
    else pushCurrent(list,data);
}

void * prev(List * list) {

  if(list->current == NULL) return NULL;
  if(list->current->prev == NULL) return NULL;
  list->current = list->current->prev;
  return (void *)list->current->data;

}

void pushCurrent(List * list, void * data) {
  Node *n = createNode(data);
  if(list->current != NULL){
    list->current->next = n;
    n->prev = list->current;
    list->tail=n;
  }
}


void pushFront(List * list,  void * data) {
  Node* n = createNode(data);
  n->next = list->head;
  if(list->head != NULL) list->head->prev = n;
  list->tail = n;
  list->head = n;
  n->prev=NULL;
}

void * popFront(List * list) {
    list->current = list->head;
    return popCurrent(list);
}

void * popBack(List * list) {
    list->current = list->tail;
    return popCurrent(list);
}

void * popCurrent(List * list) {
    assert(list != NULL || list->head != NULL);

    if (list->current == NULL) return NULL;

    Node * aux = list->current;

    if (aux->next != NULL)
        aux->next->prev = aux->prev;


    if (aux->prev != NULL)
        aux->prev->next = aux->next;


    void * data = (void *)aux->data;

    if(list->current == list->tail)
        list->tail = list->current->prev;

    if(list->current == list->head)
        list->head = list->current->next;

    list->current = aux->prev;

    free(aux);

    return data;
}

