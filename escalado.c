#include "escalado.h"


EscaladoUI escalado;

void calcularEscaladoUI(EscaladoUI* escalado, int anchoPantalla, int altoPantalla)
{
    float porcentajeMenuAlto  = 0.70f;  
    float porcentajeMenuAncho = porcentajeMenuAlto * 0.45f; 


    escalado->anchoVentanaMenu = (int)(anchoPantalla * porcentajeMenuAncho);
    escalado->altoVentanaMenu  = (int)(altoPantalla  * porcentajeMenuAlto);

    float escalaCalculada = (float)escalado->altoVentanaMenu / ALTURA_VENTANA_BASE;
    if (escalaCalculada < 0.75f)
        escalaCalculada = 0.75f; 

    escalado->escalaGlobal = escalaCalculada;

    escalado->logoAncho = (int)(240 * escalado->escalaGlobal * 1.8f);
    escalado->logoAlto  = (int)(160 * escalado->escalaGlobal * 1.8f);
    if (escalado->logoAncho < 200) escalado->logoAncho = 200;
    if (escalado->logoAlto  < 100) escalado->logoAlto  = 100;

    escalado->logoPosY  = (int)(40  * escalado->escalaGlobal);

    escalado->botonAncho = (int)(300 * escalado->escalaGlobal);
    escalado->botonAlto  = (int)(60  * escalado->escalaGlobal);
    escalado->espaciadoVertical = (int)(20 * escalado->escalaGlobal);
    escalado->margenInicialY = (int)((escalado->logoPosY + escalado->logoAlto + 40) * escalado->escalaGlobal);
    escalado->tamanioFuenteTexto = (int)(24 * escalado->escalaGlobal);
    escalado->tamanioFuenteHUD   = (int)(36 * escalado->escalaGlobal);
    escalado->paddingSuperior = (int)(PADDING_SUPERIOR_BASE * escalado->escalaGlobal);
}
