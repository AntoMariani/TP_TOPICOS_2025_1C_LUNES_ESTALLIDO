#include "juego.h"
#include "log.h"

void inicializarJuego(Juego* juego, const char* archivoConfiguracion)
{
    FILE* archivo = fopen(archivoConfiguracion, "r");
    if (!archivo) {
        printf("No se pudo abrir %s\n", archivoConfiguracion);
        chequearError(NO_ABRIO_ARCHIVO, NO_ABRIO_ARCHIVO);
    }

    char linea[256];

    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        printf("Error: No se pudo leer la línea de dimensiones\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    if (strncmp(linea, "dimensiones=", 12) != 0) {
        printf("Error: Formato inválido en la línea de dimensiones\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    int cantidadLeida = sscanf(linea, "dimensiones=%d", &juego->dimension);
    if (cantidadLeida != 1 || juego->dimension < MIN_DIMENSION || juego->dimension > MAX_DIMENSION) {
        printf("Error: Dimensiones inválidas (min %d, max %d)\n", MIN_DIMENSION, MAX_DIMENSION);
        fclose(archivo);
        chequearError(ERROR_DIMENSION, ERROR_DIMENSION);
    }

    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        printf("Error: No se pudo leer la línea de minas\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    if (strncmp(linea, "minas=", 6) != 0) {
        printf("Error: Formato inválido en la línea de minas\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    cantidadLeida = sscanf(linea, "minas=%d", &juego->totalMinas);
    if (cantidadLeida != 1 || juego->totalMinas < 1 || juego->totalMinas >= juego->dimension * juego->dimension) {
        printf("Error: Cantidad de minas inválida\n");
        fclose(archivo);
        chequearError(ERROR_CANTIDAD_MINAS, ERROR_CANTIDAD_MINAS);
    }

    fclose(archivo);

    printf("dimensiones=%d totalMinas=%d\n", juego->dimension, juego->totalMinas);

    juego->minasMarcadas = 0;
    juego->finalizado = false;
    juego->minasColocadas = false;
    juego->minaExplotadaFila = -1;
    juego->minaExplotadaCol = -1;

    juego->tamCasilla = calcularTamCasilla(juego->dimension);
    juego->tamPixel = juego->tamCasilla / 8;

    juego->tablero = malloc(juego->dimension * sizeof(Casilla*));
    if (!juego->tablero) {
        printf("Fallo al asignar memoria dinámica para el tablero\n");
        chequearError(NO_ASIGNO_MEM_TABLERO, NO_ASIGNO_MEM_TABLERO);
    }

    for (int i = 0; i < juego->dimension; i++) {
        juego->tablero[i] = malloc(juego->dimension * sizeof(Casilla));
        if (!juego->tablero[i]) {
            printf("No se pudo asignar memoria para la fila %d\n", i);
            chequearError(NO_ASIGNO_MEM_FILA_TABLERO, NO_ASIGNO_MEM_FILA_TABLERO);
        }

        for (int j = 0; j < juego->dimension; j++) {
            juego->tablero[i][j] = (Casilla){ false, false, false, 0, 0 };
        }
    }
}


void liberarJuego(Juego* juego) {
    for (int i = 0; i < juego->dimension; i++) {
        free(*(juego->tablero + i));
    }
    free(juego->tablero);
}

int calcularTamCasilla(int dimension) {
    int tam = TAM_CASILLA_FIJA;

    if (dimension * tam > MAX_ANCHO || dimension * tam > MAX_ALTO) {
        int maxTamPorAncho = MAX_ANCHO / dimension;
        int maxTamPorAlto = MAX_ALTO / dimension;

        tam = (maxTamPorAncho < maxTamPorAlto) ? maxTamPorAncho : maxTamPorAlto;
        tam -= tam % 8; 
    }

    return tam;
}


void llenar(Juego* juego, int filaInicial, int colInicial)
{
    srand((unsigned int) time(NULL));

    int intentos = 0;
    do {
        for (int fila = 0; fila < juego->dimension; fila++) {
            for (int col = 0; col < juego->dimension; col++) {
                juego->tablero[fila][col].esMina = false;
            }
        }

        int minas = 0;
        while (minas < juego->totalMinas) {
            int fila = rand() % juego->dimension;
            int col = rand() % juego->dimension;

            if ((fila == filaInicial && col == colInicial) || juego->tablero[fila][col].esMina)
                continue;

            juego->tablero[fila][col].esMina = true;
            minas++;
        }

        calcularMinasVecinas(juego);
        intentos++;
        printf("Intento %d: minas colocadas\n", intentos);

    } while (juego->tablero[filaInicial][colInicial].minasVecinas != 0);

    juego->minasColocadas = true;
}


void calcularMinasVecinas(Juego* juego) {
    for (int fila = 0; fila < juego->dimension; fila++) {
        Casilla** filaPtr = juego->tablero + fila;
        for (int col = 0; col < juego->dimension; col++) {
            Casilla* casilla = (*filaPtr) + col;

            if (casilla->esMina)
                continue;

            int contador = 0;

            for (int desplazFila = -1; desplazFila <= 1; desplazFila++) {
                for (int desplazarCol = -1; desplazarCol <= 1; desplazarCol++) {

                    if (desplazFila == 0 && desplazarCol == 0)
                        continue;

                    int nuevaFila = fila + desplazFila;
                    int nuevaCol = col + desplazarCol;

                    if (nuevaFila >= 0 && nuevaFila < juego->dimension &&
                        nuevaCol >= 0 && nuevaCol < juego->dimension) {

                        Casilla** nuevaFilaPtr = juego->tablero + nuevaFila;
                        Casilla* vecino = (*nuevaFilaPtr) + nuevaCol;

                        if (vecino->esMina)
                            contador++;
                    }
                }
            }
            casilla->minasVecinas = contador;
        }
    }
}

void revelarCasillaSinMina(Juego* juego, int fila, int col) {

    if (fila < 0 || fila >= juego->dimension || col < 0 || col >= juego->dimension)
        return;

    Casilla** filaPtr = juego->tablero + fila;
    Casilla* casilla = (*filaPtr) + col;

    if (casilla->revelada || casilla->marcada)
        return;

    casilla->revelada = true;

    if (casilla->minasVecinas > 0)
        return;

    for (int desplazarFila = -1; desplazarFila <= 1; desplazarFila++) {
        for (int desplazarCol = -1; desplazarCol <= 1; desplazarCol++) {
            if (desplazarFila == 0 && desplazarCol == 0)
                continue;
            revelarCasillaSinMina(juego, fila + desplazarFila, col + desplazarCol);
        }
    }
}

bool ganoLaPartida(Juego* juego) {
    int totalSinMinas = 0;
    int reveladasSinMinas = 0;
    int minasMarcadasCorrectamente = 0;
    int marcasIncorrectas = 0;

    for (int i = 0; i < juego->dimension; i++) {
        for (int j = 0; j < juego->dimension; j++) {
            Casilla* casilla = &juego->tablero[i][j];

            if (!casilla->esMina) {
                totalSinMinas++;
                if (casilla->revelada)
                    reveladasSinMinas++;
                if (casilla->marcada)
                    marcasIncorrectas++;
            } else { 
                if (casilla->marcada)
                    minasMarcadasCorrectamente++;
            }
        }
    }

    if (reveladasSinMinas == totalSinMinas)
        return true;

    if (minasMarcadasCorrectamente == juego->totalMinas && marcasIncorrectas == 0)
        return true;

    return false;
}

void ejecutarPartida(SDL_Renderer* renderer, SDL_Window* ventana, opcionesMenuDificultad dificultad, const char* nombreUsuario){
    Juego juego;

    int dimension = 0;
    int totalMinas = 0;
    if (dificultad == DIFICULTAD_CUSTOM)
    {
        inicializarJuego(&juego, "buscaminas.conf");
        dimension = juego.dimension;
        totalMinas = juego.totalMinas;
    }
    else
    {
        float porcentajeMinas = 0.0;
        if (dificultad == DIFICULTAD_FACIL) { dimension = 8; porcentajeMinas = 0.12; }
        if (dificultad == DIFICULTAD_MEDIO) { dimension = 16; porcentajeMinas = 0.15; }
        if (dificultad == DIFICULTAD_DIFICIL) { dimension = 24; porcentajeMinas = 0.18; }
        if (dificultad == DIFICULTAD_SSJ) { dimension = 32; porcentajeMinas = 0.22; }
        totalMinas = (int)(dimension * dimension * porcentajeMinas);
        juego.dimension = dimension;
        juego.totalMinas = totalMinas;
    }

    juego.tamCasilla = calcularTamCasilla(dimension);
    juego.tamPixel = juego.tamCasilla / 8;
    juego.minasMarcadas = 0;
    juego.finalizado = false;
    juego.minasColocadas = false;
    juego.minaExplotadaFila = -1;
    juego.minaExplotadaCol = -1;

    juego.tablero = malloc(dimension * sizeof(Casilla*));
    for (int i = 0; i < dimension; i++) {
        juego.tablero[i] = malloc(dimension * sizeof(Casilla));
        for (int j = 0; j < dimension; j++) {
            juego.tablero[i][j] = (Casilla){ false, false, false, 0, 0, 0 };
        }
    }

    ejecutarLoopDeJuego(renderer, ventana, &juego, dificultad, nombreUsuario);

    SDL_PumpEvents();
    SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);

    liberarJuego(&juego);

    liberarTodosLosRecursos();
    liberarFuentes();
    liberarTodo(renderer,ventana);
    exit(0);
}

int obtenerMaximoUsosCheat(opcionesMenuDificultad dificultad) {
    switch (dificultad) {
        case DIFICULTAD_FACIL: return 1;
        case DIFICULTAD_MEDIO: return 2;
        case DIFICULTAD_DIFICIL: return 3;
        case DIFICULTAD_SSJ: return 4;
        default: return 0;
    }
}


void ejecutarLoopDeJuego(SDL_Renderer* renderer, SDL_Window* ventana, Juego* juego, opcionesMenuDificultad dificultad, const char* nombreUsuario){
    chequearError(iniciarLog(), NO_SE_PUDO_CREAR_LOG);
    chequearError(registrarEvento("INICIO", -1, -1, -1), ERROR_LOG_ESCRITURA);
    juego->tiempoInicio = SDL_GetTicks();
    int clicksCheat = 0;
    int usosRestantesCheat = obtenerMaximoUsosCheat(dificultad);
    bool cheatEnUso = false;
    Uint32 cheatActivadoTiempo = 0;

    int anchoVentana = juego->tamCasilla * juego->dimension;
    int altoVentana  = escalado.paddingSuperior + juego->tamCasilla * juego->dimension;
    SDL_SetWindowSize(ventana, anchoVentana, altoVentana);
    calcularEscaladoUI(&escalado, anchoVentana, altoVentana);

    SDL_Event evento;
    bool ejecutando = true;
    while (ejecutando) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                ejecutando = false;
            }
            else if (evento.type == SDL_MOUSEBUTTONDOWN && !juego->finalizado && juego->tablero != NULL){
                int x = evento.button.x;
                int y = evento.button.y;
                int centroX = anchoVentana / 2;
                int centroY = escalado.paddingSuperior / 2;
                int tamanioBoton = 50;
                if (x >= centroX - tamanioBoton/2 && x <= centroX + tamanioBoton/2 && y >= centroY - tamanioBoton/2 && y <= centroY + tamanioBoton/2) {
                    if (juego->minasColocadas && usosRestantesCheat > 0) {
                        clicksCheat++;
                        if (clicksCheat >= MAX_CLICKS_CHEAT) {
                            cheatEnUso = true;
                            cheatActivadoTiempo = SDL_GetTicks();
                            usosRestantesCheat--;
                            clicksCheat = 0;
                        }
                    }
                    break;
                }
                if (y >= escalado.paddingSuperior) {
                    int fila = (y - escalado.paddingSuperior) / juego->tamCasilla;
                    int col = x / juego->tamCasilla;
                    if (evento.button.button == SDL_BUTTON_LEFT) {
                        if (!juego->minasColocadas) {
                            llenar(juego, fila, col);
                            calcularMinasVecinas(juego);
                        }
                        Casilla* casilla = &juego->tablero[fila][col];
                        if (!casilla->revelada && !casilla->marcada) {
                            if (casilla->esMina) {
                                casilla->revelada = true;
                                juego->finalizado = true;
                                juego->tiempoFin = SDL_GetTicks();
                                juego->minaExplotadaFila = fila;
                                juego->minaExplotadaCol = col;
                                for (int i = 0; i < juego->dimension; i++) {
                                    for (int j = 0; j < juego->dimension; j++) {
                                        Casilla* c = &juego->tablero[i][j];
                                        if (c->marcada) { c->marcada = false; c->esfera = 0; }
                                    }
                                }
                                srand(SDL_GetTicks());
                                for (int i = 0; i < juego->dimension; i++) {
                                    for (int j = 0; j < juego->dimension; j++) {
                                        Casilla* c = &juego->tablero[i][j];
                                        if (c->esMina) { c->esferaAlPerder = (rand() % 7) + 1; }
                                    }
                                }
                                mostrarPantallaFin(renderer, juego, false);
                            } else if (casilla->minasVecinas == 0) {
                                revelarCasillaSinMina(juego, fila, col);
                            } else {
                                casilla->revelada = true;
                            }
                            if (ganoLaPartida(juego) && !juego->finalizado) {
                                juego->finalizado = true;
                                juego->tiempoFin = SDL_GetTicks();
                            }
                        }
                        chequearError(registrarEvento("CLICK_IZQUIERDO", fila, col, casilla->minasVecinas), ERROR_LOG_ESCRITURA);
                    }
                    if (evento.button.button == SDL_BUTTON_RIGHT) {
                        Casilla* casilla = &juego->tablero[fila][col];
                        if (!casilla->revelada) {
                            if (!casilla->marcada) {
                                if (juego->minasMarcadas < juego->totalMinas) {  
                                    casilla->marcada = true;
                                    casilla->esfera = (rand() % TOTAL_ESFERAS) + 1;
                                    juego->minasMarcadas++;
                                }
                            } else {
                                casilla->marcada = false;
                                casilla->esfera = 0;
                                juego->minasMarcadas--;
                            }
                            chequearError(registrarEvento("CLICK_DERECHO", fila, col, casilla->esfera), ERROR_LOG_ESCRITURA);

                            if (ganoLaPartida(juego) && !juego->finalizado) {
                                juego->finalizado = true;
                                juego->tiempoFin = SDL_GetTicks();
                                mostrarPantallaFin(renderer, juego, true);
                            }
                        }
                    }
                }
            }
        }
        bool cheatEnPeriodoActivo = (cheatEnUso && (SDL_GetTicks() - cheatActivadoTiempo) <= DURACION_CHEAT_MS);
        dibujarTablero(renderer, juego, fuenteTexto, fuenteHUD, clicksCheat, cheatEnPeriodoActivo, cheatActivadoTiempo);
        if (!cheatEnPeriodoActivo) { cheatEnUso = false; }
        SDL_Delay(16);
    }
    registrarEvento("FIN", -1, -1, -1);
    const char* nombresDificultad[] = {"FACIL", "MEDIO", "DIFICIL", "SSJ", "CUSTOM", "VOLVER"};

    if (ganoLaPartida(juego)) {
        int tiempoSegundos = (juego->tiempoFin - juego->tiempoInicio) / 1000;
        actualizarEstadisticas(nombresDificultad[dificultad], nombreUsuario, tiempoSegundos);
    }
}
