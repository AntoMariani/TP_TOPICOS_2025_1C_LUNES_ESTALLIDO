#ifndef INTERFAZ_H_INCLUDED
#define INTERFAZ_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "escalado.h"
#include "juego.h"
#include "dibujos.h"
#include "estadisticas.h"

void inicializarSDLTtf();
void inicializarSDLImagenes();
void inicializarSDL();
void inicializarSDLCompleto();

int crearVentana(SDL_Window** ventana, int width,  int height);
int crearRenderer(SDL_Renderer** renderer,SDL_Window** ventana);
void inicializarVentanaYRenderer(SDL_Window** ventana, SDL_Renderer** renderer);

void dibujarTablero(SDL_Renderer* renderer, Juego* juego, TTF_Font* fuente, TTF_Font* fuenteHUD,int clicksCheat, bool cheatEnUso,Uint32 cheatActivadoTiempo);

void dibujarLogo(SDL_Renderer* renderer, SDL_Window* ventana, int frameLogo);
void dibujarTexto(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, SDL_Rect rect, SDL_Color colorTexto);
void dibujarOverlayCheat(SDL_Renderer* renderer, Juego* juego);
void dibujarBotonPlano(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color colorFondo);
void obtenerRectBotonesHUD(SDL_Rect* cheat, Juego* juego);

opcionesMenuPrincipal mostrarMenuPrincipal(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font * fuente);
opcionesMenuTipoPartida mostrarMenuTipoPartida(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font * fuente);
opcionesMenuDificultad mostrarMenuDificultad(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font * fuente);
opcionesMenuNickname mostrarMenuNickname(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuente, char* nombreUsuario);
bool mostrarFlujoDeMenus(SDL_Renderer* renderer, SDL_Window* ventana,opcionesMenuPrincipal* opcionPrincipal,opcionesMenuTipoPartida* opcionTipo,opcionesMenuDificultad* dificultad,char nombreUsuario[MAX_NOMBRE]);

void mostrarPantallaFin(SDL_Renderer* renderer, Juego* juego, bool gano);

void liberarTodo(SDL_Renderer* renderer, SDL_Window* ventana);

#endif // INTERFAZ_H_INCLUDED
