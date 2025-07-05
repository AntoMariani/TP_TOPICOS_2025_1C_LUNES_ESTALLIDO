#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "defines.h"
#include "escalado.h"
#include <SDL2/SDL.h>
#include "interfaz.h"
#include "historial.h"

Casilla** crearTablero(int dimension);
void liberarTablero(Casilla** tablero, int dimension);
void inicializarJuego(Juego* juego, const char* archivoConfiguracion);
int calcularTamCasilla(int dimension);
int obtenerDimensionPorDificultad(opcionesMenuDificultad dificultad);
float obtenerPorcentajeMinas(opcionesMenuDificultad dificultad);
void llenar(Juego* juego,int filaInicial, int colInicial);
void llenarElRestoDeMinas(int totalMinasALlenar, int dimension, Juego * juego);
void calcularMinasVecinas(Juego* juego);
void liberarJuego(Juego* juego);
void revelarCasillaSinMina(Juego* juego, int fila, int col);
int obtenerMaximoUsosCheat(opcionesMenuDificultad dificultad);
bool ganoLaPartida(Juego* juego);
void ejecutarPartida(SDL_Renderer* renderer, SDL_Window* ventana, opcionesMenuDificultad dificultad, const char* nombreUsuario);
void ejecutarLoopDeJuego(SDL_Renderer* renderer, SDL_Window* ventana, Juego* juego, opcionesMenuDificultad dificultad, const char* nombreUsuario);
void reiniciarPartida(Juego* juego, opcionesMenuDificultad dificultad, SDL_Window * ventana);

#endif // JUEGO_H_INCLUDED
