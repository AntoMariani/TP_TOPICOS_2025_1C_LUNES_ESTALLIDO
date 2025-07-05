#include "historial.h"

#include <stdlib.h>
#include <string.h>

//Crea una matriz nueva y copia todas las casillas
Casilla** copiarTablero(Casilla** original, int dimension) {
    if (!original || dimension <= 0)
        return NULL;

    Casilla** copia = crearTablero(dimension);
    if (!copia)
        return NULL;

    for (int i = 0; i < dimension; i++) {
        memcpy(copia[i], original[i], dimension * sizeof(Casilla));
    }

    return copia;
}

//Inicializa el historial
void inicializarHistorialFotosTablero(HistorialFotosTablero* hs) {
    hs->lista = NULL;
    hs->cantidad = 0;
    hs->posActual = 0;
}

//Guarda el estado actual del juego como snapshot
void guardarFotoTablero(HistorialFotosTablero* hs, Juego* juego) {
    if (!hs || !juego || !juego->tablero || juego->dimension <= 0) {
        printf("ERROR: Estado inválido al guardar snapshot\n");
        return;
    }

    //Si hicimos undo y ahora hacemos un nuevo movimiento, eliminamos el "futuro"
    if (hs->posActual < hs->cantidad) {
        for (int i = hs->posActual; i < hs->cantidad; i++) {
            if (hs->lista[i].tablero) {
                liberarTablero(hs->lista[i].tablero, hs->lista[i].dimension);
                hs->lista[i].tablero = NULL;
            }
            hs->lista[i].dimension = 0;
        }
        hs->cantidad = hs->posActual;
    }

    //Redimensionamos el historial
    size_t nuevoTam = (hs->cantidad + 1) * sizeof(FotoTablero);
    FotoTablero* nuevo = realloc(hs->lista, nuevoTam);
    if (!nuevo) {
        printf("ERROR: realloc falló al ampliar historial\n");
        return;
    }
    hs->lista = nuevo;

    printf("Guardando snapshot: %d\n", hs->posActual);

    //Copiamos el estado del tablero
    FotoTablero s;
    s.tablero = copiarTablero(juego->tablero, juego->dimension);
    if (!s.tablero) {
        printf("ERROR: Falló la copia del tablero. No se guarda snapshot.\n");
        return;
    }

    s.minasMarcadas = juego->minasMarcadas;
    s.dimension = juego->dimension;
    s.totalMinas = juego->totalMinas;

    hs->lista[hs->posActual] = s;
    hs->posActual++;
    if (hs->posActual > hs->cantidad)
        hs->cantidad = hs->posActual;

    printf("Snapshot guardado. posActual=%d, cantidad=%d\n", hs->posActual, hs->cantidad);
}



//Restaura el estado anterior del juego
bool deshacerFotoTablero(HistorialFotosTablero* hs, Juego* juego, SDL_Window * ventana) {
    if (hs->lista == NULL || hs->cantidad == 0 || hs->posActual <= 1)
        return false;

    hs->posActual--;
    FotoTablero* s = &hs->lista[hs->posActual - 1];

    liberarTablero(juego->tablero, juego->dimension);
    juego->tablero = copiarTablero(s->tablero, s->dimension);

    juego->dimension = s->dimension;
    juego->tamCasilla = calcularTamCasilla(juego->dimension);
    juego->minasMarcadas = s->minasMarcadas;
    juego->totalMinas = s->totalMinas;

    printf("Deshacer: posActual ahora=%d, cantidad=%d\n", hs->posActual, hs->cantidad);
    return true;
}




//Aplica el siguiente estado si existe
bool rehacerFotoTablero(HistorialFotosTablero* hs, Juego* juego, SDL_Window * ventana) {
    printf("Click en botón Rehacer\n");

    if (!hs || hs->lista == NULL || hs->posActual >= hs->cantidad)
        return false;

    printf("Intentando rehacer en posActual=%d, cantidad=%d\n", hs->posActual, hs->cantidad);

    FotoTablero* s = &hs->lista[hs->posActual];

    liberarTablero(juego->tablero, juego->dimension);
    juego->tablero = copiarTablero(s->tablero, s->dimension);

    juego->dimension = s->dimension;
    juego->tamCasilla = calcularTamCasilla(juego->dimension);
    juego->minasMarcadas = s->minasMarcadas;
    juego->totalMinas = s->totalMinas;

    hs->posActual++;

    printf("Rehacer: posActual ahora=%d, cantidad=%d\n", hs->posActual, hs->cantidad);
    return true;
}





//Libera todo el historial completo
void liberarHistorialFotosTablero(HistorialFotosTablero* hs, int dimension) {
    if (hs == NULL || hs->lista == NULL || hs->cantidad == 0)
        return;

    for (int i = 0; i < hs->cantidad; i++) {
        Casilla** tablero = hs->lista[i].tablero;
        if (tablero != NULL) {
            for (int j = 0; j <  hs->lista[i].dimension; j++) {
                if (tablero[j] != NULL) {
                    free(tablero[j]);
                    tablero[j] = NULL;
                }
            }
            free(tablero);
            hs->lista[i].tablero = NULL;
        }
    }

    free(hs->lista);
    hs->lista = NULL;
    hs->cantidad = 0;
    hs->posActual = 0;
}

