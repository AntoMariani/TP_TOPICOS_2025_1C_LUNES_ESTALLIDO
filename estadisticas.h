#ifndef ESTADISTICAS_H_INCLUDED
#define ESTADISTICAS_H_INCLUDED

#include "interfaz.h"
#include "juego.h"
#include "escalado.h"

typedef struct {
    char usuario[MAX_NOMBRE];
    char dificultad[MAX_DIFICULTAD];
    int tiempoSegundos;
} Estadistica;

typedef struct {
    char nombre[32];
    int tiempo;
} EntradaRanking;

int leerEstadisticas(Estadistica stats[], int maxStats);
void actualizarEstadisticas(const char* dificultad, const char* nombre, int tiempo);
void mostrarEstadisticas(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font * fuenteTexto, TTF_Font * fuenteHUD);

#endif // ESTADISTICAS_H_INCLUDED
