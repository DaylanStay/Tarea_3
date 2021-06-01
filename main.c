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

const char *get_csv_field (char *, int);
HashMap* importar(FILE *, int *);
void distanciaEntregas(HashMap *, int);
void entregasCercanas (HashMap *);
void crearRuta(HashMap *, int, List *);
void crearRutaAleatoria(HashMap *, int, List *);
float calcularDistancia(int, int, int, int);
int comparar(const void *, const void *);
void mejorRuta(List *);
void mostrarRutas(List *);

int main(){

    FILE *archivo = fopen("tarea3_tsp.txt", "r");
    if(archivo == NULL) return 1;
    int *n;
    HashMap *mapCord = importar(archivo, n);
    List *listaRutas = createList();

    distanciaEntregas(mapCord,n);
    entregasCercanas(mapCord);
    crearRuta(mapCord, n, listaRutas);
    crearRutaAleatoria(mapCord, n, listaRutas);
    //mejorRuta(listaRutas);
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
        scanf("%i", &n);
        if(n > 48 || n < 1) printf("numero invalido\n");
        if(n == 0) printf("no hay ciudad para entregar el pedido\n");
    }while(n > 48 || n < 1);

    ciudad *cord = (ciudad*) malloc (sizeof(ciudad) * n);
    int aux = n * 0.3;
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
        if(i == n) return mapCord;
    }
}
void distanciaEntregas(HashMap *mapCord , int n){ // Opcion 2

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

void crearRuta(HashMap *mapCord, int n, List* listaRutas) { // Opcion 4
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

    printf("ruta generada: ");
    ciudad* a = last(rutaCreada);
    ciudad* d = first(rutaCreada);
    while(d != NULL) {
        if(a->id == d->id) break; // para estetica con el printf
        printf("%i -> ", d->id);
        d = next(rutaCreada);
    }
    printf("%i\n", d->id);


    printf("Ingrese el nombre de la ruta: \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta);
    fflush(stdin);
    printf("Distancia total recorrida de la ruta %s es %f\n", nombreRuta, distanciaTotal);

    ruta *s = (ruta *) malloc(sizeof(ruta)*1);
    s->distancia = distanciaTotal;
    strcpy(&s->nombre, nombreRuta);
    pushBack(listaRutas, s);
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
    int a = last(rutaCreada);
    int d = first(rutaCreada);
    while(d != NULL) {
        if(a == d) break; // para estetica con el printf
        printf("%i -> ", d);
        d = next(rutaCreada);
    }
    printf("%i\n", d);
    printf("Ingrese el nombre de la ruta: \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta);
    fflush(stdin);
    printf("Distancia total recorrida de la ruta %s es %f\n", nombreRuta, distanciaTotal);

    ruta *s = (ruta *) malloc(sizeof(ruta)*1);
    s->distancia = distanciaTotal;
    strcpy(&s->nombre, nombreRuta);
    pushBack(listaRutas, s);
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

/*void mejorRuta(){

}*/

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
