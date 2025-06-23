#include "interfaz.h"
#include <stdio.h>

void inicializarSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        chequearError(-990, ERROR_SDL);
    }
    printf("SDL inicializado correctamente\n");
}

void inicializarSDLTtf() {
    if (TTF_Init() == -1) {
        printf("Error al inicializar SDL_ttf: %s\n", TTF_GetError());
        chequearError(-991, ERROR_SDL_TTF);
    }
    printf("SDL TTF inicializado correctamente\n");
}

void inicializarSDLImagenes() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Error al inicializar SDL_image: %s\n", IMG_GetError());
        chequearError(-992, ERROR_SDL_IMAGE);
    }
    printf("SDL imagen inicializado correctamente\n");
}

//inicializamos todo junto para evitar muchos llamados en main
void inicializarSDLCompleto(){
    inicializarSDL();
    inicializarSDLImagenes();
    inicializarSDLTtf();
}

int crearVentana(SDL_Window** ventana, int width, int height)
{
    //se crea ventana centrada
    *ventana = SDL_CreateWindow("BuscaEsferas",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width,
                               height,0);

    //si la ventana falla, es null
    if(*ventana == NULL)
    {
        printf("No se ha podido crear la ventana: %s\n", SDL_GetError());
        return NO_GENERO_VENTANA;
    }
    else
    {
        printf("Ventana creada correctamente\n");
        return TODO_OK;
    }
}


int crearRenderer(SDL_Renderer** renderer, SDL_Window** ventana)
{
    //idem ventana, pero SDL_RENDERER_ACCELERATED es para usar la GPU de estar disponible entiendo
    *renderer = SDL_CreateRenderer(*ventana, -1, SDL_RENDERER_ACCELERATED);

    if(*renderer == NULL)
    {
        printf("No se ha podido crear el renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(*ventana);
        return NO_GENERO_RENDERER;
    }
    printf("Renderer creado correctamente\n");
    return TODO_OK;
}

void inicializarVentanaYRenderer(SDL_Window** ventana, SDL_Renderer** renderer)
{
    //obtenemos el modo de pantalla actual para poder calcular como escalarlo
    SDL_DisplayMode displayMode; //es un tipo de dato struct definido por sdl con varios campos: w, h, format, refresh_rate, driverdata

    SDL_GetCurrentDisplayMode(0, &displayMode); //obtenemos la res actual del monitor principal, el 0 significa el primer monitor y la guardar en displayMode

    printf("Resolucion: %dx%d\n", displayMode.w, displayMode.h); 
    //imprimimos la res para ver de ejemplo, TIENE EN CUENTA EL ZOOM por ejemplo mi pantalla esta en 1920 x 1200 al 125% y cambia la res en base a eso

    //cargamos nuestras vars de ancho y alto para escalar
    int anchoPantalla = displayMode.w;
    int altoPantalla  = displayMode.h;

    // Calculamos el escalado de la UI en base a la resolucion del monitor
    calcularEscaladoUI(&escalado, anchoPantalla, altoPantalla);

    //generamos la ventana y el renderer según todo lo que escalamos
    chequearError(crearVentana(ventana, escalado.anchoVentanaMenu, escalado.altoVentanaMenu), NO_GENERO_VENTANA);
    chequearError(crearRenderer(renderer, ventana), NO_GENERO_RENDERER);
}


void dibujarTablero(SDL_Renderer* renderer, Juego* juego, TTF_Font* fuente, TTF_Font* fuenteHUD, int clicksCheat, bool cheatEnPeriodoActivo, Uint32 cheatActivadoTiempo)
{
    //verificacion de punteros validos
    if (!renderer || !juego || !fuente || !fuenteHUD) {
        printf("Error: Renderer, juego o fuentes no inicializados correctamente.\n");
        return;
    }

    //limpia la pantalla con un color de fondo
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    //dibuja el HUD en la barra superior
    SDL_Rect hud = { 0, 0, juego->tamCasilla * juego->dimension, escalado.paddingSuperior }; //typedef struct SDL_Rect {int x, y; int w, h;} SDL_Rect;
    //x posicion horizontal izquierda, y posicion vertical superior, w ancho, h alto.
    //x = 0 el rectangulo empieza en el borde izquierdo de la ventana, y = 0 el rectangulo empieza en el borde superior de la ventana
    //w = juego->tamCasilla (tamaño en pixeles de cada casilla) * juego->dimension (cantidad de casillas en una fila o columna)
    //h = escalado.paddingSuperior el alto del rectangulo es el padding superior

    //confugura el color de fondo del HUD y lo dibuja
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255); //aca podriamos usar un color definido en colores[], tendriamos que agregarlo, agregar su define y pasarselo asi SDL_SetRenderDrawColor(renderer, colorLCD.r, colorLCD.g, colorLCD.b, colorLCD.a)
    SDL_RenderFillRect(renderer, &hud);

    //////////////////////////////////CONTADOR DE MINAS RESTANTES////////////////////////////////////
    char minasTexto[4]; //para mostrar las minas restantes
    sprintf(minasTexto, "%03d", juego->totalMinas - juego->minasMarcadas); //formatea el texto con las minas restantes, mestra un numero de 3 digitos, rellenando con ceros a la izq
    SDL_Color colorLCD = colores[CNH];

    //generamos la superficie de texto con el contador de minas, es una imagen en memoria "surface" que contiene el texto correspondiente dibujado con la fuente y color correspondientes
    //se tiene que si o si usar una surface para crear una textura con la funcion correspondiente
    // TTF_RenderText_Blended crea una surface con el texto renderizado, el color y la fuente especificados
    SDL_Surface* surfaceMinas = TTF_RenderText_Blended(fuenteHUD, minasTexto, colorLCD);
    
    //conviertimos el surface en RAM  a una textura que puede ser usada por el renderer de SDL para dibujar en pantalla
    SDL_Texture* texturaMinas = SDL_CreateTextureFromSurface(renderer, surfaceMinas);

    //define el rectangulo en donde se tiene que dibujar la textura del contador de minas (texto)
    SDL_Rect rectMinas = {20, 10, surfaceMinas->w, surfaceMinas->h}; //20 y 10 posicion en ventana en px desde izq y desde arriba, ancho y alto del renderizado

    //aca efectivamente dibuja la textura en la ventan adonde se indica, el null indica que se usa toda la textura
    SDL_RenderCopy(renderer, texturaMinas, NULL, &rectMinas);

    //libera después de dibujar la superficie y textura
    SDL_FreeSurface(surfaceMinas);
    SDL_DestroyTexture(texturaMinas);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////TIMER SUPERIOR//////////////////////////////////////////
    // calcula el tiempo, si el juego ya termino usa el tiempo de fin - inicio divide /1000 para segundos, si el juego esta en curso usa SDL_GetTicks() que devuelve ms dedde que se inciio SDL - tiempoInicio.
    int segundosTranscurridos = (juego->finalizado) ? (juego->tiempoFin - juego->tiempoInicio) / 1000 : (SDL_GetTicks() - juego->tiempoInicio) / 1000;

    //limitamos tiempo a 999
    if (segundosTranscurridos > 999) segundosTranscurridos = 999;

    //idem minas
    char tiempoTexto[4]; 
    sprintf(tiempoTexto, "%03d", segundosTranscurridos);
    SDL_Surface* surfaceTiempo = TTF_RenderText_Blended(fuenteHUD, tiempoTexto, colorLCD);
    SDL_Texture* texturaTiempo = SDL_CreateTextureFromSurface(renderer, surfaceTiempo);
    int anchoVentana = juego->tamCasilla * juego->dimension; //como el temp va a la derecha, tenemos que calcular el ancho de ventana
    SDL_Rect rectTiempo = {anchoVentana - surfaceTiempo->w - 20, 10, surfaceTiempo->w, surfaceTiempo->h}; // calculamos segun el ancho y el padding, 20 es el padding desde la derecha, 10 es el padding desde arriba
    SDL_RenderCopy(renderer, texturaTiempo, NULL, &rectTiempo);
    SDL_FreeSurface(surfaceTiempo);
    SDL_DestroyTexture(texturaTiempo);
    ////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////CHEAT//////////////////////////////////////////////
    //calculamos centro del hud para el boton de cheat, teniendo el cuenta el padding superior
    int centroX = anchoVentana / 2;
    int centroY = escalado.paddingSuperior / 2;
    int tamanioBoton = (int)(50 * escalado.escalaGlobal); // el tamaño del botón se escala según la resolución de pantalla
    if (tamanioBoton < 35) tamanioBoton = 35; // tamaño mínimo para el botón

    SDL_Rect rectBoton = { centroX - tamanioBoton/2, centroY - tamanioBoton/2, tamanioBoton, tamanioBoton }; 
    //para que los centros coincidal centroX - tamanioBoton/2

    if (cheatEnPeriodoActivo) {
        int frame = (SDL_GetTicks() / 200) % FRAMES_RASTREADOR; //cada 200 ms el valor aumenta en 1, cada 0,2 segs cambia el frame de animacion. % FRAMES_RASTREADOR hace que el valor vuelva a 0 para que haga bucle
        SDL_RenderCopy(renderer, rastreador_cheat_texturas[frame], NULL, &rectBoton); //PONE EL RASTREADOR EN LUGAR DE LA ESFERA
    } else {
        int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_ESFERAS; //calcula que frame mostrar
        int esferaActual = (clicksCheat % 7); //calcula que esfera mostrar para que sea entre 0 y 6
        if (esferaActual == 0) esferaActual = 1; //si da 0 lo cambia a 1 porque la esfera 0 no existe
        SDL_RenderCopy(renderer, esferas_texturas[esferaActual][frameAnimacion], NULL, &rectBoton); //dibuja la textura de la esfera correspondiente a esferaActual y el frame de animacion actual frameAnimacion
    }

    SDL_Rect celda = {0, 0, juego->tamCasilla, juego->tamCasilla}; //creamos el rectangulo que representa una casilla del tablero empezando en 0,0

    for (int fila = 0; fila < juego->dimension; fila++) { //estos dos for recorren el tablero calculando la posicion de cada celda en pantalla
        for (int col = 0; col < juego->dimension; col++) {
            celda.x = col * juego->tamCasilla;
            celda.y = escalado.paddingSuperior + fila * juego->tamCasilla;
            Casilla* casilla = &juego->tablero[fila][col]; //, obteniendo un puntero a la casilla actual

            if (juego->finalizado && casilla->esMina) { //si el juego finalizó y la casilla es una mina pone una mina explotada en esa celda, sino muestra una esfera random
                if (fila == juego->minaExplotadaFila && col == juego->minaExplotadaCol) {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_MINA_EXPLOTADA;
                    SDL_RenderCopy(renderer, mina_explotada_texturas[frameAnimacion], NULL, &celda);
                } else {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_ESFERAS;
                    SDL_RenderCopy(renderer, esferas_texturas[casilla->esferaAlPerder][frameAnimacion], NULL, &celda);
                }
            }
            else if (juego->finalizado && casilla->revelada && !casilla->esMina) { //si el juego finalizo y la casilla fue revelada y no es una mina, muestra el numero de minas vecinas
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                SDL_RenderFillRect(renderer, &celda);
                if (casilla->minasVecinas > 0) {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_NUMEROS;
                    SDL_RenderCopy(renderer, numeros_texturas[casilla->minasVecinas][frameAnimacion], NULL, &celda);
                }
            }
            else if (casilla->revelada) { //si la casilla fue revelada y no es una mina, muestra el numero de minas vecinas
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &celda);
                if (casilla->minasVecinas > 0) {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_NUMEROS;
                    SDL_RenderCopy(renderer, numeros_texturas[casilla->minasVecinas][frameAnimacion], NULL, &celda);
                }
            }
            else if (casilla->marcada) { //si la casilla esta marcada, muestra una esfera
                int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_ESFERAS;
                SDL_RenderCopy(renderer, esferas_texturas[casilla->esfera][frameAnimacion], NULL, &celda);
            }
            else { //si la casilla no fue revelada ni marcada, dibuja un rectangulo gris claro
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
                SDL_RenderFillRect(renderer, &celda);
            }

            // Dibuja el borde de la celda
            SDL_SetRenderDrawColor(renderer, colores[CNH].r, colores[CNH].g, colores[CNH].b, colores[CNH].a);
            SDL_RenderDrawRect(renderer, &celda); //dibuja SOLO el contorno de un rectangulo
        }
    }

    if (juego->minasColocadas && cheatEnPeriodoActivo && !juego->finalizado) { 
        //si el cheat esta activo y las minas ya fueron colocadas, dibuja el overlay de minas
        dibujarOverlayCheat(renderer, juego);
    }

    //Actualiza el renderer para mostrar todo lo que se dibujó
    SDL_RenderPresent(renderer);
}

void dibujarOverlayCheat(SDL_Renderer* renderer, Juego* juego) 
{
    for (int fila = 0; fila < juego->dimension; fila++) { //recorre filas y columnas del tablero
        for (int col = 0; col < juego->dimension; col++) {
            Casilla* casilla = &juego->tablero[fila][col]; //puntero a casilla actual
            if (casilla->esMina && !casilla->marcada) { //si es mina y no esta marcada, dibuja un rectangulo semi-transparente
                SDL_Rect destino = { 
                    col * juego->tamCasilla, //x
                    escalado.paddingSuperior + fila * juego->tamCasilla, //y teniendo en cuenta padding de hud
                    juego->tamCasilla, //w
                    juego->tamCasilla //h
                };
                SDL_SetRenderDrawColor(renderer, 255, 180, 0, 200); //es naranja transparente
                printf("Pintando cheat en fila=%d col=%d\n", fila, col);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //permitimos transparencia
                SDL_RenderFillRect(renderer, &destino);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE); //resetea draw blend
            }
        }
    }
}

//funcion que se usaba antes
//void dibujarFigura(SDL_Renderer* renderer, const int figura[8][8], int fila, int col,int tamPixel) {
//    SDL_Rect pixel;
//    pixel.w = tamPixel;
//    pixel.h = tamPixel;
//
//    int offsetX = col * tamPixel * 8;
//    int offsetY = escalado.paddingSuperior + fila * tamPixel * 8;
//
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            int colorIndex = figura[i][j];
//            SDL_Color color = colores[colorIndex];
//            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
//            pixel.x = offsetX + j * tamPixel;
//            pixel.y = offsetY + i * tamPixel;
//            SDL_RenderFillRect(renderer, &pixel);
//        }
//    }
//}

void dibujarBotonPlano(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color colorFondo) {
    SDL_SetRenderDrawColor(renderer, colorFondo.r, colorFondo.g, colorFondo.b, colorFondo.a); //color
    SDL_RenderFillRect(renderer, &rect); //rellena de ese color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //configura color de dibujo para el borde
    SDL_RenderDrawRect(renderer, &rect); //dibuna borde
}

//funcion p dibujar texto centrado dentro de botones
void dibujarTexto(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, SDL_Rect rect, SDL_Color colorTexto) {

    //crea surface texto
    SDL_Surface* surfaceTexto = TTF_RenderText_Blended(fuente, texto, colorTexto);
    if (!surfaceTexto) {
        printf("Error renderizando texto: %s\n", TTF_GetError());
        return;
    }

    //crea texture desde la surface
    SDL_Texture* textureTexto = SDL_CreateTextureFromSurface(renderer, surfaceTexto);
    if (!textureTexto) {
        printf("Error creando textura de texto: %s\n", SDL_GetError());
        SDL_FreeSurface(surfaceTexto);
        return;
    }

    //calcula el rectangulo de destino para centrarlo
    //rect.x posicion inicial, rect.w ancho del rect, surfaceTexto->w ancho del texto, rect.y posicion inicial, rect.h alto del rect, surfaceTexto->h alto del texto
    SDL_Rect destino = {
        rect.x + (rect.w - surfaceTexto->w) / 2,
        rect.y + (rect.h - surfaceTexto->h) / 2,
        surfaceTexto->w,
        surfaceTexto->h
    };

    SDL_RenderCopy(renderer, textureTexto, NULL, &destino);
    SDL_FreeSurface(surfaceTexto);
    SDL_DestroyTexture(textureTexto);
}

opcionesMenuPrincipal mostrarMenuPrincipal(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuente) {

    //algo que costo mucho que entendamos fue: por que dibujamos y etc despues de procesar eventos?
    //porque el bucle de eventos y de dibujo son dos cosas separadas, y por convención siempre se usa ese orden
    //primero procesas eventos (sabes si el usuario hizo click, cerro ventana, movio mouse, etc).
    //y después dibujas la pantalla. Si lo hicieras al revés podes estar dibujando cosas que el usuario ya cambió.

    //eventos y flag control menu
    SDL_Event evento;
    bool salir = false;

    //cantidad de botones a mostrar y su posicion
    int cantidadBotones = 3; //JUGAR, ESTADISTICAS, SALIR

    //altototal = alto de todos los botones + espacio vertical entre botones (en este caso si hay 3 bot, hay 2 espacios entre ellos porque 3 - 1 = 2).
    int altoTotal = cantidadBotones * escalado.botonAlto + (cantidadBotones - 1) * escalado.espaciadoVertical;

    //centra botones:
    //clacula el alto disp para botones, descontando el margen superior y resta el alto de los botones al alto disponible. Divide ese espacio sobrante / 2 para que quede por arriba y por debajo de los botones.
    int inicioY = escalado.margenInicialY + (escalado.altoVentanaMenu - escalado.margenInicialY - altoTotal) / 2;


    //x, y, h, w de los botones
    SDL_Rect botonJugar = { (escalado.anchoVentanaMenu - escalado.botonAncho) / 2, inicioY, escalado.botonAncho, escalado.botonAlto };
    SDL_Rect botonEstadisticas = { botonJugar.x, inicioY + escalado.botonAlto + escalado.espaciadoVertical, escalado.botonAncho, escalado.botonAlto };
    SDL_Rect botonSalir = { botonEstadisticas.x, inicioY + 2*(escalado.botonAlto + escalado.espaciadoVertical), escalado.botonAncho, escalado.botonAlto };

    while (!salir) {
        while (SDL_PollEvent(&evento)) { //mientras no se salga del menu procesa events
            if (evento.type == SDL_QUIT) //si el usuario cierra la ventana sale del juego
                exit(0);
            else if (evento.type == SDL_MOUSEBUTTONDOWN) { //cuando hace click obtiene DONDE ahce click
                int x = evento.button.x, y = evento.button.y;

                //usa los limites de los botones para ver si corresponden las coordenadas
                if (x >= botonJugar.x 
                    && x <= botonJugar.x + escalado.botonAncho
                    && y >= botonJugar.y 
                    && y <= botonJugar.y + escalado.botonAlto)
                    return OPCION_PRINCIPAL_JUGAR;

                if (x >= botonEstadisticas.x 
                    && x <= botonEstadisticas.x + escalado.botonAncho 
                    && y >= botonEstadisticas.y 
                    && y <= botonEstadisticas.y + escalado.botonAlto)
                    return OPCION_PRINCIPAL_ESTADISTICAS;

                if (x >= botonSalir.x 
                    && x <= botonSalir.x + escalado.botonAncho 
                    && y >= botonSalir.y 
                    && y <= botonSalir.y + escalado.botonAlto)
                    return OPCION_PRINCIPAL_SALIR;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); //dibuja fondo fris oscuro
        SDL_RenderClear(renderer); //limpia pantalla

        int frameLogo = (SDL_GetTicks() / 300) % LOGO_FRAMES; //cada 300ms cambia frame del logo
        dibujarLogo(renderer, ventana, frameLogo); //dibuja el logo en la ventana

        //definimos colores
        SDL_Color fondo = {70,70,70,255}, texto = {255,255,255,255};

        //definimos botones
        dibujarBotonPlano(renderer, botonJugar, fondo);
        dibujarBotonPlano(renderer, botonEstadisticas, fondo);
        dibujarBotonPlano(renderer, botonSalir, fondo);

        //dibujamos texto
        dibujarTexto(renderer, fuente, "JUGAR", botonJugar, texto);
        dibujarTexto(renderer, fuente, "ESTADISTICAS", botonEstadisticas, texto);
        dibujarTexto(renderer, fuente, "SALIR", botonSalir, texto);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return OPCION_PRINCIPAL_SALIR;
}

opcionesMenuTipoPartida mostrarMenuTipoPartida(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuente) {
    SDL_Event evento;
    bool salir = false;

    int cantidadBotones = 3;
    int altoTotal = cantidadBotones * escalado.botonAlto + (cantidadBotones - 1) * escalado.espaciadoVertical;
    int inicioY = escalado.margenInicialY + (escalado.altoVentanaMenu - escalado.margenInicialY - altoTotal) / 2;

    SDL_Rect botonNueva = { (escalado.anchoVentanaMenu - escalado.botonAncho) / 2, inicioY, escalado.botonAncho, escalado.botonAlto };
    SDL_Rect botonCargar = { botonNueva.x, inicioY + escalado.botonAlto + escalado.espaciadoVertical, escalado.botonAncho, escalado.botonAlto };
    SDL_Rect botonMenuPrincipal = { botonCargar.x, inicioY + 2*(escalado.botonAlto + escalado.espaciadoVertical), escalado.botonAncho, escalado.botonAlto };


    while (!salir) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT)
                exit(0);
            else if (evento.type == SDL_MOUSEBUTTONDOWN) {
                int x = evento.button.x, y = evento.button.y;

                if (x >= botonNueva.x && x <= botonNueva.x + escalado.botonAncho && y >= botonNueva.y && y <= botonNueva.y + escalado.botonAlto)
                    return OPCION_PARTIDA_NUEVA;

                if (x >= botonCargar.x && x <= botonCargar.x + escalado.botonAncho && y >= botonCargar.y && y <= botonCargar.y + escalado.botonAlto)
                    return OPCION_PARTIDA_CARGAR;

                if (x >= botonMenuPrincipal.x && x <= botonMenuPrincipal.x + escalado.botonAncho && y >= botonMenuPrincipal.y && y <= botonMenuPrincipal.y + escalado.botonAlto)
                    return OPCION_PARTIDA_MENU_PRINCIPAL;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int frameLogo = (SDL_GetTicks() / 300) % LOGO_FRAMES;
        dibujarLogo(renderer, ventana, frameLogo);

        SDL_Color fondo = {70,70,70,255}, texto = {255,255,255,255};

        dibujarBotonPlano(renderer, botonNueva, fondo);
        dibujarBotonPlano(renderer, botonCargar, fondo);
        dibujarBotonPlano(renderer, botonMenuPrincipal, fondo);

        dibujarTexto(renderer, fuente, "NUEVA PARTIDA", botonNueva, texto);
        dibujarTexto(renderer, fuente, "CARGAR PARTIDA", botonCargar, texto);
        dibujarTexto(renderer, fuente, "MENU PRINCIPAL", botonMenuPrincipal, texto);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return OPCION_PARTIDA_MENU_PRINCIPAL;
}

opcionesMenuDificultad mostrarMenuDificultad(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuente) {
    SDL_Event evento;
    bool salir = false;


    int cantidadBotones = 6;

    int espacioDisponible = escalado.altoVentanaMenu - escalado.margenInicialY - (int)(escalado.botonAlto * 0.15f);
    int espaciadoVerticalDificultad = (int)(escalado.espaciadoVertical * 1.5f);
    int alturaBotonFinal = (espacioDisponible - (cantidadBotones - 1) * espaciadoVerticalDificultad) / cantidadBotones;


    if (alturaBotonFinal > escalado.botonAlto)
        alturaBotonFinal = escalado.botonAlto;

    int altoTotal = cantidadBotones * alturaBotonFinal + (cantidadBotones - 1) * escalado.espaciadoVertical;
    int inicioY = escalado.margenInicialY + (escalado.altoVentanaMenu - escalado.margenInicialY - altoTotal) / 2;

    SDL_Rect botonFacil = { (escalado.anchoVentanaMenu - escalado.botonAncho) / 2, inicioY, escalado.botonAncho, alturaBotonFinal };
    SDL_Rect botonMedio = { botonFacil.x, botonFacil.y + alturaBotonFinal + escalado.espaciadoVertical, escalado.botonAncho, alturaBotonFinal };
    SDL_Rect botonDificil = { botonMedio.x, botonMedio.y + alturaBotonFinal + escalado.espaciadoVertical, escalado.botonAncho, alturaBotonFinal };
    SDL_Rect botonSuper = { botonDificil.x, botonDificil.y + alturaBotonFinal + escalado.espaciadoVertical, escalado.botonAncho, alturaBotonFinal };
    SDL_Rect botonCustom = { botonSuper.x, botonSuper.y + alturaBotonFinal + escalado.espaciadoVertical, escalado.botonAncho, alturaBotonFinal };
    SDL_Rect botonVolver = { botonCustom.x, botonCustom.y + alturaBotonFinal + escalado.espaciadoVertical, escalado.botonAncho, alturaBotonFinal };


    while (!salir) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT)
                exit(0);
            else if (evento.type == SDL_MOUSEBUTTONDOWN) {
                int x = evento.button.x, y = evento.button.y;

                if (x >= botonFacil.x && x <= botonFacil.x + escalado.botonAncho && y >= botonFacil.y && y <= botonFacil.y + alturaBotonFinal)
                    return DIFICULTAD_FACIL;

                if (x >= botonMedio.x && x <= botonMedio.x + escalado.botonAncho && y >= botonMedio.y && y <= botonMedio.y + alturaBotonFinal)
                    return DIFICULTAD_MEDIO;

                if (x >= botonDificil.x && x <= botonDificil.x + escalado.botonAncho && y >= botonDificil.y && y <= botonDificil.y + alturaBotonFinal)
                    return DIFICULTAD_DIFICIL;

                if (x >= botonSuper.x && x <= botonSuper.x + escalado.botonAncho && y >= botonSuper.y && y <= botonSuper.y + alturaBotonFinal)
                    return DIFICULTAD_SSJ;

                if (x >= botonCustom.x && x <= botonCustom.x + escalado.botonAncho && y >= botonCustom.y && y <= botonCustom.y + alturaBotonFinal)
                    return DIFICULTAD_CUSTOM;

                if (x >= botonVolver.x && x <= botonVolver.x + escalado.botonAncho && y >= botonVolver.y && y <= botonVolver.y + alturaBotonFinal)
                    return DIFICULTAD_VOLVER;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int frameLogo = (SDL_GetTicks() / 300) % LOGO_FRAMES;
        dibujarLogo(renderer, ventana, frameLogo);

        SDL_Color fondo = {70,70,70,255}, texto = {255,255,255,255};

        dibujarBotonPlano(renderer, botonFacil, fondo);
        dibujarBotonPlano(renderer, botonMedio, fondo);
        dibujarBotonPlano(renderer, botonDificil, fondo);
        dibujarBotonPlano(renderer, botonSuper, fondo);
        dibujarBotonPlano(renderer, botonCustom, fondo);
        dibujarBotonPlano(renderer, botonVolver, fondo);

        dibujarTexto(renderer, fuente, "FACIL", botonFacil, texto);
        dibujarTexto(renderer, fuente, "MEDIO", botonMedio, texto);
        dibujarTexto(renderer, fuente, "DIFICIL", botonDificil, texto);
        dibujarTexto(renderer, fuente, "SSJ", botonSuper, texto);
        dibujarTexto(renderer, fuente, "CUSTOM", botonCustom, texto);
        dibujarTexto(renderer, fuente, "VOLVER", botonVolver, texto);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); //espera 16ms para no saturar el CPU, lo vimos varias veces en tutoriales asi que decidimos sumarlo
    }
    return DIFICULTAD_VOLVER;
}

//void pedirNombreUsuario(SDL_Renderer* renderer, TTF_Font* fuente, char* nombreUsuario) {
//
//    SDL_StartTextInput(); //habilitamos texto
//
//    nombreUsuario[0] = '\0'; //inicializamos el nombre de usuario como una cadena vacía
//    int salir = 0; //flag salida
//    SDL_Event evento; 
//
//    while (!salir) {
//        while (SDL_PollEvent(&evento)) {
//            if (evento.type == SDL_QUIT) {
//                exit(0);
//            }
//            if (evento.type == SDL_TEXTINPUT) {
//                if (strlen(nombreUsuario) < MAX_NOMBRE - 1) {
//                    strcat(nombreUsuario, evento.text.text); //copia lo que entro el usuario, concatena al final mientras tenga <5 caracteres
//                }
//            }
//            if (evento.type == SDL_KEYDOWN) { //si apreta borrar y el nombre no esta vacio borra 1 caracter, si le da enter y el nombre no esta vacio confirma el nombre
//                if (evento.key.keysym.sym == SDLK_BACKSPACE && strlen(nombreUsuario) > 0) {
//                    nombreUsuario[strlen(nombreUsuario) - 1] = '\0'; //strlen(nombreUsuario) - 1 es el ultimo caracter, lo pone en 0 para borrar
//                }
//                if (evento.key.keysym.sym == SDLK_RETURN && strlen(nombreUsuario) > 0) { // si presiono enter se fija si escribi 1 caracter por lo menos y sale del bucle
//                    salir = 1;
//                }
//            }
//        }
//
//        //limpia pantalla a negro 
//        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//        SDL_RenderClear(renderer);
//
//
//        //color blanco texto
//        SDL_Color color = {255, 255, 255, 255};
//
//        //dibuja ingrese su nombre (idem otros surfaces y textures) y libera memoria
//        SDL_Surface* superficieTexto = TTF_RenderText_Blended(fuente, "Ingrese su nombre:", color);
//        SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);
//        SDL_Rect rect = {100, 100, superficieTexto->w, superficieTexto->h};
//        SDL_RenderCopy(renderer, texturaTexto, NULL, &rect);
//        SDL_FreeSurface(superficieTexto);
//        SDL_DestroyTexture(texturaTexto);
//
//
//        //dibuja el nombre que el user escribe y va liberando mem
//        superficieTexto = TTF_RenderText_Blended(fuente, nombreUsuario, color);
//        texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);
//        rect.y = 200;
//        rect.w = superficieTexto->w;
//        rect.h = superficieTexto->h;
//        SDL_RenderCopy(renderer, texturaTexto, NULL, &rect);
//        SDL_FreeSurface(superficieTexto);
//        SDL_DestroyTexture(texturaTexto);
//
//        //actualiza pantalla
//        SDL_RenderPresent(renderer);
//        SDL_Delay(16);
//    }
//
//    //frena el input
//    SDL_StopTextInput();
//}

opcionesMenuNickname mostrarMenuNickname(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuente, char* nombreUsuario)
{
    //printf("ENTRE AL MENU NICKNAME\n");

    SDL_StartTextInput();

    nombreUsuario[0] = '\0';
    SDL_Event evento;
    bool salir = false;
    opcionesMenuNickname resultado = OPCION_NICKNAME_VOLVER;

    //bloques: titulo, campo texto, botones JUGAR y VOLVER
    int cantidadBloques = 3;
    int altoTotal = cantidadBloques * escalado.botonAlto + (cantidadBloques - 1) * escalado.espaciadoVertical;
    int inicioY = escalado.margenInicialY + (escalado.altoVentanaMenu - escalado.margenInicialY - altoTotal) / 2;

    int anchoNickname = (int)(escalado.anchoVentanaMenu * 0.8f);
    SDL_Rect rectTitulo = { (escalado.anchoVentanaMenu - anchoNickname)/2, inicioY, anchoNickname, escalado.botonAlto };
    SDL_Rect rectTexto = { rectTitulo.x, rectTitulo.y + escalado.botonAlto + escalado.espaciadoVertical, anchoNickname, escalado.botonAlto };
    SDL_Rect botonJugar = { rectTexto.x, rectTexto.y + escalado.botonAlto + escalado.espaciadoVertical, anchoNickname, escalado.botonAlto };
    SDL_Rect botonVolver = { botonJugar.x, botonJugar.y + escalado.botonAlto + escalado.espaciadoVertical, anchoNickname, escalado.botonAlto };

    while (!salir)
    {
        while (SDL_PollEvent(&evento))
        {
            if (evento.type == SDL_QUIT)
                exit(0);
            else {
                if (evento.type == SDL_TEXTINPUT)
                {
                    if (strlen(nombreUsuario) < MAX_NOMBRE-1)
                        strcat(nombreUsuario, evento.text.text);
                }

                if (evento.type == SDL_KEYDOWN)
                {
                    if (evento.key.keysym.sym == SDLK_BACKSPACE && strlen(nombreUsuario) > 0)
                        nombreUsuario[strlen(nombreUsuario)-1] = '\0';
                }

                if (evento.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x = evento.button.x;
                    int y = evento.button.y;

                    if (x >= botonJugar.x && x <= botonJugar.x + botonJugar.w &&
                        y >= botonJugar.y && y <= botonJugar.y + botonJugar.h)
                    {
                        if (strlen(nombreUsuario) > 0) {
                            resultado = OPCION_NICKNAME_CONFIRMAR;
                            salir = true;
                        }
                    }

                    if (x >= botonVolver.x && x <= botonVolver.x + botonVolver.w &&
                        y >= botonVolver.y && y <= botonVolver.y + botonVolver.h)
                    {
                        resultado = OPCION_NICKNAME_VOLVER;
                        salir = true;
                    }
                }
             }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int frameLogo = (SDL_GetTicks() / 300) % LOGO_FRAMES;
        dibujarLogo(renderer, ventana, frameLogo);

        SDL_Color fondo = {70,70,70,255}, textoColor = {255, 255, 255, 255};

        SDL_Surface* surfaceTitulo = TTF_RenderText_Blended(fuente, "Ingrese su Nickname", textoColor);
        SDL_Texture* textureTitulo = SDL_CreateTextureFromSurface(renderer, surfaceTitulo);
        SDL_Rect rectTextoTitulo = {
            rectTitulo.x + (rectTitulo.w - surfaceTitulo->w)/2,
            rectTitulo.y + (rectTitulo.h - surfaceTitulo->h)/2,
            surfaceTitulo->w,
            surfaceTitulo->h
        };
        SDL_RenderCopy(renderer, textureTitulo, NULL, &rectTextoTitulo);
        SDL_FreeSurface(surfaceTitulo);
        SDL_DestroyTexture(textureTitulo);

        SDL_SetRenderDrawColor(renderer, 240, 180, 80, 255); 
        SDL_RenderFillRect(renderer, &rectTexto);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &rectTexto);

        if (strlen(nombreUsuario) > 0)
        {
            SDL_Surface* surfaceNombre = TTF_RenderText_Blended(fuente, nombreUsuario, textoColor);
            SDL_Texture* textureNombre = SDL_CreateTextureFromSurface(renderer, surfaceNombre);
            SDL_Rect rectNombre = {
                rectTexto.x + (rectTexto.w - surfaceNombre->w)/2,
                rectTexto.y + (rectTexto.h - surfaceNombre->h)/2,
                surfaceNombre->w,
                surfaceNombre->h
            };
            SDL_RenderCopy(renderer, textureNombre, NULL, &rectNombre);
            SDL_FreeSurface(surfaceNombre);
            SDL_DestroyTexture(textureNombre);
        }

        dibujarBotonPlano(renderer, botonJugar, fondo);
        dibujarTexto(renderer, fuente, "JUGAR", botonJugar, textoColor);

        dibujarBotonPlano(renderer, botonVolver, fondo);
        dibujarTexto(renderer, fuente, "VOLVER", botonVolver, textoColor);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    return resultado;
}

bool mostrarFlujoDeMenus(SDL_Renderer* renderer, SDL_Window* ventana,
                         opcionesMenuPrincipal* opcionPrincipal,
                         opcionesMenuTipoPartida* opcionTipo,
                         opcionesMenuDificultad* dificultad,
                         char nombreUsuario[MAX_NOMBRE])
{
    while (true) { //opciones menu principal
        *opcionPrincipal = mostrarMenuPrincipal(renderer, ventana, fuenteTexto);
        //si quiso salir, salgo del menu
        if (*opcionPrincipal == OPCION_PRINCIPAL_SALIR)
            return false;
        //si eligió estadísticas, muestro las estadísticas
        if (*opcionPrincipal == OPCION_PRINCIPAL_ESTADISTICAS) {
            mostrarEstadisticas(renderer, ventana, fuenteTexto, fuenteHUD);
            continue; // hace que el bucle vuelva al incicio DESDE EL MENÚ PRINCIPAL
        }
        //si llego hasta aca es porque eligió jugar
        if (*opcionPrincipal == OPCION_PRINCIPAL_JUGAR) {
            while (true) { //en este bucle manejamos el flujo de menus de partida
                // Menú tipo de partida
                *opcionTipo = mostrarMenuTipoPartida(renderer, ventana, fuenteTexto);
                if (*opcionTipo == OPCION_PARTIDA_MENU_PRINCIPAL)
                    break; // Corta y vuelve al menú principal porque el primer while vuelve a arrancar
                //si llego aca es porque eligió una partida nueva
                if (*opcionTipo == OPCION_PARTIDA_NUEVA) {
                    while (true) { //manejo de menús de partida nueva
                        // Menú dificultad
                        *dificultad = mostrarMenuDificultad(renderer, ventana, fuenteTexto);
                        if (*dificultad == DIFICULTAD_VOLVER)
                            break; // Volver al menú tipo de partida porque se repite el while mas cercano
                        while (true) {
                            // Menú nickname, donde pide el nick
                            opcionesMenuNickname opcionNick = mostrarMenuNickname(renderer, ventana, fuenteTexto, nombreUsuario);
                            if (opcionNick == OPCION_NICKNAME_VOLVER)
                                break; // Volver al menú dificultad pooorque vuelve al while mas cercano
                            // Si llegó acá, juega
                            return true;
                        }
                    }
                }
            }
        }
    }
}

void dibujarLogo(SDL_Renderer* renderer, SDL_Window* ventana, int frameLogo)
{
    SDL_Texture* textura = logo_texturas[frameLogo];

    SDL_Rect destino = {
        (escalado.anchoVentanaMenu - escalado.logoAncho) / 2,
        escalado.logoPosY,
        escalado.logoAncho,
        escalado.logoAlto
    };

    SDL_RenderCopy(renderer, textura, NULL, &destino);
}

void mostrarPantallaFin(SDL_Renderer* renderer, Juego* juego, bool gano)
{
    Uint32 tiempoInicio = SDL_GetTicks();  //tiempo actual
    Uint32 duracion = 2000; //duracion de la pantalla de fin en milisegundos

    while (SDL_GetTicks() - tiempoInicio < duracion) //dura 2 segs mientras no salga
    {
        SDL_Event evento;
        while (SDL_PollEvent(&evento))
        {
            if (evento.type == SDL_QUIT)
                exit(0);
        }

        //dibuja un fondo oscuro sobre todo el tablero y el HUD con el mensaje GANASTE o PERDISTE segun corresponda
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        //crea un rectangulo overlay que cubre toda la pantalla y hud, empieza borde izquierdo y borde superior y toma todo el ancho del tablero, luego toma el padding superior mas dimensiones y toma todo el alto
        SDL_Rect overlay = { 0, 0, juego->tamCasilla * juego->dimension, escalado.paddingSuperior + juego->tamCasilla * juego->dimension };
        SDL_RenderFillRect(renderer, &overlay); 

        //texto superpuesto
        const char* texto = gano ? ":) GANASTE" : " :( PERDISTE";
        SDL_Color colorTexto = {255, 150, 0, 255};
        SDL_Surface* sTexto = TTF_RenderText_Blended(fuenteHUD, texto, colorTexto);
        SDL_Texture* tTexto = SDL_CreateTextureFromSurface(renderer, sTexto);
        SDL_Rect rectTexto = { //centra texto
            (overlay.w - sTexto->w) / 2,
            (overlay.h - sTexto->h) / 2,
            sTexto->w,
            sTexto->h
        };
        SDL_RenderCopy(renderer, tTexto, NULL, &rectTexto);
        SDL_FreeSurface(sTexto);
        SDL_DestroyTexture(tTexto);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}


void liberarTodo(SDL_Renderer* renderer, SDL_Window* ventana)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

