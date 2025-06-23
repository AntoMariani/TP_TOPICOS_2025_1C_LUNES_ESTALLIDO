#include "estadisticas.h"

// Lee las estadísticas desde el archivo y las guarda en el array stats[].
// Devuelve la cantidad de registros leídos (máximo maxStats).
int leerEstadisticas(Estadistica stats[], int maxStats) {
    FILE* f = fopen("estadisticas.txt", "r"); // Abre el archivo en modo lectura
    if (!f) return 0; // Si no existe, retorna 0

    int count = 0;
    // Lee cada línea con formato: dificultad;usuario;tiempo
    while (fscanf(f, "%15[^;];%15[^;];%d\n",
                  stats[count].dificultad,
                  stats[count].usuario,
                  &stats[count].tiempoSegundos) == 3) {
        count++;
        if (count >= maxStats) break; // No lee más de maxStats registros
    }
    fclose(f);
    return count; // Devuelve la cantidad de registros leídos
}

// Actualiza las estadísticas agregando un nuevo registro y manteniendo el ranking ordenado
void actualizarEstadisticas(const char* dificultad, const char* nombre, int tiempo) {
    Estadistica todas[MAX_ESTAD_TOTALES];
    int total = leerEstadisticas(todas, MAX_ESTAD_TOTALES); // Lee todas las estadísticas existentes

    Estadistica nivel[MAX_ESTAD_TOTALES];
    int nivel_count = 0;

    // Filtra solo las estadísticas de la dificultad actual
    for (int i = 0; i < total; i++) {
        if (strcmp(todas[i].dificultad, dificultad) == 0) {
            nivel[nivel_count++] = todas[i];
        }
    }

    // Agrega la nueva estadística
    Estadistica nueva;
    strcpy(nueva.dificultad, dificultad);
    strcpy(nueva.usuario, nombre);
    nueva.tiempoSegundos = tiempo;
    nivel[nivel_count++] = nueva;

    // Ordena las estadísticas de la dificultad actual por tiempo (menor a mayor)
    for (int i = 0; i < nivel_count - 1; i++) {
        for (int j = i + 1; j < nivel_count; j++) {
            if (nivel[j].tiempoSegundos < nivel[i].tiempoSegundos) {
                Estadistica aux = nivel[i];
                nivel[i] = nivel[j];
                nivel[j] = aux;
            }
        }
    }

    // Limita la cantidad de registros a MAX_ESTADISTICAS
    if (nivel_count > MAX_ESTADISTICAS)
        nivel_count = MAX_ESTADISTICAS;

    FILE* f = fopen("estadisticas.txt", "w"); // Abre el archivo para sobrescribirlo
    if (!f) {
        printf("Error guardando estadisticas!\n");
        return;
    }

    // Escribe las estadísticas de otras dificultades (no la actual)
    for (int i = 0; i < total; i++) {
        if (strcmp(todas[i].dificultad, dificultad) != 0) {
            fprintf(f, "%s;%s;%d\n", todas[i].dificultad, todas[i].usuario, todas[i].tiempoSegundos);
        }
    }

    // Escribe las mejores estadísticas de la dificultad actual (ordenadas)
    for (int i = 0; i < nivel_count; i++) {
        fprintf(f, "%s;%s;%d\n", nivel[i].dificultad, nivel[i].usuario, nivel[i].tiempoSegundos);
    }

    fclose(f);
}

// Muestra la pantalla de estadísticas con los rankings por dificultad
void mostrarEstadisticas(SDL_Renderer* renderer, SDL_Window* ventana, TTF_Font* fuenteTexto, TTF_Font* fuenteHUD)
{
    SDL_Event evento;
    bool salir = false;

    // Nombres de las dificultades
    const char* dificultades[] = {"FACIL", "MEDIO", "DIFICIL", "SSJ"};

    // Ranking: para cada dificultad, guarda hasta MAX_ENTRADAS
    EntradaRanking ranking[4][MAX_ENTRADAS];
    int cantidadPorNivel[4] = {0};

    // Inicializa el ranking con valores por defecto
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < MAX_ENTRADAS; j++) {
            strcpy(ranking[i][j].nombre, "-");
            ranking[i][j].tiempo = -1;
        }
    }

    // Lee el archivo de estadísticas y llena el ranking
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

        // Busca a qué dificultad corresponde y lo agrega al ranking
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

    // Define el botón "VOLVER"
    SDL_Rect botonVolver = { 
        (escalado.anchoVentanaMenu - escalado.botonAncho) / 2, 
        escalado.altoVentanaMenu - (int)(escalado.botonAlto * 1.6f),
        escalado.botonAncho, escalado.botonAlto 
    };

    // Bucle principal de la pantalla de estadísticas
    while (!salir)
        {
            // Procesa eventos (cerrar ventana o click en "VOLVER")
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

            // Limpia la pantalla
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderClear(renderer);

            SDL_Color colorNaranja = {255, 150, 0, 255};
            SDL_Color textoColor = {255, 255, 255, 255};

            // Calcula tamaños y posiciones para los textos
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

            // Dibuja el título principal "ESTADISTICAS"
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

            // Dibuja los rankings para cada dificultad
            for (int i = 0; i < 4; i++)
            {
                // Dibuja el nombre de la dificultad
                SDL_Surface* sDif = TTF_RenderText_Blended(fuenteTexto, dificultades[i], colorNaranja);
                SDL_Texture* tDif = SDL_CreateTextureFromSurface(renderer, sDif);
                SDL_Rect rDif = { (escalado.anchoVentanaMenu - sDif->w) / 2, posY, sDif->w, sDif->h };
                SDL_RenderCopy(renderer, tDif, NULL, &rDif);
                SDL_FreeSurface(sDif);
                SDL_DestroyTexture(tDif);

                posY += alturaTitulo;

                // Dibuja cada entrada del ranking
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

            // Dibuja el botón "VOLVER"
            dibujarBotonPlano(renderer, botonVolver, (SDL_Color){70,70,70,255});
            dibujarTexto(renderer, fuenteTexto, "VOLVER", botonVolver, textoColor);

            // Actualiza la pantalla
            SDL_RenderPresent(renderer);
        }
}
