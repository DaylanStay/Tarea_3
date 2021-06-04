#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include "list.h"
#include "hashmap.h"


typedef struct{
    int id;
    int x;
    int y;
    bool marca;
}ciudad;

typedef struct{
    float distancia;
    char nombre[20];
    int* trayecto;
}ruta;

ruta *crearTrayecto(n){
    ruta *aux = (ruta *) calloc(n,sizeof(ruta));
    aux->trayecto = (ruta *) malloc(sizeof(ruta) * n);
    return aux;
}

const char *get_csv_field (char *, int);
HashMap* importar(FILE *, int *);
void distanciaEntregas(HashMap *, int *);
void entregasCercanas (HashMap *);
void crearRuta(HashMap *, int*, List *);
void crearRutaAleatoria(HashMap *, int, List *);
float calcularDistancia(int, int, int, int);
int comparar(const void *, const void *);
void mejorRuta(List *, int *, HashMap *);
void manual(List *, int *, HashMap*, ruta*);
void mostrarRutas(List *);

int main(){

    FILE *archivo = fopen("tarea3_tsp.txt", "r");
    if(archivo == NULL) return 1;
    int *n;
    HashMap *mapCord = importar(archivo, &n);
    List *listaRutas = createList();
    // printf("caca %i caca\n", n);

    //distanciaEntregas(mapCord,n);
    //printf("caca %i caca\n", n);
    //entregasCercanas(mapCord);
    crearRuta(mapCord, n, listaRutas);
    crearRutaAleatoria(mapCord, n, listaRutas);
    mejorRuta(listaRutas, n, mapCord);
    mostrarRutas(listaRutas);

    /*ciudad *p = firstMap(mapCord);
    while(p != NULL){
        printf("%i = (%i %i) \n", p->id,p->x,p->y);
        p = nextMap(mapCord);
    }*/
    return 0;
}

HashMap* importar(FILE *archivo , int *n) { // Opcion 1

    printf("ingrese el numero de entregas a realizar (no puede ser mayor que 48)\n");
    do {
        scanf("%i", n);
        if(*n > 48 || *n < 1) printf("numero invalido\n");
        if(*n == 0){
          printf("no hay ciudad para entregar el pedido\n");
          exit(1);
        }
    }while(*n > 48 || *n < 1);

    ciudad *cord = (ciudad*) malloc (sizeof(ciudad) * *n);
    int aux = *n + *n * 0.3;
    HashMap *mapCord = createMap(aux);

    int line;
    int i = 0;
    int line2;
    while(fscanf(archivo,"%i %d ",&line,&line2)!= EOF){
        cord[i].x = line;
        cord[i].y = line2;
        cord[i].id = i+1;
        cord[i].marca = false;
        insertMap(mapCord, &cord[i].id, &cord[i]);
        i++;
        if(i == *n) return mapCord;
    }

}
void distanciaEntregas(HashMap *mapCord , int *n){ // Opcion 2

    int aux;
    ciudad *entrega1;
    ciudad *entrega2;
    float distancia = 0;
    printf("Tiene disponible %i entregas para calcular \n",n);
    ciudad *p = firstMap(mapCord);

    while(p != NULL){
        printf("Entrega %i con coordenadas %i y %i \n", p->id,p->x,p->y);
        p = nextMap(mapCord);
    }

    printf("\nIngrese la ID de las entregas que le desea calcular la distancia \n");
    printf("Ingrese primera ID: ");
    scanf("%i", &aux);
    entrega1 = searchMap(mapCord, &aux);
    printf("Ingrese segunda ID: ");
    scanf("%i", &aux);
    entrega2 = searchMap(mapCord, &aux);

    distancia = calcularDistancia(entrega1->x,entrega1->y,entrega2->x,entrega2->y);

    printf("\nEsta es la distancia entre las dos entregas elegidas: %f \n", distancia);
}

void entregasCercanas(HashMap *mapCord){ // Opcion 3
    int x, y;
    printf("ingrese coordenada X\n");
    scanf("%i", &x);
    printf("ingrese coordenada Y\n");
    scanf("%i", &y);

    ciudad *p =firstMap(mapCord);
    // int cont = 0;
    while(p != NULL){
        if(p->marca == false) {
            float aux = calcularDistancia(x, y, p->x, p->y);
            printf("distancia entre su ubicacion y la ciudad %i = %.2f \n", p->id, aux);
        }
        p = nextMap(mapCord);
    }

}

void crearRuta(HashMap *mapCord, int *n, List* listaRutas) { // Opcion 4
    printf("este es el valor de la n %i \n",n);
    int x, y;
    int id = 0;
    printf("ingrese coordenada X\n");
    scanf("%i", &x);
    printf("ingrese coordenada Y\n");
    scanf("%i", &y);
    int cont = 0;
    ciudad *p;
    float distanciaTotal = 0;
    List *rutaCreada = createList();

    while(cont < n) {
        p = firstMap(mapCord);
        while(p != NULL){
            if(p->marca == false) {
                float aux = calcularDistancia(x, y, p->x, p->y);
                printf("distancia entre su ubicacion y la ciudad %i = %.2f \n", p->id, aux);
            }
        p = nextMap(mapCord);
        }
        printf("seleccione el numero de la ciudad que desea ir: \n");
        scanf("%i", &id);
        p = searchMap(mapCord, &id);
        while(p->marca == true) {
            scanf("%i", &id);
            p = searchMap(mapCord, &id);
        }
        pushBack(rutaCreada, p);
        distanciaTotal += calcularDistancia(x, y, p->x, p->y);
        x = p->x;
        y = p->y;
        p->marca = true;
        eraseMap(mapCord, &id);
        insertMap(mapCord, &p->id, p);
        p = searchMap(mapCord, &p->id);
        cont++;
    }

    ruta *s = crearTrayecto(n);

    printf("ruta generada: ");
    ciudad* a = last(rutaCreada);
    ciudad* d = first(rutaCreada);
    int i=0;
    while(d != NULL) {
        if(a->id == d->id) break; // para estetica con el printf
        printf("%i -> ", d->id);
        s->trayecto[i] = d->id;
        d = next(rutaCreada);
        i++;
    }
    printf("%i\n", d->id);
    s->trayecto[i] = d->id;

    printf("Ingrese el nombre de la ruta: \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta);
    fflush(stdin);
    printf("Distancia total recorrida de la ruta %s es %f\n", nombreRuta, distanciaTotal);

    s->distancia = distanciaTotal;
    strcpy(&s->nombre, nombreRuta);
    pushBack(listaRutas, s);

    for(i = 0 ; i < n ; i++){
        printf("Estos son los valores de la trayectoria: %i \n",s->trayecto[i]);
    }
}

void crearRutaAleatoria(HashMap *mapCord, int n, List * listaRutas){ // Opcion 5
    int x, y;
    int id = 0;
    printf("ingrese coordenada X\n");
    scanf("%i", &x);
    printf("ingrese coordenada Y\n");
    scanf("%i", &y);
    int cont = 0;
    ciudad *p;
    float distanciaTotal = 0;
    List *rutaCreada = createList();
    HashMap *mapRand = createMap(n + 20);
    while(cont < n) {
        srand(time(NULL));
        id = rand () % n + 1;
        //p =
        while(searchMap(mapRand, &id) != NULL) {
            id = rand () % n + 1;
            //p = searchMap(mapRand, &id);
        }
        insertMap(mapRand, &id, id);
        if(cont == 0) pushFront(rutaCreada, id);
        else pushBack(rutaCreada, id);
        p = searchMap(mapCord, &id);
        distanciaTotal += calcularDistancia(x, y, p->x, p->y);
        x = p->x;
        y = p->y;
        cont++;
    }

    printf("ruta generada: ");
    ruta *s = crearTrayecto(n);
    int a = last(rutaCreada);
    int d = first(rutaCreada);
    int i=0;
    while(d != NULL) {
        if(a == d) break; // para estetica con el printf
        printf("%i -> ", d);
        s->trayecto[i] = d;
        d = next(rutaCreada);
        i++;
    }
    printf("%i\n", d);
    s->trayecto[i] = d;

    printf("Ingrese el nombre de la ruta: \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta);
    fflush(stdin);
    printf("Distancia total recorrida de la ruta %s es %f\n", nombreRuta, distanciaTotal);

//    ruta *s = (ruta *) malloc(sizeof(ruta)*1);
    s->distancia = distanciaTotal;
    strcpy(&s->nombre, nombreRuta);
    pushBack(listaRutas, s);

     for(i = 0 ; i < n ; i++){
        printf("Estos son los valores de la trayectoria: %i \n",s->trayecto[i]);
    }
}

float calcularDistancia(int x, int y, int X, int Y) {
    float distancia;
    int resultadoX = abs(x - X);
    resultadoX = pow(resultadoX, 2);
    int resultadoY = abs(y - Y);
    resultadoY = pow(resultadoY, 2);
    distancia = sqrt(resultadoX + resultadoY);
    return distancia;
}

void mejorRuta(List * listaRutas, int *n, HashMap * mapCord){
    printf("RUTAS ALMACENADAS: \n");
    ruta *p = first(listaRutas);
    while(p!=NULL){
        printf("%s\n",p->nombre);
        p = next(listaRutas);
    }

    printf("INGRESE EL NOMBRE DE LA RUTA A MEJORAR \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta);
    fflush(stdin);

    p = first(listaRutas);
    while(p!=NULL){
        if(strcmp(nombreRuta, p->nombre) == 0) break;
        p = next(listaRutas);
    }


    int aux;
    printf("DESEA MEJORAR RUTA AUTOMATICAMENTE O MANUALMENTE, SI LO DESEA AUTOMATICAMENTE PRESIONE 1 SINO PRESIONE 0\n");
    scanf("%i",&aux);

    if(aux == 1){
        printf("AUTOMATICO\n");

    }
    if(aux == 0){
        printf("MANUAL\n");
        manual(listaRutas, n, mapCord, p);
    }

}

void manual(List * listaRutas, int* n, HashMap* mapCord, ruta *p){
    int a, b;
    int aux;
    ciudad* entrega1;
    ciudad* entrega2;
    //List* entregas = createList();

    for(int i = 0 ; i < n ; i++){
        printf("%i -> ", p->trayecto[i]);
        aux = p->trayecto[i];
        //pushBack(entregas,aux);
    }

    printf("SELECCIONE DOS ENTREGAS\n");
    printf("Ingrese primera entrega: ");
    scanf("%i", &a);
    printf("Ingrese segunda entrega: ");
    scanf("%i", &b);

    for(int i=0 ; i < n ; i++){
        if(p->trayecto[i] == a){
            p->trayecto[i] =  b;
            i++;
        }

        if(p->trayecto[i] == b){
            p->trayecto[i] = a;
            break;
        }
    }

    int i=0, x = 0, y =0;
    ciudad* temp;
    float distancia = 0;
    temp = searchMap(mapCord, &p->trayecto[i]);
    x = temp->x;
    y = temp->y;
    for(i = 1 ; i < n ; i++){
        temp = searchMap(mapCord, &p->trayecto[i]);
        distancia += calcularDistancia(temp->x,temp->y,x,y);
        x = temp->x;
        y = temp->y;
    }

    printf("Esta es la primera distancia: %f\n", p->distancia);
    printf("esta es la nueva distancia : %f\n", distancia);

    if(p->distancia > distancia){
        p->distancia = distancia;
        printf("RUTA ACTUALIZADA");
    }

    if(p->distancia < distancia){
        printf("LA RUTA EMPEORO, SE VUELVE A LA RUTA ORIGINAL");
    }
}

int comparar(const void *pivote, const void *item){ //Este qsort nos sirve para ordenar por orden de pc
   ruta *ptrPivote = (ruta *) pivote;
   ruta *ptrItem = (ruta *) item;
   if(ptrPivote->distancia > ptrItem ->distancia) return 1;
   return 0;
}

void mostrarRutas(List *listaRutas){ // Opcion 7
    ruta *a = NULL;
    a = (ruta *) malloc(sizeof(ruta) * 49);
    if(a == NULL) exit(EXIT_FAILURE);

    int i = 0;
    ruta *c = first(listaRutas);
    while(c != NULL){
        a[i].distancia = c->distancia;
        strcpy(a[i].nombre, c->nombre);
        i = i + 1;
        c = next(listaRutas);
    }

    qsort(a,i,sizeof(ruta),comparar);
    int j;
    for(j = 0; j < i; j++){
        printf("| %s | %f |\n",a[j].nombre,a[j].distancia);
    }
}
