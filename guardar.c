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
    nuevaPartida.tiempoGuardado = (Uint32)time(NULL);
    nuevaPartida.tiempoFin = juego->tiempoFin;
    nuevaPartida.tamCasilla = juego->tamCasilla;
    nuevaPartida.cheatActivo = juego->cheatActivo;
    nuevaPartida.cheatUsosRestantes = juego->cheatUsosRestantes;

    Uint32 tiempoJugadoHastaAhora = (juego->finalizado ? juego->tiempoFin : SDL_GetTicks()) - juego->tiempoInicio;
    nuevaPartida.tiempoInicio = tiempoJugadoHastaAhora;
    strncpy(nuevaPartida.nombreUsuario,nombreUsuario, MAX_NOMBRE);
    nuevaPartida.dificultad = dificultad;

    for (int i = 0; i < nuevaPartida.dimension; i++)
        for (int j = 0; j < nuevaPartida.dimension; j++)
            nuevaPartida.tablero[i][j] = juego->tablero[i][j];

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

    FILE * archivoEscritura = fopen(NOMBRE_ARCHIVO_PARTIDAS, "wb");

    if(!archivoEscritura){
        puts("error al abrir archivo para guardar partidas");
        return;
    }

    for (int i = 0; i < cantidad ; i++){
        fwrite(&partidas[i],sizeof(PartidaGuardada),1,archivoEscritura);
    }

    fclose(archivoEscritura);
    //liberarTablero(nuevaPartida.tablero,nuevaPartida.dimension);

    puts("Se guardo campeon\n");
}

bool cargarPartidaGuardada(Juego* juego, int indiceSlot, opcionesMenuDificultad *dificultad, char* nombreUsuario){
    printf("ingrese a cargar partida guardada");
    if(indiceSlot < 0 || indiceSlot >= MAX_PARTIDAS_GUARDADAS)
    {
        puts("Eligio cualquier cosa");
        return false;
    }
    FILE* archivo = fopen(NOMBRE_ARCHIVO_PARTIDAS,"rb");
    if(!archivo)
    {
        printf("No se pudo abrir el archivo %s\n",NOMBRE_ARCHIVO_PARTIDAS);
        return false;
    }
    PartidaGuardada cargarPartida;

    fseek(archivo, sizeof(PartidaGuardada) * indiceSlot, SEEK_SET);
    fread(&cargarPartida, sizeof(PartidaGuardada), 1, archivo);
    fclose(archivo);

    //revisar y revisar tiempo
    juego->dimension = cargarPartida.dimension;
    juego->totalMinas = cargarPartida.totalMinas;
    juego->minasMarcadas = cargarPartida.minasMarcadas;
    juego->minaExplotadaFila = cargarPartida.minaExplotadaFila;
    juego->minaExplotadaCol = cargarPartida.minaExplotadaCol;
    juego->finalizado = cargarPartida.finalizado;
    juego->minasColocadas = cargarPartida.minasColocadas;
    Uint32 tiempoJugadoHastaAhora = cargarPartida.tiempoInicio;
    juego->tiempoInicio = SDL_GetTicks() - tiempoJugadoHastaAhora;
    juego->tiempoFin = juego->finalizado ? juego->tiempoInicio + tiempoJugadoHastaAhora : 0;
    juego->tiempoFin = cargarPartida.tiempoFin;
    juego->tamCasilla = cargarPartida.tamCasilla;
    juego->cheatActivo = cargarPartida.cheatActivo;
    juego->cheatUsosRestantes = cargarPartida.cheatUsosRestantes;
    juego->cheatTiempoInicio = 0;

    strncpy(nombreUsuario, cargarPartida.nombreUsuario, MAX_NOMBRE);
    *dificultad = cargarPartida.dificultad;


    juego->tablero = crearTablero(cargarPartida.dimension);
    if(!juego->tablero){
        puts("Error al cargar partida");
        return false;
    }

    for (int i = 0; i < cargarPartida.dimension; i++) {
        for (int j = 0; j < cargarPartida.dimension; j++) {
            juego->tablero[i][j] = cargarPartida.tablero[i][j];
        }
    }

    inicializarHistorialFotosTablero(&juego->historial);
    guardarFotoTablero(&juego->historial, juego);

    printf("TIEMPO INICIO CARGADO: %u\n", juego->tiempoInicio);
    return true;
}




