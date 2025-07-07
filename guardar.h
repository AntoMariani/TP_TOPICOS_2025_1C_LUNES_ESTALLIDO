#include "defines.h"
#include <string.h>
#include <stdio.h>
#include "historial.h"

void guardarPartida(Juego * juego, const char * nombreUsuario, opcionesMenuDificultad dificultad);
bool cargarPartidaGuardada(Juego* juego, int indiceSlot, opcionesMenuDificultad *dificultad, char* nombreUsuario);