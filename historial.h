#ifndef HISTORIAL_H_INCLUDED
#define HISTORIAL_H_INCLUDED

#include <stdbool.h>
#include "defines.h"
#include "juego.h"

Casilla** copiarTablero(Casilla** original, int dimension);
void liberarTablero(Casilla** tablero, int dimension);
void inicializarHistorialFotosTablero(HistorialFotosTablero* hs);
void guardarFotoTablero(HistorialFotosTablero* hs, Juego* juego);
bool deshacerFotoTablero(HistorialFotosTablero* hs, Juego* juego, SDL_Window* ventana);
bool rehacerFotoTablero(HistorialFotosTablero* hs, Juego* juego, SDL_Window* ventana);
void liberarHistorialFotosTablero(HistorialFotosTablero* hs, int dimension);


#endif // HISTORIAL_H_INCLUDED
