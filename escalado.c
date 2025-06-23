#include "escalado.h"

//variable global para almacenar todos los datos del escalado
EscaladoUI escalado;

void calcularEscaladoUI(EscaladoUI* escalado, int anchoPantalla, int altoPantalla)
{
    float porcentajeMenuAlto  = 0.70f; //es lo mismo que = 0.70; pero entendemos que de esta manera no hace conversion de double a float
    float porcentajeMenuAncho = porcentajeMenuAlto * 0.45f; //el ancho del menu es el 45% del alto del menu, los numeros correctos los sacamos jugando con los mismos

    //calculamos el ancho y alto de la ventana de los menus en base a la resolucion que calculamos antes de pantalla, nos da los pixeles
    //casteamos a int porque SDL usa enteros para dimensionar ventanas y demases
    escalado->anchoVentanaMenu = (int)(anchoPantalla * porcentajeMenuAncho);
    escalado->altoVentanaMenu  = (int)(altoPantalla  * porcentajeMenuAlto);

    //esto fue un lio, pero dividimos el alto del menu por un valor base, porque nosotros ya habiamos hecho gran parte del codigo
    //con un alto de ventana base de 800, pero como nos dimos cuenta que precisamos escalarlo, hicimos eso
    //y de paso limitamos que no pueda quedar todo muy chiquito en pantallas chicas
    float escalaCalculada = (float)escalado->altoVentanaMenu / ALTURA_VENTANA_BASE;
    if (escalaCalculada < 0.75f)
        escalaCalculada = 0.75f; 

    //esto lo usamos para calcular los botones y demases
    escalado->escalaGlobal = escalaCalculada;

    //calculamos el ancho y el alto del logo en pixeles, como nos quedaba chiquito para nuestro gusto, lo subimos un poco multiplicandolo por 1.8f
    escalado->logoAncho = (int)(240 * escalado->escalaGlobal * 1.8f);
    escalado->logoAlto  = (int)(160 * escalado->escalaGlobal * 1.8f);

    //limitamos el ancho y alto del logo a un minimo de 200x100, porque si no se ve muy chiquito en pantallas chicas
    if (escalado->logoAncho < 200)
        escalado->logoAncho = 200;
    if (escalado->logoAlto  < 100)
        escalado->logoAlto  = 100;
    
    escalado->logoPosY  = (int)(40  * escalado->escalaGlobal); //posicionamos el logo en la parte superior de la ventana, con un margen de 40px desde arriba
    escalado->botonAncho = (int)(300 * escalado->escalaGlobal); //ancho de los botones, arrancando desde 300px
    escalado->botonAlto  = (int)(60  * escalado->escalaGlobal); //alto de los botones, arrancando desde 60px
    escalado->espaciadoVertical = (int)(20 * escalado->escalaGlobal); //espaciado vertical entre los botones, arrancando desde 20px

    escalado->margenInicialY = (int)((escalado->logoPosY + escalado->logoAlto + 40) * escalado->escalaGlobal); //calculamos donde tienen que arrancar los botones, debajo del logo y con un margen de 40px
    escalado->tamanioFuenteTexto = (int)(24 * escalado->escalaGlobal); //tamaño de la fuente del texto, arrancando desde 24px
    escalado->tamanioFuenteHUD   = (int)(36 * escalado->escalaGlobal); //tamaño de la fuente del HUD, arrancando desde 36px
    escalado->paddingSuperior = (int)(PADDING_SUPERIOR_BASE * escalado->escalaGlobal); //para que la interfaz no quede pegada al borde superior
}
