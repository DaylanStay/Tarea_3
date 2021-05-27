#ifndef List_H
#define List_H

#include <stdio.h>
#include <string.h>

typedef struct List List;

List *createList();

void *first(List *L);
void *next(List *L);
void *last(List *L);
void *prev(List *L);

void pushFront(List *, void *);
void pushBack(List *, void *);
void pushCurrent(List *, void *);

void *popFront(List *L);
void *popBack(List *L);
void *popCurrent(List *L);
void clean(List *L);

#endif

