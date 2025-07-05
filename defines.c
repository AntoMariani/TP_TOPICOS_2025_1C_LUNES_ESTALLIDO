#include "defines.h"

void chequearError(int error, int errorEsperado)
{
    if(error == errorEsperado)
    {
        exit(error);
    }
}

SDL_Color colores[] =
{
    {0, 0, 0, 255},             // N[0] - Negro
    {255, 255, 0, 255},         // Y[1] - Amarillo
    {255, 255, 255, 255},       // B[2] - Blanco
    {0, 150, 60, 255},          // V[3] - Verde
    {255, 0, 0, 255},           // R[4] - Rojo
    {0, 0, 255, 255},           // R[5] - Azul
    {255, 105, 180, 255},       // R[6] - Rosa
    {254, 195, 29, 255},         // N[7] - Naranja HUD
    {5, 135,  3, 255},        // 8 - Verde esferas
    {254, 163,  0, 255},        // 9 - Naranja Goku
    {0, 108,124, 255},        // 10 - Cian radar
    {252,  16,  0, 255},        // 11 - Rojo alerta
    {110, 110,110, 255},        // 12 - Casilla oculta
    {200, 180,160, 255},         // 13 - Casilla revelada
    {70,70,70,255}, //14 - color fondo botones
    {255, 150, 0, 255}, //15 NARANJA FUERTE
    {100, 100, 100, 255} //16 botones rehacer y deshacer
};
