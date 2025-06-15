#include "estadisticas.h"

int leerEstadisticas(Estadistica stats[], int maxStats) {
    FILE* f = fopen("estadisticas.txt", "r");
    if (!f) return 0;

    int count = 0;
    while (fscanf(f, "%15[^;];%15[^;];%d\n",
                  stats[count].dificultad,
                  stats[count].usuario,
                  &stats[count].tiempoSegundos) == 3) {
        count++;
        if (count >= maxStats) break;
    }
    fclose(f);
    return count;
}

void actualizarEstadisticas(const char* dificultad, const char* nombre, int tiempo) {
    Estadistica todas[MAX_ESTAD_TOTALES];
    int total = leerEstadisticas(todas, MAX_ESTAD_TOTALES);

    Estadistica nivel[MAX_ESTAD_TOTALES];
    int nivel_count = 0;

    for (int i = 0; i < total; i++) {
        if (strcmp(todas[i].dificultad, dificultad) == 0) {
            nivel[nivel_count++] = todas[i];
        }
    }

    Estadistica nueva;
    strcpy(nueva.dificultad, dificultad);
    strcpy(nueva.usuario, nombre);
    nueva.tiempoSegundos = tiempo;
    nivel[nivel_count++] = nueva;

    for (int i = 0; i < nivel_count - 1; i++) {
        for (int j = i + 1; j < nivel_count; j++) {
            if (nivel[j].tiempoSegundos < nivel[i].tiempoSegundos) {
                Estadistica aux = nivel[i];
                nivel[i] = nivel[j];
                nivel[j] = aux;
            }
        }
    }

    if (nivel_count > MAX_ESTADISTICAS)
        nivel_count = MAX_ESTADISTICAS;

    FILE* f = fopen("estadisticas.txt", "w");
    if (!f) {
        printf("Error guardando estadisticas!\n");
        return;
    }

    for (int i = 0; i < total; i++) {
        if (strcmp(todas[i].dificultad, dificultad) != 0) {
            fprintf(f, "%s;%s;%d\n", todas[i].dificultad, todas[i].usuario, todas[i].tiempoSegundos);
        }
    }

    for (int i = 0; i < nivel_count; i++) {
        fprintf(f, "%s;%s;%d\n", nivel[i].dificultad, nivel[i].usuario, nivel[i].tiempoSegundos);
    }

    fclose(f);
}

void mostrarEstadisticas(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuenteTexto, TTF_Font* fuenteHUD)
{
    SDL_Event evento;
    bool salir = false;

    const char* dificultades[] = {"FACIL", "MEDIO", "DIFICIL", "SSJ"};

    EntradaRanking ranking[4][MAX_ENTRADAS];
    int cantidadPorNivel[4] = {0};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < MAX_ENTRADAS; j++) {
            strcpy(ranking[i][j].nombre, "-");
            ranking[i][j].tiempo = -1;
        }
    }

    FILE* archivo = fopen("estadisticas.txt", "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de estadísticas.\n");
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo))
    {
        char nivel[32], nombre[32];
        int tiempo;
        sscanf(linea, "%[^;];%[^;];%d", nivel, nombre, &tiempo);

        for (int i = 0; i < 4; i++)
        {
            if (strcmp(nivel, dificultades[i]) == 0)
            {
                int pos = cantidadPorNivel[i];
                if (pos < MAX_ENTRADAS) {
                    strcpy(ranking[i][pos].nombre, nombre);
                    ranking[i][pos].tiempo = tiempo;
                    cantidadPorNivel[i]++;
                }
            }
        }
    }
    fclose(archivo);

    SDL_Rect botonVolver = { 
        (escalado.anchoVentanaMenu - escalado.botonAncho) / 2, 
        escalado.altoVentanaMenu - (int)(escalado.botonAlto * 1.6f),
        escalado.botonAncho, escalado.botonAlto 
    };

    while (!salir)
        {
            while (SDL_PollEvent(&evento))
            {
                if (evento.type == SDL_QUIT)
                    exit(0);
                else if (evento.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x = evento.button.x;
                    int y = evento.button.y;
                    if (x >= botonVolver.x && x <= botonVolver.x + escalado.botonAncho &&
                        y >= botonVolver.y && y <= botonVolver.y + escalado.botonAlto)
                    {
                        salir = true;
                    }
                }
            }

            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderClear(renderer);

            SDL_Color colorNaranja = {255, 150, 0, 255};
            SDL_Color textoColor = {255, 255, 255, 255};

            int alturaTitulo = (int)(30 * escalado.escalaGlobal);
            int alturaRegistro = (int)(22 * escalado.escalaGlobal);
            int espacioEntreNiveles = (int)(16 * escalado.escalaGlobal);

            int altoTotal = 0;
            for (int i = 0; i < 4; i++) {
                altoTotal += alturaTitulo;
                altoTotal += (MAX_ENTRADAS * alturaRegistro);
                altoTotal += espacioEntreNiveles;
            }

            int margenSuperiorTitulo = (int)(escalado.altoVentanaMenu * 0.06f);
            int posY = margenSuperiorTitulo;

            int tamanioTitulo = (int)(escalado.tamanioFuenteTexto * 1.5f);
            TTF_Font* fuenteTitulo = TTF_OpenFont("fnt/DragonBall.ttf", tamanioTitulo);
            SDL_Surface* sTitulo = TTF_RenderText_Blended(fuenteTitulo, "ESTADISTICAS", colorNaranja);
            SDL_Texture* tTitulo = SDL_CreateTextureFromSurface(renderer, sTitulo);
            SDL_Rect rTitulo = { (escalado.anchoVentanaMenu - sTitulo->w) / 2, posY, sTitulo->w, sTitulo->h };
            SDL_RenderCopy(renderer, tTitulo, NULL, &rTitulo);
            SDL_FreeSurface(sTitulo);
            SDL_DestroyTexture(tTitulo);
            TTF_CloseFont(fuenteTitulo);

            posY += rTitulo.h + (int)(30 * escalado.escalaGlobal);

            for (int i = 0; i < 4; i++)
            {
                SDL_Surface* sDif = TTF_RenderText_Blended(fuenteTexto, dificultades[i], colorNaranja);
                SDL_Texture* tDif = SDL_CreateTextureFromSurface(renderer, sDif);
                SDL_Rect rDif = { (escalado.anchoVentanaMenu - sDif->w) / 2, posY, sDif->w, sDif->h };
                SDL_RenderCopy(renderer, tDif, NULL, &rDif);
                SDL_FreeSurface(sDif);
                SDL_DestroyTexture(tDif);

                posY += alturaTitulo;

                for (int j = 0; j < MAX_ENTRADAS; j++)
                {
                    char buffer[64];
                    if (ranking[i][j].tiempo >= 0)
                        sprintf(buffer, "%-10s %5d", ranking[i][j].nombre, ranking[i][j].tiempo);
                    else
                        sprintf(buffer, "%-10s %5s", "-", "-");

                    SDL_Surface* sLin = TTF_RenderText_Blended(fuenteHUD, buffer, textoColor);
                    SDL_Texture* tLin = SDL_CreateTextureFromSurface(renderer, sLin);
                    SDL_Rect rLin = { (escalado.anchoVentanaMenu - sLin->w) / 2, posY, sLin->w, sLin->h };
                    SDL_RenderCopy(renderer, tLin, NULL, &rLin);
                    SDL_FreeSurface(sLin);
                    SDL_DestroyTexture(tLin);

                    posY += alturaRegistro + (int)(4 * escalado.escalaGlobal);
                }

                posY += espacioEntreNiveles;
            }

            dibujarBotonPlano(renderer, botonVolver, (SDL_Color){70,70,70,255});
            dibujarTexto(renderer, fuenteTexto, "VOLVER", botonVolver, textoColor);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
}
