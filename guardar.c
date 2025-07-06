#include "guardar.h"


void guardarPartida(Juego * juego, const char * nombreUsuario, opcionesMenuDificultad dificultad)
{
    FILE * archivo = fopen(NOMBRE_ARCHIVO_PARTIDAS, "rb");
    PartidaGuardada partidas[MAX_PARTIDAS_GUARDADAS];
    int cantidad = 0;


    if(archivo){
        while(fread(&partidas[cantidad], sizeof(PartidaGuardada), 1, archivo) == 1 && cantidad < MAX_PARTIDAS_GUARDADAS)
        {
            cantidad++;
        }
        fclose(archivo);
    } if(!archivo){
        archivo = fopen(NOMBRE_ARCHIVO_PARTIDAS, "wb");
        if(!archivo){
            puts("error al crear el archivo de partidas\n");
            return;
        }
        fclose(archivo);
    }

    PartidaGuardada nuevaPartida;
    nuevaPartida.dimension = juego->dimension;
    nuevaPartida.totalMinas = juego->totalMinas;
    nuevaPartida.minasMarcadas = juego->minasMarcadas;
    nuevaPartida.minaExplotadaFila = juego->minaExplotadaFila;
    nuevaPartida.minaExplotadaCol = juego->minaExplotadaCol;
    nuevaPartida.finalizado = juego->finalizado;
    nuevaPartida.minasColocadas = juego->minasColocadas;
    nuevaPartida.tiempoInicio =

    nuevaPartida.tiempoGuardado = SDL_GetTicks(); ///FIJARSE ESTO
    nuevaPartida.tiempoFin = juego->tiempoFin;
    nuevaPartida.tamCasilla = juego->tamCasilla;
    nuevaPartida.cheatActivo = juego->cheatActivo;
    nuevaPartida.cheatUsosRestantes = juego->cheatUsosRestantes;
    nuevaPartida.tiempoInicio = juego->tiempoInicio;
    strncpy(nuevaPartida.nombreUsuario,nombreUsuario, MAX_NOMBRE);
    nuevaPartida.dificultad = dificultad;

    nuevaPartida.tablero = copiarTablero(juego->tablero, juego->dimension);

    if(!nuevaPartida.tablero){
        printf("Error al copiar el tablero\n");
        return;
    }

    if(cantidad == MAX_PARTIDAS_GUARDADAS)
    {
            int indiceMasVieja = 0;
            for(int i = 1; i < MAX_PARTIDAS_GUARDADAS; i++){
                if(partidas[i].tiempoGuardado < partidas[indiceMasVieja].tiempoGuardado){
                    indiceMasVieja = i;
                }
            }
            partidas[indiceMasVieja] = nuevaPartida;
    } else {
        partidas[cantidad] = nuevaPartida;
        cantidad++;
    }

    FILE * archivoEscritura = fopen(NOMBRE_ARCHIVO_PARTIDAS, "rb");

    if(!archivoEscritura){
        puts("error al abrir archivo para guardar partidas");
        liberarTablero(nuevaPartida.tablero, nuevaPartida.dificultad);
        return;
    }

    for (int i = 0; i < cantidad ; i++){
        fwrite(&partidas[i],sizeof(PartidaGuardada),1,archivoEscritura);
    }

    fclose(archivoEscritura);
    liberarTablero(nuevaPartida.tablero,nuevaPartida.dimension);

    puts("Se guardo campeon\n");
}






