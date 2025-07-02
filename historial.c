#include "historial.h"

#include <stdlib.h>
#include <string.h>

//Crea una matriz nueva y copia todas las casillas
Casilla** copiarTablero(Casilla** original, int dimension) {
    Casilla** copia = malloc(dimension * sizeof(Casilla*));

    for (int i = 0; i < dimension; i++) {
        copia[i] = malloc(dimension * sizeof(Casilla));
        memcpy(copia[i], original[i], dimension * sizeof(Casilla));
    }

    return copia;
}

//Libera un tablero completo
void liberarTablero(Casilla** tablero, int dimension) {
    for (int i = 0; i < dimension; i++) {
        free(tablero[i]);
    }
    free(tablero);
}

//Inicializa el historial
void inicializarHistorialFotosTablero(HistorialFotosTablero* hs) {
    hs->lista = NULL;
    hs->cantidad = 0;
    hs->posActual = 0;
}

//Guarda el estado actual del juego como snapshot
void guardarFotoTablero(HistorialFotosTablero* hs, Juego* juego) {

    printf("DEBUG guardarFotoTablero: hs=%p, juego=%p, tablero=%p, dimension=%d\n",
       hs, juego, juego ? juego->tablero : NULL, juego ? juego->dimension : -1);


    if (!hs || !juego || !juego->tablero || juego->dimension <= 0) {
        printf("ERROR: Estado inválido al guardar snapshot\n");
        return;
    }

    //Si hicimos undo y ahora hacemos un nuevo movimiento, eliminamos el "futuro"
    if (hs->posActual < hs->cantidad) {
        for (int i = hs->posActual; i < hs->cantidad; i++) {
            if (hs->lista[i].tablero) {
                liberarTablero(hs->lista[i].tablero, juego->dimension);
                hs->lista[i].tablero = NULL;
            }
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

    hs->lista[hs->posActual] = s;
    hs->posActual++;
    hs->cantidad++;

    printf("Snapshot guardado. posActual=%d, cantidad=%d\n", hs->posActual, hs->cantidad);
}



//Restaura el estado anterior del juego
bool deshacerFotoTablero(HistorialFotosTablero* hs, Juego* juego) {
    if (hs->lista == NULL || hs->cantidad == 0 || hs->posActual <= 1)
        return false;

    // Retrocedemos 1 posición
    hs->posActual--;

    FotoTablero* s = &hs->lista[hs->posActual - 1];

    liberarTablero(juego->tablero, juego->dimension);
    juego->tablero = copiarTablero(s->tablero, juego->dimension);

    juego->minasMarcadas = s->minasMarcadas;

    printf("Deshacer: posActual ahora=%d, cantidad=%d\n", hs->posActual, hs->cantidad);
    return true;
}




//Aplica el siguiente estado si existe
bool rehacerFotoTablero(HistorialFotosTablero* hs, Juego* juego) {
    printf("Click en botón Rehacer\n");

    if (!hs || hs->lista == NULL || hs->posActual >= hs->cantidad)
        return false;

    FotoTablero* s = &hs->lista[hs->posActual];

    liberarTablero(juego->tablero, juego->dimension);
    juego->tablero = copiarTablero(s->tablero, juego->dimension);

    juego->minasMarcadas = s->minasMarcadas;

    printf("Rehacer: posActual ahora=%d, cantidad=%d\n", hs->posActual + 1, hs->cantidad);
    hs->posActual++;

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

