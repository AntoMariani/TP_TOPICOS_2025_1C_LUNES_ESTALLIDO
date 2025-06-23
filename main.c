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

    inicializarSDLCompleto(); //inicializa SDL, SDL_image, SDL_ttf

    SDL_Window* ventana = NULL; //genera puntero a la ventana
    SDL_Renderer* renderer = NULL; //genera puntero al renderer
    inicializarVentanaYRenderer(&ventana, &renderer); //inicializa la ventana y el renderer


    cargarFuentes(); //carga las fuentes de texto
    chequearError(cargarTodosLosRecursos(renderer),NO_CARGO_RECURSOS);

    bool salirPrograma = false; //flag para controlar el programa

    while (!salirPrograma) 
    {
        //inicializamos las opciones de los menus para poder enviarla al flujo de menus
        opcionesMenuPrincipal opcionPrincipal;
        opcionesMenuTipoPartida opcionTipo;
        opcionesMenuDificultad dificultad;

        //inicializamos el nombre de usuario que se usa para guardar estadisticas
        char nombreUsuario[MAX_NOMBRE] = "";

        //bool continuarMenus almacena si el usuario quiere continuar o no en los menus
        bool continuarMenus = mostrarFlujoDeMenus(renderer, ventana, &opcionPrincipal, &opcionTipo, &dificultad, nombreUsuario);
        
        if (!continuarMenus) 
        {
            salirPrograma = true;
            continue; //usamos continue para saltar al final del bucle y no ejecutar el resto del código
        }

        if (opcionTipo == OPCION_PARTIDA_NUEVA) //el usuario eligió una nueva partida
        {
            ejecutarPartida(renderer, ventana, dificultad, nombreUsuario);
        }
     }

    liberarTodosLosRecursos(); //libera animaciones y frames simples
    liberarFuentes(); //libera las fuentes de texto
    liberarTodo(renderer,ventana); //libera la ventana y el renderer, sale de SDL, SDL imagen y SDL ttf
    return 0;
}
