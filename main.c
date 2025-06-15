#include <stdio.h>
#include <stdlib.h>
#include "interfaz.h"
#include "log.h"
#include "estadisticas.h"
#include "escalado.h"
#include "juego.h"

/*
Apellido: Mariani, Antonella Lucia
DNI: 41585421
Entrega: Sí

Apellido: Nieva, Alejo Nicolas
DNI: 46556068
Entrega: Sí

Apellido: Savino, Gabriel Dario
DNI: 39067659
Entrega: No
*/

int main(int argc, char* argv[])
{

    inicializarSDLCompleto();

    SDL_Window* ventana = NULL;
    SDL_Renderer* renderer = NULL;
    inicializarVentanaYRenderer(&ventana, &renderer);


    cargarFuentes();
    chequearError(cargarTodosLosRecursos(renderer),NO_CARGO_RECURSOS);

    bool salirPrograma = false;
    while (!salirPrograma)
    {
        opcionesMenuPrincipal opcionPrincipal;
        opcionesMenuTipoPartida opcionTipo;
        opcionesMenuDificultad dificultad;
        char nombreUsuario[MAX_NOMBRE] = "";

        bool continuarMenus = mostrarFlujoDeMenus(renderer, ventana, &opcionPrincipal, &opcionTipo, &dificultad, nombreUsuario);

        if (!continuarMenus)
        {
            salirPrograma = true;
            continue;
        }

        if (opcionTipo == OPCION_PARTIDA_NUEVA)
        {
            ejecutarPartida(renderer, ventana, dificultad, nombreUsuario);
        }
     }
    liberarTodosLosRecursos();
    liberarFuentes();
    liberarTodo(renderer,ventana);
    return 0;
}
