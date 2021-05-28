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

const char *get_csv_field (char *, int);
int importar(ciudad *, FILE *, HashMap *);
void crearRuta(HashMap *, int);
void crearRutaAleatoria(HashMap *, int);
float calcularDistancia(int, int, int, int);

int main()
{
    FILE *archivo = fopen("tarea3_tsp.txt", "r");
    if(archivo == NULL) return 1;
    ciudad *cord = (ciudad*) malloc (sizeof(ciudad) * 49);
    HashMap *mapCord = createMap(65);
    int n = importar(cord, archivo, mapCord);

    crearRuta(mapCord, n);
    crearRutaAleatoria(mapCord, n);


    /*ciudad *p = firstMap(mapCord);
    while(p != NULL){
        printf("%i = (%i %i) \n", p->id,p->x,p->y);
        p = nextMap(mapCord);
    }*/
    return 0;
}

int importar(ciudad *cord, FILE *archivo, HashMap *mapCord) {
    int n;
    printf("ingrese el numero de entregas a realizar (no puede ser mayor que 48)\n");
    do {
        scanf("%i", &n);
        if(n > 48 || n < 1) printf("numero invalido\n");
        if(n == 0) printf("no hay ciudad para entregar el pedido\n");
    }while(n > 48 || n < 1);
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
        if(i == n) return n;
    }
}

void crearRuta(HashMap *mapCord, int n) {
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
                printf("distancia entre ciudad %i y ciudad %i = %.2f \n", id, p->id, aux);
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
        if(cont == 0) pushFront(rutaCreada, p);
        else pushBack(rutaCreada, p);
        distanciaTotal += calcularDistancia(x, y, p->x, p->y);
        x = p->x;
        y = p->y;
        p->marca = true;
        eraseMap(mapCord, &id);
        insertMap(mapCord, &p->id, p);
        p = searchMap(mapCord, &p->id);
        cont++;
    }
    ciudad *d = first(rutaCreada);
    while(d != NULL) {
        printf("%i = (%i,%i)\n", d->id, d->x, d->y);
    d = next(rutaCreada);
    }
    printf("Ingrese el nombre de la ruta: \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta);
    fflush(stdin);
    printf("Distancia total recorrida de la ruta %s es %f\n", nombreRuta, distanciaTotal);
}

void crearRutaAleatoria(HashMap *mapCord, int n) {
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
        if(a == d) break;
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
