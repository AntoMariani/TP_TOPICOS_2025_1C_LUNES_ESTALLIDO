#include "dibujos.h"
#include "interfaz.h"

// Texturas globales
SDL_Texture* logo_texturas[LOGO_FRAMES] = {NULL};
SDL_Texture* numeros_texturas[TOTAL_NUMEROS + 1][FRAMES_NUMEROS] = {{NULL}};
SDL_Texture* esferas_texturas[TOTAL_ESFERAS + 1][FRAMES_NUMEROS] = {{NULL}};
SDL_Texture* mina_explotada_texturas[FRAMES_MINA_EXPLOTADA] = {NULL};
SDL_Texture* rastreador_cheat_texturas[FRAMES_RASTREADOR] = {NULL};
TTF_Font* fuenteTexto = NULL;
TTF_Font* fuenteHUD = NULL;

SDL_Color colores[] =
{
    {0, 0, 0, 255},             // N[0] - Negro
    {255, 255, 0, 255},         // Y[1] - Amarillo
    {255, 255, 255, 255},       // B[2] - Blanco
    {0, 150, 60, 255},          // V[3] - Verde
    {255, 0, 0, 255},           // R[4] - Rojo
    {0, 0, 255, 255},           // R[5] - Azul
    {255, 105, 180, 255},       // R[6] - Rosa
    {254, 195, 29, 255}         // N[7] - Naranja HUD
};

int cargarAnimaciones(SDL_Renderer* renderer, SDL_Texture* texturas[][FRAMES_NUMEROS], int totalNumeros, int totalFrames, const char* formatoPath)
{
    char path[64];

    for (int num = 1; num <= totalNumeros; num++) {
        for (int frame = 0; frame < totalFrames; frame++) {
            snprintf(path, sizeof(path), formatoPath, num, frame);
            SDL_Surface* superficie = IMG_Load(path);
            if (!superficie) {
                printf("Error cargando %s: %s\n", path, IMG_GetError());
                return 0;
            }
            texturas[num][frame] = SDL_CreateTextureFromSurface(renderer, superficie);
            SDL_FreeSurface(superficie);
        }
    }
    return 1;
}

int cargarFramesSimples(SDL_Renderer* renderer, SDL_Texture* texturas[], int totalFrames, const char* formatoPath)
{
    char path[64];

    for (int frame = 0; frame < totalFrames; frame++) {
        snprintf(path, sizeof(path), formatoPath, frame);
        SDL_Surface* superficie = IMG_Load(path);
        if (!superficie) {
            printf("Error cargando %s: %s\n", path, IMG_GetError());
            return 0;
        }
        texturas[frame] = SDL_CreateTextureFromSurface(renderer, superficie);
        SDL_FreeSurface(superficie);
    }
    return 1;
}

int cargarTodosLosRecursos(SDL_Renderer* renderer)
{
    if (!cargarAnimaciones(renderer, numeros_texturas, TOTAL_NUMEROS, FRAMES_NUMEROS, "img/numero_%d_%d.png"))
        return NO_CARGO_RECURSOS;
    if (!cargarAnimaciones(renderer, esferas_texturas, TOTAL_ESFERAS, FRAMES_NUMEROS, "img/esfera_%d_%d.png"))
        return NO_CARGO_RECURSOS;
    if (!cargarFramesSimples(renderer, logo_texturas, LOGO_FRAMES, "img/logo_%d.png"))
        return NO_CARGO_RECURSOS;
    if (!cargarFramesSimples(renderer, rastreador_cheat_texturas, FRAMES_RASTREADOR, "img/rastreador_cheat_%d.png"))
        return NO_CARGO_RECURSOS;
    if (!cargarFramesSimples(renderer, mina_explotada_texturas, FRAMES_MINA_EXPLOTADA, "img/mina_explotada_%d.png"))
        return NO_CARGO_RECURSOS;
    return TODO_OK;
}

void liberarAnimaciones(SDL_Texture* texturas[][FRAMES_NUMEROS], int totalNumeros, int totalFrames)
{
    for (int num = 1; num <= totalNumeros; num++) {
        for (int frame = 0; frame < totalFrames; frame++) {
            if (texturas[num][frame]) {
                SDL_DestroyTexture(texturas[num][frame]);
                texturas[num][frame] = NULL;
            }
        }
    }
}

void liberarFramesSimples(SDL_Texture* texturas[], int totalFrames)
{
    for (int frame = 0; frame < totalFrames; frame++) {
        if (texturas[frame]) {
            SDL_DestroyTexture(texturas[frame]);
            texturas[frame] = NULL;
        }
    }
}

void liberarTodosLosRecursos()
{
    liberarAnimaciones(numeros_texturas, TOTAL_NUMEROS, FRAMES_NUMEROS);
    liberarAnimaciones(esferas_texturas, TOTAL_ESFERAS, FRAMES_NUMEROS);
    liberarFramesSimples(logo_texturas, LOGO_FRAMES);
    liberarFramesSimples(rastreador_cheat_texturas, FRAMES_RASTREADOR);
    liberarFramesSimples(mina_explotada_texturas, FRAMES_MINA_EXPLOTADA);
}

void cargarFuentes()
{
    fuenteTexto = TTF_OpenFont("fnt/DragonBall.ttf", escalado.tamanioFuenteTexto );
    if (!fuenteTexto)
    {
        printf("Error al cargar fuenteTexto: %s\n", TTF_GetError());
        chequearError(ERROR_SDL_TTF, ERROR_SDL_TTF);
    }

    fuenteHUD = TTF_OpenFont("fnt/DS-DIGIT.ttf", escalado.tamanioFuenteHUD);
    if (!fuenteHUD)
    {
        printf("Error al cargar fuenteHUD: %s\n", TTF_GetError());
        chequearError(ERROR_SDL_TTF, ERROR_SDL_TTF);
    }
}


void liberarFuentes()
{
    if (fuenteTexto != NULL) {
        TTF_CloseFont(fuenteTexto);
        fuenteTexto = NULL;
    }
    if (fuenteHUD != NULL) {
        TTF_CloseFont(fuenteHUD);
        fuenteHUD = NULL;
    }
}
