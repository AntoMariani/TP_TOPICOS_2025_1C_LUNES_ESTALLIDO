#include "juego.h"
#include "log.h"
#include "guardar.h"

Casilla** crearTablero(int dimension) {
    Casilla** tablero = malloc(dimension * sizeof(Casilla*));
    if (!tablero) return NULL;

    for (int i = 0; i < dimension; i++) {
        tablero[i] = malloc(dimension * sizeof(Casilla));
        if (!tablero[i]) {
            liberarTablero(tablero, i);  // Solo libera hasta donde se había asignado
            return NULL;
        }

        for (int j = 0; j < dimension; j++) {
            tablero[i][j] = (Casilla){ false, false, false, 0, 0, 0 };
        }
    }

    return tablero;
}

void liberarTablero(Casilla** tablero, int dimension) {
    for (int i = 0; i < dimension; i++)
        free(tablero[i]);
    free(tablero);
}

void inicializarJuego(Juego* juego, const char* archivoConfiguracion) //inicializaCustom nada mas
{
    FILE* archivo = fopen(archivoConfiguracion, "r"); //abre en read
    if (!archivo) {
        printf("No se pudo abrir %s\n", archivoConfiguracion);
        chequearError(NO_ABRIO_ARCHIVO, NO_ABRIO_ARCHIVO);
    }

    char linea[256]; //se prepara para leer linea

    //intenta leer linea dimensiones
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        printf("Error: No se pudo leer la línea de dimensiones\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    // Verifica que la línea comience con "dimensiones="
    if (strncmp(linea, "dimensiones=", 12) != 0) {
        printf("Error: Formato inválido en la línea de dimensiones\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    // Intenta leer el valor de dimensiones y chequea min y max
    int cantidadLeida = sscanf(linea, "dimensiones=%d", &juego->dimension);
    if (cantidadLeida != 1 || juego->dimension < MIN_DIMENSION || juego->dimension > MAX_DIMENSION) {
        printf("Error: Dimensiones inválidas (min %d, max %d)\n", MIN_DIMENSION, MAX_DIMENSION);
        fclose(archivo);
        chequearError(ERROR_DIMENSION, ERROR_DIMENSION);
    }

    // Intenta leer la línea de minas
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        printf("Error: No se pudo leer la línea de minas\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    // Verifica que la línea comience con "minas="
    if (strncmp(linea, "minas=", 6) != 0) { //comparamos los primeros 6 catacteres
        printf("Error: Formato inválido en la línea de minas\n");
        fclose(archivo);
        chequearError(ERROR_FORMATO, ERROR_FORMATO);
    }

    // Intenta leer el valor de minas y chequea si no pudo leer el número, o si el número es menor a 1 o mayor o igual al total de casillas del tablero, muestra un mensaje, cierra el archivo y termina con error.
    cantidadLeida = sscanf(linea, "minas=%d", &juego->totalMinas);
    if (cantidadLeida != 1 || juego->totalMinas < 1 || juego->totalMinas >= juego->dimension * juego->dimension) {
        printf("Error: Cantidad de minas inválida\n");
        fclose(archivo);
        chequearError(ERROR_CANTIDAD_MINAS, ERROR_CANTIDAD_MINAS);
    }

    fclose(archivo); //cierra

    printf("dimensiones=%d totalMinas=%d\n", juego->dimension, juego->totalMinas);
    juego->dimensionInicial = juego->dimension;
    printf("dimension inicial %d\n", juego->dimensionInicial);
    juego->totalMinasInicial = juego->totalMinas;

    juego->minasMarcadas = 0; //ninguna marcada al inicio
    juego->finalizado = false; //no finalizado al inicio
    juego->minasColocadas = false; //no colocadas al inicio
    juego->minaExplotadaFila = -1;
    juego->minaExplotadaCol = -1;

    juego->tamCasilla = calcularTamCasilla(juego->dimension);

    //reserva memoria para el array de punteros a filas del tablero
    juego->tablero = crearTablero(juego->dimension);
    if (!juego->tablero)
        chequearError(NO_ASIGNO_MEM_TABLERO, NO_ASIGNO_MEM_TABLERO);

}

int calcularTamCasilla(int dimension) { //calcula el tam en piceles de cada casilla del tableto para que todo el tablero entre en la ventana
    int tam = TAM_CASILLA_FIJA;

    if (dimension * tam > MAX_ANCHO || dimension * tam > MAX_ALTO) { //si el tablero usando ese tamaño fijo no entra en el ancho o alto calcula el maximo tamaño de casillas que cabe en el ancho y en el alto
        int maxTamPorAncho = MAX_ANCHO / dimension;
        int maxTamPorAlto = MAX_ALTO / dimension;

        tam = (maxTamPorAncho < maxTamPorAlto) ? maxTamPorAncho : maxTamPorAlto; //elige el menor de los dos tamaños
        tam -= tam % 8;
    }

    return tam;
}

int obtenerDimensionPorDificultad(opcionesMenuDificultad dificultad) {
    switch (dificultad) {
        case DIFICULTAD_FACIL:   return 8;
        case DIFICULTAD_MEDIO:   return 16;
        case DIFICULTAD_DIFICIL: return 24;
        case DIFICULTAD_SSJ:     return 32;
        default:                 return 8;  // valor por defecto
    }
}

float obtenerPorcentajeMinas(opcionesMenuDificultad dificultad) {
    switch (dificultad) {
        case DIFICULTAD_FACIL:   return 0.12f;
        case DIFICULTAD_MEDIO:   return 0.15f;
        case DIFICULTAD_DIFICIL: return 0.18f;
        case DIFICULTAD_SSJ:     return 0.22f;
        default:                 return 0.0f;
    }
}

void llenar(Juego* juego, int filaInicial, int colInicial)
{
    srand((unsigned int) time(NULL)); //incializa la semilla del generador de nums random

    int intentos = 0; // para que no genere una mina en esa casilla inciial
    do { //Repite hasta que la casilla inicial no tenga minas vecinas
        for (int fila = 0; fila < juego->dimension; fila++) { //rrecorre todo y lo deja sin minas
            for (int col = 0; col < juego->dimension; col++) {
                juego->tablero[fila][col].esMina = false;
            }
        }

        //coloca minas aleatorais
        int minas = 0;
        while (minas < juego->totalMinas) {
            int fila = rand() % juego->dimension;
            int col = rand() % juego->dimension;

            if ((fila == filaInicial && col == colInicial) || juego->tablero[fila][col].esMina) //si es la casilla inicial o ya hay mina ahi, la salta, sino coloca mina y suma
                continue;

            juego->tablero[fila][col].esMina = true;
            minas++;
        }

        calcularMinasVecinas(juego); //calcula las minas vecinas de cada casilla
        intentos++;
        printf("Intento %d: minas colocadas\n", intentos);

    } while (juego->tablero[filaInicial][colInicial].minasVecinas != 0); //si la casilla inical tiene minas, repite proceso

    juego->minasColocadas = true;
}

void llenarElRestoDeMinas(int totalMinasALlenar, int dimension, Juego * juego){
    srand((unsigned int) time(NULL)); //incializa la semilla del generador de nums random

    int intentos = 0; // para que no genere una mina en esa casilla inciial
    //coloca minas aleatorais
    int minas = 0;
    while (minas < totalMinasALlenar) {
        int fila = rand() % juego->dimension;
        int col = rand() % juego->dimension;

        if ((fila < dimension-2 && col < dimension-2) || juego->tablero[fila][col].esMina || juego->tablero[fila][col].revelada || juego->tablero[fila][col].marcada) //si es la casilla inicial o ya hay mina ahi, la salta, sino coloca mina y suma
            continue;

        juego->tablero[fila][col].esMina = true;
        minas++;
    }

    calcularMinasVecinas(juego); //calcula las minas vecinas de cada casilla
    intentos++;
    printf("Intento %d: minas colocadas\n", intentos);

}

void calcularMinasVecinas(Juego* juego) {
    for (int fila = 0; fila < juego->dimension; fila++) { //recorre todas filas y columnas
        Casilla** filaPtr = juego->tablero + fila;
        for (int col = 0; col < juego->dimension; col++) {
            Casilla* casilla = (*filaPtr) + col; //casilla apunta a la casilla actual

            if (casilla->esMina)
                continue; //si es mina no calcula minas vecinas

            int contador = 0; //inicializa el contador de minas vecinas para la casilla dada

            for (int desplazFila = -1; desplazFila <= 1; desplazFila++) {//recorre las 8 posiciones vecinas, arriba, abajo, izq, der y diagonales
                for (int desplazarCol = -1; desplazarCol <= 1; desplazarCol++) {

                    if (desplazFila == 0 && desplazarCol == 0)
                        continue;

                    int nuevaFila = fila + desplazFila;  //calcula la posicion de la casilla vecina
                    int nuevaCol = col + desplazarCol;

                    if (nuevaFila >= 0 && nuevaFila < juego->dimension && //verifica que este dentro de limites
                        nuevaCol >= 0 && nuevaCol < juego->dimension) {

                        Casilla** nuevaFilaPtr = juego->tablero + nuevaFila;
                        Casilla* vecino = (*nuevaFilaPtr) + nuevaCol;

                        if (vecino->esMina) //si la casilla vecina es una mina suma 1 al contador
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
        return; //si la posicion ta fuera del tablero no hace nada

    Casilla** filaPtr = juego->tablero + fila; //obtiene puntero a la casilla actual
    Casilla* casilla = (*filaPtr) + col;

    if (casilla->revelada || casilla->marcada) //si la casilla ya esta revelada o marcada no la procesa de nuevo
        return;

    casilla->revelada = true;//revela la casilla

    if (casilla->minasVecinas > 0) //si la casilla tiene al menos una mina vecina, termina
        return;

    for (int desplazarFila = -1; desplazarFila <= 1; desplazarFila++) { // si la casilla no tiene minas vecinas, recorre las 8 posiciones vecinas, y llama recursivamente a la funcion para cada casilla vecina.
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
        dimension = obtenerDimensionPorDificultad(dificultad);
        float porcentajeMinas = obtenerPorcentajeMinas(dificultad);
        totalMinas = (int)(dimension * dimension * porcentajeMinas);
        juego.dimension = dimension;
        juego.totalMinas = totalMinas;
        juego.dimensionInicial = dimension;
        juego.totalMinasInicial = totalMinas;
    }


    juego.tamCasilla = calcularTamCasilla(dimension);

    juego.minasMarcadas = 0;
    juego.finalizado = false;
    juego.minasColocadas = false;
    juego.minaExplotadaFila = -1;
    juego.minaExplotadaCol = -1;

    juego.tablero = crearTablero(dimension);
    if (!juego.tablero)
        chequearError(NO_ASIGNO_MEM_TABLERO, NO_ASIGNO_MEM_TABLERO);

    inicializarHistorialFotosTablero(&juego.historial);

    ejecutarLoopDeJuego(renderer, ventana, &juego, dificultad, nombreUsuario,false);

    SDL_PumpEvents();
    SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);

    liberarHistorialFotosTablero(&juego.historial, juego.dimension);

    liberarTablero(juego.tablero, juego.dimension);

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

void ejecutarLoopDeJuego(SDL_Renderer* renderer, SDL_Window* ventana, Juego* juego, opcionesMenuDificultad dificultad, const char* nombreUsuario,bool esPartidaGuardada)
{
    chequearError(iniciarLog(), NO_SE_PUDO_CREAR_LOG);
    chequearError(registrarEvento("INICIO", -1, -1, -1), ERROR_LOG_ESCRITURA);

    int clicksCheat = 0;
    bool cheatEnUso = false;
    Uint32 cheatActivadoTiempo = 0;

    if(!esPartidaGuardada)
    {
        juego->tiempoInicio = SDL_GetTicks();
        //cheat
        juego->cheatUsosRestantes = obtenerMaximoUsosCheat(dificultad);
    }
    //tam ventana segun tablero
    ajustarVentanaYEscalado(ventana, juego);

    SDL_Event evento;

    //obtener botones
    SDL_Rect botonDeshacer, botonCheat, botonRehacer;
    SDL_Rect botonReset, botonAgrandar, botonSalir;
    obtenerRectBotonesHUD(&botonDeshacer, &botonCheat, &botonRehacer,&botonReset, &botonAgrandar, &botonSalir, juego);

    bool ejecutando = true;
    while (ejecutando)
    {
        while (SDL_PollEvent(&evento))
        {
            int x = evento.button.x;
            int y = evento.button.y;
            if (evento.type == SDL_QUIT)
            {
                if(!juego->finalizado)
                    {
                        int anchoVentana = juego->tamCasilla * juego->dimension;
                        int altoVentana  = escalado.tamanioHUD + escalado.tamanioHUDextra + juego->tamCasilla * juego->dimension;
                        opcionesMenuGuardar decision = mostrarMenuGuardar(renderer, ventana, fuenteTexto, anchoVentana, altoVentana);
                        if(decision != OPCION_GUARDAR_CANCELAR)
                        {
                            if(decision == OPCION_GUARDAR_SI)
                            {
                                puts("Click en guardar y salir");
                                guardarPartida(juego, nombreUsuario, dificultad);
                            }
                            ejecutando = false;
                         }
                    }
            }
            //si se detecta clck y juego no termino
            else if (evento.type == SDL_MOUSEBUTTONDOWN && !juego->finalizado && juego->tablero != NULL)
            {

                //agarro donde hizo click y me fijo el centro de la ventana, ademas fijo tamanioBoton por alguna razon?


                if (x >= botonDeshacer.x && x <= botonDeshacer.x + botonDeshacer.w &&
                    y >= botonDeshacer.y && y <= botonDeshacer.y + botonDeshacer.h)
                {
                        if (!deshacerFotoTablero(&juego->historial, juego, ventana))
                        {
                            printf("No se puede deshacer: ya estás en el estado inicial.\n");
                        }
                        ajustarVentanaYEscalado(ventana, juego);
                        obtenerRectBotonesHUD(&botonDeshacer, &botonCheat, &botonRehacer,&botonReset, &botonAgrandar, &botonSalir, juego);
                    break;
                }

                if (x >= botonCheat.x && x <= botonCheat.x + botonCheat.w &&
                    y >= botonCheat.y && y <= botonCheat.y + botonCheat.h)
                {

                    if (juego->minasColocadas && juego->cheatUsosRestantes > 0)
                    {
                        clicksCheat++;
                        if (clicksCheat >= MAX_CLICKS_CHEAT) {
                            cheatEnUso = true;
                            cheatActivadoTiempo = SDL_GetTicks();
                            juego->cheatUsosRestantes--;
                            clicksCheat = 0;
                        }
                    }
                    break;
                }

                if (x >= botonRehacer.x && x <= botonRehacer.x + botonRehacer.w &&
                    y >= botonRehacer.y && y <= botonRehacer.y + botonRehacer.h)
                {
                    if (!rehacerFotoTablero(&juego->historial, juego, ventana)){
                        printf("No hay nada para rehacer\n");
                    }
                    ajustarVentanaYEscalado(ventana, juego);
                    obtenerRectBotonesHUD(&botonDeshacer, &botonCheat, &botonRehacer,&botonReset, &botonAgrandar, &botonSalir, juego);

                    break;
                }

                if (x >= botonReset.x && x <= botonReset.x + botonReset.w &&
                    y >= botonReset.y && y <= botonReset.y + botonReset.h)
                {
                    printf("CLICK: RESET\n");
                    reiniciarPartida(juego, dificultad, ventana);
                    obtenerRectBotonesHUD(&botonDeshacer, &botonCheat, &botonRehacer,
                      &botonReset, &botonAgrandar, &botonSalir, juego);

                }
                else if (x >= botonAgrandar.x && x <= botonAgrandar.x + botonAgrandar.w &&
                         y >= botonAgrandar.y && y <= botonAgrandar.y + botonAgrandar.h)
                {
                    if (juego->dimension < 30) {
                        agrandarTablero(juego, ventana, dificultad);
                        obtenerRectBotonesHUD(&botonDeshacer, &botonCheat, &botonRehacer, &botonReset, &botonAgrandar, &botonSalir, juego);
                    }
                    break;
                }
                else if (x >= botonSalir.x && x <= botonSalir.x + botonSalir.w &&
                         y >= botonSalir.y && y <= botonSalir.y + botonSalir.h)
                {
                    printf("CLICK: SALIR\n");
                    if(!juego->finalizado)
                    {
                        int anchoVentana = juego->tamCasilla * juego->dimension;
                        int altoVentana  = escalado.tamanioHUD + escalado.tamanioHUDextra + juego->tamCasilla * juego->dimension;
                        opcionesMenuGuardar decision = mostrarMenuGuardar(renderer, ventana, fuenteTexto, anchoVentana, altoVentana);
                        if(decision != OPCION_GUARDAR_CANCELAR)
                        {
                            if(decision == OPCION_GUARDAR_SI)
                            {
                                puts("Click en guardar y salir");
                                guardarPartida(juego, nombreUsuario, dificultad);
                            }
                            ejecutando = false;
                         }
                    }
                }

                //si el click fue en el tablero
                if (y >= (escalado.tamanioHUD + escalado.tamanioHUDextra))
                {
                    int fila = (y - escalado.tamanioHUD - escalado.tamanioHUDextra) / juego->tamCasilla;
                    int col = x / juego->tamCasilla;

                    if (evento.button.button == SDL_BUTTON_LEFT) //click izq
                    {

                        if (!juego->minasColocadas)
                        { //si no tiene las minas las coloca
                            llenar(juego, fila, col);
                            calcularMinasVecinas(juego);
                            guardarFotoTablero(&juego->historial,juego);
                        }
                        Casilla* casilla = &juego->tablero[fila][col];

                        if (!casilla->revelada && !casilla->marcada)
                        { //si la casilla no esta revelada ni marcada
                            if (casilla->esMina)
                            { //y es mina, termina el juego
                                casilla->revelada = true;
                                juego->finalizado = true;
                                juego->tiempoFin = SDL_GetTicks();
                                juego->minaExplotadaFila = fila;
                                juego->minaExplotadaCol = col;
                                //dermarca todas las flags y esferas
                                for (int i = 0; i < juego->dimension; i++)
                                {
                                    for (int j = 0; j < juego->dimension; j++)
                                    {
                                        Casilla* c = &juego->tablero[i][j];
                                        if (c->marcada) { c->marcada = false; c->esfera = 0; }
                                    }
                                }
                                //asign esferas aleatorias a las minas para la animacion de perder
                                srand(SDL_GetTicks());
                                for (int i = 0; i < juego->dimension; i++)
                                {
                                    for (int j = 0; j < juego->dimension; j++)
                                    {
                                        Casilla* c = &juego->tablero[i][j];
                                        if (c->esMina) { c->esferaAlPerder = (rand() % 7) + 1; }
                                    }
                                }
                                //perdiste
                                mostrarPantallaFin(renderer, juego, false);
                            } else if (casilla->minasVecinas == 0)
                            {
                                //si no tiene minas vecinas revela en cascada
                                revelarCasillaSinMina(juego, fila, col);
                                guardarFotoTablero(&juego->historial, juego);
                            } else {
                                //sino, solo la revela
                                casilla->revelada = true;
                                guardarFotoTablero(&juego->historial, juego);
                            }
                            //si gano la partida despues de este click marca como finalizado y guarda
                            if (ganoLaPartida(juego) && !juego->finalizado) {
                                juego->finalizado = true;
                                juego->tiempoFin = SDL_GetTicks();
                            }
                        }
                        //registra el evento en el log
                        chequearError(registrarEvento("CLICK_IZQUIERDO", fila, col, casilla->minasVecinas), ERROR_LOG_ESCRITURA);
                    }
                    //click derecho /marca o desmarca
                    if (evento.button.button == SDL_BUTTON_RIGHT)
                    {
                        Casilla* casilla = &juego->tablero[fila][col];
                        if (!casilla->revelada) {
                            if (!casilla->marcada) {

                                if (juego->minasMarcadas < juego->totalMinas)
                                {
                                    casilla->marcada = true;
                                    casilla->esfera = (rand() % TOTAL_ESFERAS) + 1;
                                    juego->minasMarcadas++;
                                    guardarFotoTablero(&juego->historial,juego);
                                }
                            } else
                            {
                                casilla->marcada = false;
                                casilla->esfera = 0;
                                juego->minasMarcadas--;
                                guardarFotoTablero(&juego->historial,juego);
                            }
                            //registra el evento en el log
                            chequearError(registrarEvento("CLICK_DERECHO", fila, col, casilla->esfera), ERROR_LOG_ESCRITURA);

                            //si gano la partida despues del click termina
                            if (ganoLaPartida(juego) && !juego->finalizado)
                            {
                                juego->finalizado = true;
                                juego->tiempoFin = SDL_GetTicks();
                                mostrarPantallaFin(renderer, juego, true);
                            }
                        }
                    }
                }
            }
            ////para que funcione el boton de salir
            if(evento.type == SDL_MOUSEBUTTONDOWN && juego->finalizado)
            {
                if(evento.button.button == SDL_BUTTON_LEFT)
                {
                    if (x >= botonSalir.x && x <= botonSalir.x + botonSalir.w &&
                            y >= botonSalir.y && y <= botonSalir.y + botonSalir.h)
                    {
                        printf("CLICK: SALIR\n");
                        ejecutando = false;
                    }
                }
            }
        }
        //determina si el cheat esta activo
        bool cheatEnPeriodoActivo = (cheatEnUso && (SDL_GetTicks() - cheatActivadoTiempo) <= DURACION_CHEAT_MS);

        bool puedeDeshacer = juego->historial.posActual > 1;
        bool puedeRehacer = juego->historial.posActual < juego->historial.cantidad;
        bool puedeAgrandar = juego->dimension < 30;
        //dibuja tablero e interfaz
        dibujarTablero(renderer, juego, fuenteTexto, fuenteHUD, fuenteBotones, clicksCheat, cheatEnPeriodoActivo, cheatActivadoTiempo, puedeRehacer, puedeDeshacer, puedeAgrandar);

        //si termino el periodo del cheat lo desactiva
        if (!cheatEnPeriodoActivo) { cheatEnUso = false; }
        SDL_Delay(16);


    }
    registrarEvento("FIN", -1, -1, -1);
    const char* nombresDificultad[] = {"FACIL", "MEDIO", "DIFICIL", "SSJ", "CUSTOM", "VOLVER"};

    //si gano, acutaliza estadisticas
    if (ganoLaPartida(juego))
    {
        int tiempoSegundos = (juego->tiempoFin - juego->tiempoInicio) / 1000;
        actualizarEstadisticas(nombresDificultad[dificultad], nombreUsuario, tiempoSegundos);
    }
}

void reiniciarPartida(Juego* juego, opcionesMenuDificultad dificultad, SDL_Window * ventana) {

    juego->dimension = juego->dimensionInicial;
    juego->totalMinas = juego->totalMinasInicial;

    liberarHistorialFotosTablero(&juego->historial, juego->dimension);
    liberarTablero(juego->tablero, juego->dimension);

    //tam ventana segun tablero
    ajustarVentanaYEscalado(ventana, juego);

    // Crear el tablero con valores por defecto
    juego->tablero = crearTablero(juego->dimension);
    if (!juego->tablero)
        chequearError(NO_ASIGNO_MEM_TABLERO, NO_ASIGNO_MEM_TABLERO);

    // Resetear campos de una nueva partida
    juego->minasMarcadas = 0;
    juego->minaExplotadaFila = -1;
    juego->minaExplotadaCol = -1;
    juego->minasColocadas = false;
    juego->finalizado = false;
    juego->tiempoInicio = SDL_GetTicks();
    juego->tiempoFin = 0;

    // Setear esferas
    for (int fila = 0; fila < juego->dimension; fila++) {
        for (int col = 0; col < juego->dimension; col++) {
            juego->tablero[fila][col].esfera = rand() % 7 + 1;
            juego->tablero[fila][col].esferaAlPerder = rand() % 7 + 1;
        }
    }

    juego->cheatActivo = false;
    juego->cheatUsosRestantes = obtenerMaximoUsosCheat(dificultad);
    juego->cheatTiempoInicio = 0;

    inicializarHistorialFotosTablero(&juego->historial);
    guardarFotoTablero(&juego->historial, juego);  // Guarda snapshot inicial (sin minas todavía)

    printf("Partida reiniciada completamente.\n");
}

void agrandarTablero(Juego* juego, SDL_Window* ventana, opcionesMenuDificultad dificultad) {
    int dimensionAnterior = juego->dimension;
    juego->dimension += 2;
    juego->tamCasilla = calcularTamCasilla(juego->dimension);
    ajustarVentanaYEscalado(ventana, juego);

    Casilla** nuevoTablero = crearTablero(juego->dimension);
    if (!nuevoTablero)
        chequearError(NO_ASIGNO_MEM_TABLERO, NO_ASIGNO_MEM_TABLERO);

    //copiar contenido del tablero anterior
    for (int i = 0; i < dimensionAnterior; i++) {
        for (int j = 0; j < dimensionAnterior; j++) {
            nuevoTablero[i][j] = juego->tablero[i][j];
        }
    }

    liberarTablero(juego->tablero, dimensionAnterior);
    juego->tablero = nuevoTablero;

    ///recalcular minas
    float porcentajeMinas = obtenerPorcentajeMinas(dificultad);
    int totalMinasNuevo = (int)(juego->dimension * juego->dimension * porcentajeMinas);
    int totalMinasALlenar = totalMinasNuevo - juego->totalMinas;
    juego->totalMinas = totalMinasNuevo;

    llenarElRestoDeMinas(totalMinasALlenar, juego->dimension, juego);
    guardarFotoTablero(&juego->historial, juego);
}
