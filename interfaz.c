#include "interfaz.h"
#include <stdio.h>

void inicializarSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        chequearError(-1, ERROR_SDL);
    }
    printf("SDL inicializado correctamente\n");
}

void inicializarSDLImagenes() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Error al inicializar SDL_image: %s\n", IMG_GetError());
        chequearError(-1, ERROR_SDL_IMAGE);
    }
}

void inicializarSDLTtf() {
    if (TTF_Init() == -1) {
        printf("Error al inicializar SDL_ttf: %s\n", TTF_GetError());
        chequearError(-1, ERROR_SDL_TTF);
    }
}

void inicializarSDLCompleto(){
    inicializarSDL();
    inicializarSDLImagenes();
    inicializarSDLTtf();
}

int crearVentana(SDL_Window** ventana, int width, int height)
{
    *ventana = SDL_CreateWindow("BuscaEsferas",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width,
                               height,0);

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
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    int anchoPantalla = displayMode.w;
    int altoPantalla  = displayMode.h;

    calcularEscaladoUI(&escalado, anchoPantalla, altoPantalla);

    chequearError(crearVentana(ventana, escalado.anchoVentanaMenu, escalado.altoVentanaMenu), NO_GENERO_VENTANA);
    chequearError(crearRenderer(renderer, ventana), NO_GENERO_RENDERER);
}


void dibujarTablero(SDL_Renderer* renderer, Juego* juego, TTF_Font* fuente, TTF_Font* fuenteHUD, int clicksCheat, bool cheatEnPeriodoActivo, Uint32 cheatActivadoTiempo)
{
    if (!renderer || !juego || !fuente || !fuenteHUD) {
        printf("Error: Renderer, juego o fuentes no inicializados correctamente.\n");
        return;
    }

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    SDL_Rect hud = { 0, 0, juego->tamCasilla * juego->dimension, escalado.paddingSuperior };
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderFillRect(renderer, &hud);

    char minasTexto[4];
    sprintf(minasTexto, "%03d", juego->totalMinas - juego->minasMarcadas);
    SDL_Color colorLCD = colores[CNH];
    SDL_Surface* surfaceMinas = TTF_RenderText_Blended(fuenteHUD, minasTexto, colorLCD);
    SDL_Texture* texturaMinas = SDL_CreateTextureFromSurface(renderer, surfaceMinas);
    SDL_Rect rectMinas = {20, 10, surfaceMinas->w, surfaceMinas->h};
    SDL_RenderCopy(renderer, texturaMinas, NULL, &rectMinas);
    SDL_FreeSurface(surfaceMinas);
    SDL_DestroyTexture(texturaMinas);

    int segundosTranscurridos = (juego->finalizado) ? (juego->tiempoFin - juego->tiempoInicio) / 1000 : (SDL_GetTicks() - juego->tiempoInicio) / 1000;
    if (segundosTranscurridos > 999) segundosTranscurridos = 999;
    char tiempoTexto[4];
    sprintf(tiempoTexto, "%03d", segundosTranscurridos);
    SDL_Surface* surfaceTiempo = TTF_RenderText_Blended(fuenteHUD, tiempoTexto, colorLCD);
    SDL_Texture* texturaTiempo = SDL_CreateTextureFromSurface(renderer, surfaceTiempo);
    int anchoVentana = juego->tamCasilla * juego->dimension;
    SDL_Rect rectTiempo = {anchoVentana - surfaceTiempo->w - 20, 10, surfaceTiempo->w, surfaceTiempo->h};
    SDL_RenderCopy(renderer, texturaTiempo, NULL, &rectTiempo);
    SDL_FreeSurface(surfaceTiempo);
    SDL_DestroyTexture(texturaTiempo);

    int centroX = anchoVentana / 2;
    int centroY = escalado.paddingSuperior / 2;
    int tamanioBoton = (int)(50 * escalado.escalaGlobal);
    if (tamanioBoton < 35) tamanioBoton = 35;

    SDL_Rect rectBoton = { centroX - tamanioBoton/2, centroY - tamanioBoton/2, tamanioBoton, tamanioBoton };

    if (cheatEnPeriodoActivo) {
        int frame = (SDL_GetTicks() / 200) % FRAMES_RASTREADOR;
        SDL_RenderCopy(renderer, rastreador_cheat_texturas[frame], NULL, &rectBoton);
    } else {
        int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_ESFERAS;
        int esferaActual = (clicksCheat % 7);
        if (esferaActual == 0) esferaActual = 1;
        SDL_RenderCopy(renderer, esferas_texturas[esferaActual][frameAnimacion], NULL, &rectBoton);
    }

    SDL_Rect celda = {0, 0, juego->tamCasilla, juego->tamCasilla};

    for (int fila = 0; fila < juego->dimension; fila++) {
        for (int col = 0; col < juego->dimension; col++) {
            celda.x = col * juego->tamCasilla;
            celda.y = escalado.paddingSuperior + fila * juego->tamCasilla;
            Casilla* casilla = &juego->tablero[fila][col];

            if (juego->finalizado && casilla->esMina) {
                if (fila == juego->minaExplotadaFila && col == juego->minaExplotadaCol) {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_MINA_EXPLOTADA;
                    SDL_RenderCopy(renderer, mina_explotada_texturas[frameAnimacion], NULL, &celda);
                } else {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_ESFERAS;
                    SDL_RenderCopy(renderer, esferas_texturas[casilla->esferaAlPerder][frameAnimacion], NULL, &celda);
                }
            }
            else if (juego->finalizado && casilla->revelada && !casilla->esMina) {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                SDL_RenderFillRect(renderer, &celda);
                if (casilla->minasVecinas > 0) {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_NUMEROS;
                    SDL_RenderCopy(renderer, numeros_texturas[casilla->minasVecinas][frameAnimacion], NULL, &celda);
                }
            }
            else if (casilla->revelada) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &celda);
                if (casilla->minasVecinas > 0) {
                    int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_NUMEROS;
                    SDL_RenderCopy(renderer, numeros_texturas[casilla->minasVecinas][frameAnimacion], NULL, &celda);
                }
            }
            else if (casilla->marcada) {
                int frameAnimacion = (SDL_GetTicks() / 200) % FRAMES_ESFERAS;
                SDL_RenderCopy(renderer, esferas_texturas[casilla->esfera][frameAnimacion], NULL, &celda);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
                SDL_RenderFillRect(renderer, &celda);
            }

            SDL_SetRenderDrawColor(renderer, colores[CN].r, colores[CN].g, colores[CN].b, colores[CN].a);
            SDL_RenderDrawRect(renderer, &celda);
        }
    }

    if (juego->minasColocadas && cheatEnPeriodoActivo && !juego->finalizado) {
        dibujarOverlayCheat(renderer, juego);
    }

    SDL_RenderPresent(renderer);
}

void dibujarOverlayCheat(SDL_Renderer* renderer, Juego* juego)
{
    for (int fila = 0; fila < juego->dimension; fila++) {
        for (int col = 0; col < juego->dimension; col++) {
            Casilla* casilla = &juego->tablero[fila][col];
            if (casilla->esMina && !casilla->marcada) {
                SDL_Rect destino = {
                    col * juego->tamCasilla,
                    escalado.paddingSuperior + fila * juego->tamCasilla,
                    juego->tamCasilla,
                    juego->tamCasilla
                };
                SDL_SetRenderDrawColor(renderer, 255, 180, 0, 200);
                printf("Pintando cheat en fila=%d col=%d\n", fila, col);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &destino);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
        }
    }
}


void dibujarFigura(SDL_Renderer* renderer, const int figura[8][8], int fila, int col,int tamPixel) {
    SDL_Rect pixel;
    pixel.w = tamPixel;
    pixel.h = tamPixel;

    int offsetX = col * tamPixel * 8;
    int offsetY = escalado.paddingSuperior + fila * tamPixel * 8;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int colorIndex = figura[i][j];
            SDL_Color color = colores[colorIndex];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            pixel.x = offsetX + j * tamPixel;
            pixel.y = offsetY + i * tamPixel;
            SDL_RenderFillRect(renderer, &pixel);
        }
    }
}

void dibujarBotonPlano(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color colorFondo) {
    SDL_SetRenderDrawColor(renderer, colorFondo.r, colorFondo.g, colorFondo.b, colorFondo.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void dibujarTexto(SDL_Renderer* renderer, TTF_Font* fuente, const char* texto, SDL_Rect rect, SDL_Color colorTexto) {
    SDL_Surface* surfaceTexto = TTF_RenderText_Blended(fuente, texto, colorTexto);
    if (!surfaceTexto) {
        printf("Error renderizando texto: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* textureTexto = SDL_CreateTextureFromSurface(renderer, surfaceTexto);
    if (!textureTexto) {
        printf("Error creando textura de texto: %s\n", SDL_GetError());
        SDL_FreeSurface(surfaceTexto);
        return;
    }

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
    SDL_Event evento;
    bool salir = false;

    int cantidadBotones = 3;
    int altoTotal = cantidadBotones * escalado.botonAlto + (cantidadBotones - 1) * escalado.espaciadoVertical;
    int inicioY = escalado.margenInicialY + (escalado.altoVentanaMenu - escalado.margenInicialY - altoTotal) / 2;

    SDL_Rect botonJugar = { (escalado.anchoVentanaMenu - escalado.botonAncho) / 2, inicioY, escalado.botonAncho, escalado.botonAlto };
    SDL_Rect botonEstadisticas = { botonJugar.x, inicioY + escalado.botonAlto + escalado.espaciadoVertical, escalado.botonAncho, escalado.botonAlto };
    SDL_Rect botonSalir = { botonEstadisticas.x, inicioY + 2*(escalado.botonAlto + escalado.espaciadoVertical), escalado.botonAncho, escalado.botonAlto };

    while (!salir) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT)
                return OPCION_PRINCIPAL_SALIR;
            else if (evento.type == SDL_MOUSEBUTTONDOWN) {
                int x = evento.button.x, y = evento.button.y;

                if (x >= botonJugar.x && x <= botonJugar.x + escalado.botonAncho && y >= botonJugar.y && y <= botonJugar.y + escalado.botonAlto)
                    return OPCION_PRINCIPAL_JUGAR;

                if (x >= botonEstadisticas.x && x <= botonEstadisticas.x + escalado.botonAncho && y >= botonEstadisticas.y && y <= botonEstadisticas.y + escalado.botonAlto)
                    return OPCION_PRINCIPAL_ESTADISTICAS;

                if (x >= botonSalir.x && x <= botonSalir.x + escalado.botonAncho && y >= botonSalir.y && y <= botonSalir.y + escalado.botonAlto)
                    return OPCION_PRINCIPAL_SALIR;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int frameLogo = (SDL_GetTicks() / 300) % LOGO_FRAMES;
        dibujarLogo(renderer, ventana, frameLogo);

        SDL_Color fondo = {70,70,70,255}, texto = {255,255,255,255};

        dibujarBotonPlano(renderer, botonJugar, fondo);
        dibujarBotonPlano(renderer, botonEstadisticas, fondo);
        dibujarBotonPlano(renderer, botonSalir, fondo);

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
                return OPCION_PARTIDA_MENU_PRINCIPAL;
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
                return DIFICULTAD_VOLVER;
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
        SDL_Delay(16);
    }
    return DIFICULTAD_VOLVER;
}

void pedirNombreUsuario(SDL_Renderer* renderer, TTF_Font* fuente, char* nombreUsuario) {
    SDL_StartTextInput();

    nombreUsuario[0] = '\0';
    int salir = 0;
    SDL_Event evento;

    while (!salir) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                exit(0);
            }
            if (evento.type == SDL_TEXTINPUT) {
                if (strlen(nombreUsuario) < MAX_NOMBRE - 1) {
                    strcat(nombreUsuario, evento.text.text);
                }
            }
            if (evento.type == SDL_KEYDOWN) {
                if (evento.key.keysym.sym == SDLK_BACKSPACE && strlen(nombreUsuario) > 0) {
                    nombreUsuario[strlen(nombreUsuario) - 1] = '\0';
                }
                if (evento.key.keysym.sym == SDLK_RETURN && strlen(nombreUsuario) > 0) {
                    salir = 1;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Color color = {255, 255, 255, 255};

        SDL_Surface* superficieTexto = TTF_RenderText_Blended(fuente, "Ingrese su nombre:", color);
        SDL_Texture* texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);
        SDL_Rect rect = {100, 100, superficieTexto->w, superficieTexto->h};
        SDL_RenderCopy(renderer, texturaTexto, NULL, &rect);
        SDL_FreeSurface(superficieTexto);
        SDL_DestroyTexture(texturaTexto);

        superficieTexto = TTF_RenderText_Blended(fuente, nombreUsuario, color);
        texturaTexto = SDL_CreateTextureFromSurface(renderer, superficieTexto);
        rect.y = 200;
        rect.w = superficieTexto->w;
        rect.h = superficieTexto->h;
        SDL_RenderCopy(renderer, texturaTexto, NULL, &rect);
        SDL_FreeSurface(superficieTexto);
        SDL_DestroyTexture(texturaTexto);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
}

opcionesMenuNickname mostrarMenuNickname(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuente, char* nombreUsuario)
{
    printf("ENTRE AL MENU NICKNAME\n");

    SDL_StartTextInput();

    nombreUsuario[0] = '\0';
    SDL_Event evento;
    bool salir = false;
    opcionesMenuNickname resultado = OPCION_NICKNAME_VOLVER;

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
                          char* nombreUsuario)
{
    *opcionPrincipal = mostrarMenuPrincipal(renderer, ventana, fuenteTexto);

    if (*opcionPrincipal == OPCION_PRINCIPAL_SALIR)
        return false;

    if (*opcionPrincipal == OPCION_PRINCIPAL_ESTADISTICAS)
    {
        mostrarEstadisticas(renderer, ventana, fuenteTexto, fuenteHUD);
        return true;  
    }

    *opcionTipo = mostrarMenuTipoPartida(renderer, ventana, fuenteTexto);
    if (*opcionTipo == OPCION_PARTIDA_MENU_PRINCIPAL)
        return true;

    *dificultad = mostrarMenuDificultad(renderer, ventana, fuenteTexto);
    if (*dificultad == DIFICULTAD_VOLVER)
        return true;

    if (*dificultad != DIFICULTAD_CUSTOM)
    {
        opcionesMenuNickname opcionNick = mostrarMenuNickname(renderer, ventana, fuenteTexto, nombreUsuario);
        if (opcionNick == OPCION_NICKNAME_VOLVER)
            return true;
    }

    return true;
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
    Uint32 tiempoInicio = SDL_GetTicks();
    Uint32 duracion = 2000;

    while (SDL_GetTicks() - tiempoInicio < duracion)
    {
        SDL_Event evento;
        while (SDL_PollEvent(&evento))
        {
            if (evento.type == SDL_QUIT)
                exit(0);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect overlay = { 0, 0, juego->tamCasilla * juego->dimension, escalado.paddingSuperior + juego->tamCasilla * juego->dimension };
        SDL_RenderFillRect(renderer, &overlay);

        const char* texto = gano ? ":) GANASTE" : " :( PERDISTE";
        SDL_Color colorTexto = {255, 150, 0, 255};
        SDL_Surface* sTexto = TTF_RenderText_Blended(fuenteHUD, texto, colorTexto);
        SDL_Texture* tTexto = SDL_CreateTextureFromSurface(renderer, sTexto);
        SDL_Rect rectTexto = {
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

