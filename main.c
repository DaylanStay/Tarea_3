#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include "list.h"
#include "hashmap.h"
#include "treemap.h"

typedef struct{
    int id;
    int x;
    int y;
    bool marca;
    float recorrido;
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

void menu(FILE *);
const char *get_csv_field (char *, int);
HashMap* importar(FILE *, int *);
void distanciaEntregas(HashMap *, int);
void entregasCercanas (HashMap *, int);
void crearRuta(HashMap *, int*, List *);
void crearRutaAleatoria(HashMap *, int, List *);
float calcularDistancia(int, int, int, int);
int comparar(const void *, const void *);
void mejorarRuta(List *, int *, HashMap *);
void manual(List *, int *, HashMap*, ruta*);
void automatico(List *, int, HashMap*, ruta*);
void upgrade(int, int, int, ruta *, HashMap*);
void mostrarRutas(List *);
void mejorRuta(HashMap *, int, List *);
void liberar(HashMap *, List *);

int main(){
    FILE *archivo = fopen("tarea3_tsp.txt", "r"); // Se crea una variable FILE y se abre el archivo
    if(archivo == NULL) return 1; // Si no tiene nada la variable se retorna 1
    menu(archivo); // Se envia la variable a la funcion MENU
    return 0;
}

void menu(FILE *archivo){ // Funcion para la creacion de menu menu
    int *n; // Tambien se crea una variable int que se encarga de leer las lineas necesarias del archivo (esta depende del numero ingresado por el usuario)
    List *listaRutas = createList(); // Se crea una lista para guardar todas las rutas que se vayan creando
    HashMap *mapCord; // Y un HashMap para almacenar todos los ratos de las rutas mediante una clave
    int a = 1; // Se hace una variable entera igualada a 1, para que salga del menu si la variable es 0
    while(a != 0){
         printf("Menu\n");
         printf("1. Importar\n");
         printf("2. Distancia entre entregas\n");
         printf("3. Mostrar entregas cercanas\n");
         printf("4. Crear Ruta\n");
         printf("5. Generar Ruta\n");
         printf("6. Mejorar ruta\n");
         printf("7. Mostrar rutas\n");
         printf("8. Mejor ruta\n");
         printf("0. Salir\n");
         fflush(stdin);
         scanf("%d",&a);
         fflush(stdin);
       switch(a){
            case 1: mapCord = importar(archivo, &n);break;
            case 2: distanciaEntregas(mapCord ,n); break;
            case 3: entregasCercanas(mapCord, n); break;
            case 4: crearRuta(mapCord, n, listaRutas); break;
            case 5: crearRutaAleatoria(mapCord, n, listaRutas); break;
            case 6: mejorarRuta(listaRutas, n, mapCord); break;
            case 7: mostrarRutas(listaRutas); break;
            case 8: mejorRuta(mapCord, n, listaRutas); break;
            case 0: liberar(mapCord, listaRutas); break;
            default: printf("\nEl numero que ingreso es incorrecto\n"); break;
        }
        printf("\n");
     }
}

HashMap* importar(FILE *archivo , int *n) { // Opcion 1
    /* En esta opcion el usuario ingresara cuantas entregas desea realizar, dicha cantidad se quedara en la variable n definida anteriormente en el menu
    esta variable n no dejara al usuario seguir con la importacion si es menor o igual a 0 y si es mayor que 48 */

    printf("ingrese el numero de entregas a realizar (no puede ser mayor que 48)\n");
    do {
        scanf("%i", n);
        if(*n > 48 || *n < 1) printf("Numero invalido\n");
        if(*n == 0){
          printf("No hay ciudad para entregar el pedido\n");
          exit(1);
        }
    }while(*n > 48 || *n < 1);

    // Se crea una variable ciudad, esta variable ciudad es una estructura con todos los datos necesarios para leer el archivo, como las coordenadas x e y, la i y un booleano
    ciudad *cord = (ciudad*) malloc (sizeof(ciudad) * *n);
    int aux = *n + *n * 0.3; // Este auxiliar es para darle el espacio necesario a la Tabla Hash
    HashMap *mapCord = createMap(aux); // Se crea el mapa que tendra todos los datos leidos del archivo

    int line;
    int i = 0;
    int line2;
    /* En este while se va a leer el archivo y ira importando sus datos a la variable ciudad creada anteriormente */
    while(fscanf(archivo,"%i %d ",&line,&line2)!= EOF){
        cord[i].x = line;
        cord[i].y = line2;
        cord[i].id = i+1; // Se le agrega su ID empezando desde 1
        cord[i].marca = false;
        insertMap(mapCord, &cord[i].id, &cord[i]); // Inmediatamente se va insertando en el mapa
        i++;
        if(i == *n){ // Si la i es igual a la n significa que ya se leyeron las entregas que queria el usuario
            fclose(archivo); // Se cierra el archivo
            return mapCord; // Y se retorna el mapa con los datos
        }
    }
}

void distanciaEntregas(HashMap *mapCord , int n){ // Opcion 2
    /* En esta funcion se calculara las distancias de las entregas que desea el usuario, la distancia se sacara mediante la funcion
    CalcularDistancia que sera explicara posteriormente, se hara una variable auxiliar de tipo entero que sera la que lea las entregas
    ingresados por el usuario, luego se buscara si la entrega esta en el tabla hash. Despues si las entregas estan en la tabla hash se
    usara la funcion calcularDistancia y se mostrara en pantalla la distancia entre las dos entregas */

    int aux;
    ciudad *entrega1;
    ciudad *entrega2;
    float distancia = 0;
    printf("Tiene disponible %i entregas para calcular \n",n);

    /* Aqui se llama y se recorre el mapa para mostrarle al usuario con mayor claridad cuales son las entregas que tiene con sus
    respectivas cordenadas e id */

    ciudad *p = firstMap(mapCord);

    while(p != NULL){
        printf("Entrega %i con coordenadas %i y %i \n", p->id,p->x,p->y);
        p = nextMap(mapCord);
    }

    printf("\nIngrese la ID de las entregas que le desea calcular la distancia \n");
    printf("Ingrese primera ID: ");
    scanf("%i", &aux);
    entrega1 = searchMap(mapCord, &aux); // Se busca en el mapa si esque esta la entrega ingresada por el usuario
    printf("Ingrese segunda ID: ");
    scanf("%i", &aux);
    entrega2 = searchMap(mapCord, &aux); // Se hace la misma operacion que arriba
    distancia = calcularDistancia(entrega1->x,entrega1->y,entrega2->x,entrega2->y); // Se usa la funcion calcularDistancia para obtener el valor de las distancia entre las dos entregas escogidas
    printf("\nEsta es la distancia entre las dos entregas elegidas: %f \n", distancia);
}

int lower_than_int(void* key1, void* key2){ //Funcion utilizada para crear arbol binario
    int k1 = *((int*) (key1));
    int k2 = *((int*) (key2));
    return k1<k2;
}

void entregasCercanas(HashMap *mapCord, int n){ // Opcion 3
    /*Esta funcion mostrara las 3 primeras entregas cercana */

    int x, y;
    printf("Ingrese coordenada X\n");
    scanf("%i", &x);
    printf("Ingrese coordenada Y\n");
    scanf("%i", &y);
    TreeMap *ordenar = createTreeMap(lower_than_int); // Se crea un mapa ordenado para almacenar las distancias para luego mostrarlas ordenadas de menor a mayor
    ciudad *p =firstMap(mapCord); // Aqui se va recorriendo el mapa
    while(p != NULL){
        if(p->marca == false) { // Si la marca es falso significa que no se ha pasado por ella todavia por lo que entraria al ciclo
            float aux = calcularDistancia(x, y, p->x, p->y); // Se calcula la distancia otra vez usando la funcion correspondiente
            p->recorrido = aux; // Se le asigna la distancia a la variable p
            insertTreeMap(ordenar, &p->recorrido, p); // Y se inserta en el mapa con la del recorrido y el valor de la p
        }
        p = nextMap(mapCord); // Se sigue recorriendo el mapa hasta que se complete
    }

    p = firstTreeMap(ordenar); // Se llama al mapa ordenado y se empieza a recorrer
    for(int i = 0 ; i < n ; i++) {
        if(i == 3) break; // Cuando ya se muestren las 3 entregas, el programa rompera su ciclo
        printf("Distancia entre su ubicacion y la ciudad %i = %.2f \n", p->id, p->recorrido);
        p = nextTreeMap(ordenar);
    }
    free(ordenar); // Se libera memoria
}

void crearRuta(HashMap *mapCord, int *n, List* listaRutas) { // Opcion 4
    /* Funcion donde el usuario ingresara sus coordenadas y ira escogiendo que entrega quiere realizar primero, asi hasta completar toda la ruta de entrega*/

    int x, y;
    int id = 0;
    printf("Ingrese coordenada X\n");
    scanf("%i", &x);
    printf("Ingrese coordenada Y\n");
    scanf("%i", &y);
    int cont = 0; // Cuando el contador sea igual a la n se detendra el ciclo y se dejara de escoger entregas
    ciudad *p;

    float distanciaTotal = 0;
    List *rutaCreada = createList(); // Se crea una lista para almacenar las rutas creadas
    while(cont < n) { // En este while se estarian generando nuestros nodos adyacentes
        TreeMap *orden = createTreeMap(lower_than_int); // Al igual que en la anterior funcion se crea un mapa ordenado para que vaya mostrando las distancia de menor a mayor
        p = firstMap(mapCord); // Se empieza a recorrer el mapa hasta el final
        while(p != NULL){
            if(p->marca == false) { // Si la marca es falsa significa que todavia no se calcula la distancia de dicha entrega
                float aux = calcularDistancia(x, y, p->x, p->y); // Se usa la funcion para calcular la distancia
                p->recorrido = aux; // Se le asigna a la variable p la distancia conseguida por la función
                insertTreeMap(orden, &p->recorrido, p); // Se inserta en el mapa ordenado con la distancia como clave
            }
            p = nextMap(mapCord);
        }
        p = firstTreeMap(orden); // Se recorre el mapa ordenado
        while(p != NULL){
            if(p->marca == false) { // Si es false significa que todavia no se pasa por dicha entrega
                printf("Distancia entre su ubicacion y la ciudad %i = %.2f \n", p->id, p->recorrido);
            }

            p= nextTreeMap(orden);
        }

        free(orden);

        printf("Seleccione el numero de la ciudad que desea ir: \n");
        scanf("%i", &id);
        p = searchMap(mapCord, &id); // Aqui se busca el numero de la ciudad que haya ingresado el usuario
        while(p->marca == true) { // Como se va a pasar por esa entrega se marca como verdadera para no mostrarla mas dentro de las opciones
            scanf("%i", &id);
            p = searchMap(mapCord, &id); // Se busca en el mapa para luego insertar la id en una lista
        }
        pushBack(rutaCreada, p); // Se inserta la variable p en la lista, para luego mostrar la ruta completada mostrando todo el recorrido mediante las id
        distanciaTotal += calcularDistancia(x, y, p->x, p->y); // Se va calculando la distancia con la entrega que se escogio, asi sucesivamente hasta que haya terminado la ruta de entregas
        x = p->x; // se modifican las coordenadas iniciales ya que completo una entrega
        y = p->y;
        p->marca = true; // Se marca como verdadera la variable ya que fue visitada y entregada
        eraseMap(mapCord, &id); // Se borra del mapa la id
        insertMap(mapCord, &p->id, p); // Y se inserta nuevamente con la nueva id
        p = searchMap(mapCord, &p->id); // Se busca en el mapa la id nueva
        cont++;
    }

    // Aqui se recorre el mapa y todas las marcas vuelve a hacer falsas
    for(int k = 1 ; k <= n ; k++) {
        p = searchMap(mapCord, &k);
        p->marca = false;
    }

    ruta *s = crearTrayecto(n); // Se crea una variable ruta que tendra los valores del trayecto recorrido

    printf("Ruta generada: ");
    ciudad* a = last(rutaCreada);
    ciudad* d = first(rutaCreada);
    int i=0;
    while(d != NULL) {
        if(a->id == d->id) break; // para estetica con el printf
        printf("%i -> ", d->id);
        s->trayecto[i] = d->id; // Se le va asignando al trayecto la id en el orden que fueron escogidas por el usuario
        d = next(rutaCreada);
        i++;
    }
    printf("%i\n", d->id); // Mayor estetica a la hora de mostrar en pantalla
    s->trayecto[i] = d->id;

    printf("Ingrese el nombre de la ruta: \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta); // Aqui se le pone un nombre a la ruta
    fflush(stdin);
    printf("salio \n");
    printf("Distancia total recorrida de la ruta %s es %f\n", nombreRuta, distanciaTotal); // Muestra el nombre y la distancia total recorrida

    // Aqui se copia el nombre y la distancia en la variable ruta para luego insertarla en la lista de rutas (donde estaran todas las rutas que fueron creadas)
    s->distancia = distanciaTotal;
    strcpy(&s->nombre, nombreRuta);
    pushBack(listaRutas, s); // se inserta a la lista de rutas
    free(rutaCreada); // se libera la lista ruta creada
}

void crearRutaAleatoria(HashMap *mapCord, int n, List * listaRutas){ // Opcion 5
    /* Esta funcion cumple el mismo objetivo que el crear ruta, pero estas no son escogidas por el usuario sino por la computadora*/

    int x, y;
    int id = 0;
    printf("Ingrese coordenada X\n");
    scanf("%i", &x);
    printf("Ingrese coordenada Y\n");
    scanf("%i", &y);
    int cont = 0;
    ciudad *p;
    float distanciaTotal = 0;
    List *rutaCreada = createList();
    HashMap *mapRand = createMap(n + 20); // Se crea un mapa para almacenar la id que el programa arroja
    /* En este ciclo while va haciendo el mismo proceso que en la anterior funcion pero ahora aleatoriamente se van escogiendo las entregas */

    while(cont < n) {
        srand(time(NULL));
        id = rand () % n + 1;
        while(searchMap(mapRand, &id) != NULL) { // Este while es para que no se repita la id aleatoriamente
            id = rand () % n + 1;
        }
        /* Se realiza el mismo proceso que en la funcion crearRuta */

        insertMap(mapRand, &id, id);
        if(cont == 0) pushFront(rutaCreada, id);
        else pushBack(rutaCreada, id);
        p = searchMap(mapCord, &id);
        distanciaTotal += calcularDistancia(x, y, p->x, p->y);
        // Se actualizan las coordenadas
        x = p->x;
        y = p->y;
        cont++;
    }

    /* Aqui se hace el mismo proceso que en la anterior funcion, crear una variable ruta y darle los valores correspondiente para
    luego insertarla en la lista de rutas */

    printf("Ruta generada: ");
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
    s->distancia = distanciaTotal;
    strcpy(&s->nombre, nombreRuta);
    pushBack(listaRutas, s);

    // Se liberan la lista y el mapa usado
    free(rutaCreada);
    free(mapRand);
}

float calcularDistancia(int x, int y, int X, int Y) { // Funcion para calcular la distancia con la formula matematica entre dos puntos
    float distancia;
    int resultadoX = abs(x - X);
    resultadoX = pow(resultadoX, 2);
    int resultadoY = abs(y - Y);
    resultadoY = pow(resultadoY, 2);
    distancia = sqrt(resultadoX + resultadoY);
    return distancia;
}

void mejorarRuta(List * listaRutas, int *n, HashMap * mapCord){ // Aqui se van mejorando las rutas que ya fueron creadas anteriormente
    printf("RUTAS ALMACENADAS: \n");
    // Aca se van mostrando los nombres de las rutas que fueron creadas aleatoriamente
    ruta *p = first(listaRutas);
    while(p!=NULL){
        printf("%s\n",p->nombre);
        p = next(listaRutas);
    }

    printf("Ingrese el nombre de la ruta a mejorar \n");
    char *nombreRuta = (char *) malloc(sizeof(char) * 20);
    fflush(stdin);
    scanf("%[^\n]s", nombreRuta);
    fflush(stdin);

    p = first(listaRutas);
    while(p!=NULL){
        if(strcmp(nombreRuta, p->nombre) == 0) break;
        p = next(listaRutas);
    }

    // Se pregunta si el usuario lo quiere hacer automaticamente o manualmente
    int aux;
    printf("Desea mejorar la ruta automaticamente o manualmente, si lo desea automaticamente presione 1. Si no, presione 0\n");
    scanf("%i",&aux);

    // Si presiono automaticamente entrara en este if y a la funcion automatico
    if(aux == 1){
        printf("AUTOMATICO\n");
        automatico(listaRutas, n, mapCord, p);
    }
    // Si presiona manualmente entrara a este if y a la funcion manual
    if(aux == 0){
        printf("MANUAL\n");
        manual(listaRutas, n, mapCord, p);
    }

}

void manual(List * listaRutas, int* n, HashMap* mapCord, ruta *p){
    // En esta funcion se va a modificar la ruta manualmente
    int a, b;
    int aux;
    ciudad* entrega1;
    ciudad* entrega2;

    // Se mostrara como fue realizada el trayecto de la ruta
    for(int i = 0 ; i < n ; i++){
        printf("%i -> ", p->trayecto[i]);
        aux = p->trayecto[i];
    }

    // El usuario escogera las entregas a intercambar para ver si el trayecto de su ruta mejora o no
    printf("SELECCIONE DOS ENTREGAS\n");
    printf("Ingrese primera entrega: ");
    scanf("%i", &a);
    printf("Ingrese segunda entrega: ");
    scanf("%i", &b);

    upgrade(a, b, n, p, mapCord); // se manda todo a esta funcion
}

void automatico(List * listaRutas, int n, HashMap* mapCord, ruta *p){
    /* Se realiza exactamente lo mismo que en la funcion manual pero esta de forma aleatoria con el random*/

    int a, b;
    ciudad* entrega1;
    ciudad* entrega2;

    srand(time(NULL));
    a = rand () % n + 1;
    b = a;
    while(b == a) { // Este while hara que no se repita la misma entrega
        srand(time(NULL));
        b = rand () % n + 1;
    }
    printf("%d %d\n", a, b);

    upgrade(a, b, n, p, mapCord);
}

void upgrade(int a, int b, int n, ruta *p, HashMap* mapCord) {
    /* En esta funcion se hara el intercambio para ver si la ruta mejora o no */

    // En este for ocurre el intercambio de posicion entre las entregas seleccionadas
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

    // Aqui se va a buscar en el mapa la trayectoria para sacar la nueva distancia tras el intercambio de la trayectoria
    int i=0, x = 0, y =0;
    ciudad* temp;
    float distancia = 0;
    temp = searchMap(mapCord, &p->trayecto[i]);
    x = temp->x;
    y = temp->y;
    for(i = 1 ; i < n ; i++){
        // Aqui se va calculando las distancias y se van actualizando las coordenadas.
        temp = searchMap(mapCord, &p->trayecto[i]);
        distancia += calcularDistancia(temp->x,temp->y,x,y);
        x = temp->x;
        y = temp->y;
    }

    // Se mostrara la distancia original y la nueva distancia
    printf("Esta es la primera distancia: %f\n", p->distancia);
    printf("Esta es la nueva distancia : %f\n", distancia);

    // Si la nueva distancia es menor que la distancia original se cambiara la original por la nueva, si pasa lo contrario que la original es menor que la nueva, se mantedra la distancia original y ruta original.

    if(p->distancia == distancia){
        printf("LA DISTANCIA NUEVA ES IGUAL A LA ANTERIOR\n");
    }

    if(p->distancia > distancia){
        p->distancia = distancia;
        printf("RUTA ACTUALIZADA\n");
    }

    if(p->distancia < distancia){
        printf("LA RUTA EMPEORO, SE VUELVE A LA RUTA ORIGINAL\n");
    }
}

int comparar(const void *pivote, const void *item){ //Este qsort nos sirve para ordenar por orden las distancia totales
   ruta *ptrPivote = (ruta *) pivote;
   ruta *ptrItem = (ruta *) item;
   if(ptrPivote->distancia > ptrItem ->distancia) return 1;
   return 0;
}

void mostrarRutas(List *listaRutas){ // Opcion 7
    /* En esta funcion se mostraran todas las rutas creadas con su nombre y distancia total, esto se hara de la siguiente forma
    se pasa los valores de la lista a un arreglo para luego enviar dicho arreglo a la funcion qsort para que se ordene de menor a mayor
    cuando pase por la funcion qsort esta mostrara por pantalla el nombre y la distancia de la ruta de menor a mayor */

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

void mejorRuta(HashMap *mapCord, int n, List *listaRutas){
    int x, y, cont = 0;
    int id = 0;
    printf("Ingrese coordenada X\n");
    scanf("%i", &x);
    printf("Ingrese coordenada Y\n");
    scanf("%i", &y);
    ciudad *p;
    List *rutaCreada = createList(); // Se crea una lista para almacenar la nueva ruta
    float aux = 0, distanciaTotal = 0;

    while(cont < n) {
        p = firstMap(mapCord);
        while(p != NULL){
            if(p->marca == false) { // Al igual que antes si la marca es falsa significa que todavia no se pasa por dicha entrega
                if(aux > calcularDistancia(x, y, p->x, p->y) || aux == 0) { // Si el aux es mayor o igual a 0 entrara al if, esto servira para obtener la distancia mas pequeña entre las entregas
                    aux = calcularDistancia(x, y, p->x, p->y); // Se van actualizando la distancia entre las entregas
                    id = p->id; // Se actualiza la id, cuando no entre al if estara sera la entrega mas corta
                }
            }
            p = nextMap(mapCord);
        }
        distanciaTotal += aux; // Cada vez que se encuentre la distancia mas corta se ira sumando a la distancia total

        aux = 0;
        p = searchMap(mapCord, &id); // Se busca la id de la distancia mas corta
        pushBack(rutaCreada, p); // Se inserta en la lista la distancia mas corta
        // Se van actualizando las coordenadas de la ubicacion de las entregas
        x = p->x;
        y = p->y;
        p->marca = true; // Se marca como verdadero debido a que ya se paso por dicha entrega
        eraseMap(mapCord, &id);
        insertMap(mapCord, &p->id, p);
        cont++;
    }
    // Se vuelven a marcar como false las entregas
    for(int k = 1 ; k <= n ; k++) {
        p = searchMap(mapCord, &k);
        p->marca = false;
    }

    // Se vuelve a realizar el mismo proceso que en el crear ruta para guardar la ruta generada en la nueva lista de rutas
    printf("Ruta generada: ");
    ruta *s = crearTrayecto(n);
    strcpy(&s->nombre, "Ruta optima");
    s->distancia = distanciaTotal;
    int i = 0;

    ciudad* a = last(rutaCreada);
    p = first(rutaCreada);
    while(p != NULL) {
        if(a->id == p->id) break; // para estetica con el printf
        printf("%i -> ", p->id);
        s->trayecto[i] = p->id;
        p = next(rutaCreada);
        i++;
    }
    printf("%i\n", p->id);
    s->trayecto[i] = p->id;
    printf("Distancia total recorrida de la %s es %f\n", s->nombre, distanciaTotal);
    pushBack(listaRutas, s);

    free(rutaCreada); // Se libera la lista de la ruta creada
}

void liberar(HashMap *mapCord, List *listaRutas) { // Funcion para liberar todo luego de cerrar el menu
    free(mapCord);
    free(listaRutas);
    exit(EXIT_SUCCESS);
}
