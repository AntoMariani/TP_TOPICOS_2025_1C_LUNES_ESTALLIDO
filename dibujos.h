#ifndef DIBUJOS_H_INCLUDED
#define DIBUJOS_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "defines.h"

extern SDL_Texture* logo_texturas[LOGO_FRAMES];
extern SDL_Texture* numeros_texturas[TOTAL_NUMEROS + 1][FRAMES_NUMEROS];
extern SDL_Texture* esferas_texturas[TOTAL_ESFERAS + 1][FRAMES_NUMEROS];
extern SDL_Texture* mina_explotada_texturas[FRAMES_MINA_EXPLOTADA];
extern SDL_Texture* rastreador_cheat_texturas[FRAMES_RASTREADOR];
extern TTF_Font* fuenteTexto;
extern TTF_Font* fuenteHUD;

extern SDL_Color colores[];

int cargarTodosLosRecursos(SDL_Renderer* renderer);
void liberarTodosLosRecursos();

void cargarFuentes();
void liberarFuentes();

int cargarAnimaciones(SDL_Renderer* renderer, SDL_Texture* texturas[][FRAMES_NUMEROS], int totalNumeros, int totalFrames, const char* formatoPath);
void liberarAnimaciones(SDL_Texture* texturas[][FRAMES_NUMEROS], int totalNumeros, int totalFrames);

#endif // DIBUJOS_H_INCLUDED
