#include "juego.h"
#include "log.h"

void inicializarJuego(Juego* juego, const char* archivoConfiguracion)
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

    juego->minasMarcadas = 0; //ninguna marcada al inicio
    juego->finalizado = false; //no finalizado al inicio
    juego->minasColocadas = false; //no colocadas al inicio
    juego->minaExplotadaFila = -1;
    juego->minaExplotadaCol = -1;

    juego->tamCasilla = calcularTamCasilla(juego->dimension);
    //juego->tamPixel = juego->tamCasilla / 8;

    //reserva memoria para el array de punteros a filas del tablero
    juego->tablero = malloc(juego->dimension * sizeof(Casilla*));
    if (!juego->tablero) {
        printf("Fallo al asignar memoria dinámica para el tablero\n");
        chequearError(NO_ASIGNO_MEM_TABLERO, NO_ASIGNO_MEM_TABLERO);
    }

    //para cada fila reserva memoria para las casillas de esa fila
    for (int i = 0; i < juego->dimension; i++) {
        juego->tablero[i] = malloc(juego->dimension * sizeof(Casilla));
        if (!juego->tablero[i]) {
            printf("No se pudo asignar memoria para la fila %d\n", i);
            chequearError(NO_ASIGNO_MEM_FILA_TABLERO, NO_ASIGNO_MEM_FILA_TABLERO);
        }

        for (int j = 0; j < juego->dimension; j++) { //inicializa cada casilla de la fila, sin mina, no revelada, no marcada, sin minas vecinas
            juego->tablero[i][j] = (Casilla){ false, false, false, 0, 0 };
        }
    }
}


void liberarJuego(Juego* juego) {
    for (int i = 0; i < juego->dimension; i++) { //Recorres las filas
        free(*(juego->tablero + i)); //Libera la memoria
    }
    free(juego->tablero); //libera la memoria del array principal de punteros a filas
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
    //juego.tamPixel = juego.tamCasilla / 8;
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

    //INICIAR

    ejecutarLoopDeJuego(renderer, ventana, &juego, dificultad, nombreUsuario);

    SDL_PumpEvents();
    SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);

    //LIBERAR

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


void ejecutarLoopDeJuego(SDL_Renderer* renderer, SDL_Window* ventana, Juego* juego, opcionesMenuDificultad dificultad, const char* nombreUsuario)
{
    chequearError(iniciarLog(), NO_SE_PUDO_CREAR_LOG);
    chequearError(registrarEvento("INICIO", -1, -1, -1), ERROR_LOG_ESCRITURA);
    juego->tiempoInicio = SDL_GetTicks();

    //cheat
    int clicksCheat = 0;
    int usosRestantesCheat = obtenerMaximoUsosCheat(dificultad);
    bool cheatEnUso = false;
    Uint32 cheatActivadoTiempo = 0;

    //tam ventana segun tablero
    int anchoVentana = juego->tamCasilla * juego->dimension;
    int altoVentana  = escalado.paddingSuperior + juego->tamCasilla * juego->dimension;
    SDL_SetWindowSize(ventana, anchoVentana, altoVentana);
    calcularEscaladoUI(&escalado, anchoVentana, altoVentana); //se escala el juego ahora


    SDL_Event evento;

    //obtener boton
    SDL_Rect botonCheat, botonFuncionalidadNueva;
    obtenerRectBotonesHUD(&botonCheat,&botonFuncionalidadNueva, juego);

    bool ejecutando = true;
    bool hizoClick = false;
    while (ejecutando)
    {
        while (SDL_PollEvent(&evento))
        {
            if (evento.type == SDL_QUIT)
            {
                ejecutando = false;
            }
            //si se detecta clck y juego no termino
            else if (evento.type == SDL_MOUSEBUTTONDOWN && !juego->finalizado && juego->tablero != NULL)
            {

                //agarro donde hizo click y me fijo el centro de la ventana, ademas fijo tamanioBoton por alguna razon?
                int x = evento.button.x;
                int y = evento.button.y;


                if (x >= botonCheat.x && x <= botonCheat.x + botonCheat.w &&
                    y >= botonCheat.y && y <= botonCheat.y + botonCheat.h)
                {

                    if (juego->minasColocadas && usosRestantesCheat > 0)
                    {
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

                if (x >= botonFuncionalidadNueva.x && x <= botonFuncionalidadNueva.x + botonFuncionalidadNueva.w &&
                    y >= botonFuncionalidadNueva.y && y <= botonFuncionalidadNueva.y + botonFuncionalidadNueva.h && !hizoClick)
                {

                    puts("LONG LIVE DEBUGUEAR POR CONSOLA");
                    if(juego->dimension<30)
                    {
                        juego->dimension = juego->dimension+2;

                        juego->tamCasilla = calcularTamCasilla(juego->dimension);

                        //tam ventana segun tablero
                        anchoVentana = juego->tamCasilla * juego->dimension;
                        altoVentana  = escalado.paddingSuperior + juego->tamCasilla * juego->dimension;
                        SDL_SetWindowSize(ventana, anchoVentana, altoVentana);
                        calcularEscaladoUI(&escalado, anchoVentana, altoVentana); //se escala el juego ahora

                        //una vez que reescale la ventana, ahora tengo que generar el nuevo tablero y asignarle memoria

                        Casilla ** nuevoTablero;

                        nuevoTablero = malloc(juego->dimension * sizeof(Casilla*));
                        for (int i = 0; i < juego->dimension; i++) {
                            nuevoTablero[i] = malloc(juego->dimension * sizeof(Casilla));
                            for (int j = 0; j < juego->dimension; j++) {
                                if (i<juego->dimension-2 && j<juego->dimension-2)
                                    nuevoTablero[i][j] = (Casilla){ juego->tablero[i][j].esMina, juego->tablero[i][j].revelada, juego->tablero[i][j].marcada, juego->tablero[i][j].minasVecinas, juego->tablero[i][j].esfera, juego->tablero[i][j].esferaAlPerder };
                                else
                                    nuevoTablero[i][j] = (Casilla){ false, false, false, 0, 0, 0 };
                            }
                        }

                        //calculo de nuevo la cantidad de minas a poner porque no era una func
                        float porcentajeMinas = 0.0;
                        if (dificultad == DIFICULTAD_FACIL) {  porcentajeMinas = 0.12f; }
                        if (dificultad == DIFICULTAD_MEDIO) {  porcentajeMinas = 0.15f; }
                        if (dificultad == DIFICULTAD_DIFICIL) {  porcentajeMinas = 0.18f; }
                        if (dificultad == DIFICULTAD_SSJ) {  porcentajeMinas = 0.22f; }

                        printf("TOTAL MINAS JUEGO %d\n",juego->totalMinas);
                        int totalMinasNuevo = (int)(juego->dimension * juego->dimension * porcentajeMinas);
                        printf("TOTAL MINAS NUEVO %d\n",totalMinasNuevo);
                        int totalMinasALlenar = totalMinasNuevo - juego->totalMinas;
                        printf("TOTAL MINAS a llenar %d\n",totalMinasALlenar);
                        juego->totalMinas = totalMinasNuevo;

                        juego->tablero = nuevoTablero;

                        llenarElRestoDeMinas(totalMinasALlenar,juego->dimension, juego);

                        obtenerRectBotonesHUD(&botonCheat,&botonFuncionalidadNueva, juego);


                    }
                    break;
                    hizoClick = true;
                }

                //si el click fue en el tablero
                if (y >= escalado.paddingSuperior)
                {
                    int fila = (y - escalado.paddingSuperior) / juego->tamCasilla;
                    int col = x / juego->tamCasilla;

                    if (evento.button.button == SDL_BUTTON_LEFT) //click izq
                    {

                        if (!juego->minasColocadas)
                        { //si no tiene las minas las coloca
                            llenar(juego, fila, col);
                            calcularMinasVecinas(juego);
                            ///////////
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
                                ///////////
                            } else {
                                //sino, solo la revela
                                casilla->revelada = true;
                                ///////////
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
                                    ///////////
                                }
                            } else
                            {
                                casilla->marcada = false;
                                casilla->esfera = 0;
                                juego->minasMarcadas--;
                                ///////////
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
        }
        //determina si el cheat esta activo
        bool cheatEnPeriodoActivo = (cheatEnUso && (SDL_GetTicks() - cheatActivadoTiempo) <= DURACION_CHEAT_MS);

        //dibuja tablero e interfaz
        dibujarTablero(renderer, juego, fuenteTexto, fuenteHUD, clicksCheat, cheatEnPeriodoActivo, cheatActivadoTiempo);

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


//void llenarElRestoDeMinas(int totalMinasALlenar, int dimension, Juego * juego){
//
//    srand((unsigned int) time(NULL)); //incializa la semilla del generador de nums random
//    int intentos = 0;
//
//    int minas = 0;
//
//    while (minas < totalMinasALlenar) {
//        int fila = rand() % dimension;
//        int col = rand() % dimension;
//
//        if(juego->tablero[fila][col].esMina)
//                continue;
//
//        juego->tablero[fila][col].esMina = true;
//        minas++;
//    }
//
//        calcularMinasVecinas(juego);
//        intentos++;
//        printf("Intento %d: minas colocadas\n", intentos);
//}

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