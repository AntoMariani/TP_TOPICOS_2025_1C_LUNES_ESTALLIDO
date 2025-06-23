#ifndef ESCALADO_H_INCLUDED
#define ESCALADO_H_INCLUDED

#define ALTURA_VENTANA_BASE 800
#define PADDING_SUPERIOR_BASE 60

//la estructura EscaladoUI tiene todos los datos necesarios para el escalado de la UI
typedef struct {
    float escalaGlobal; //factor de escalado global, se calcula en base a la resolucion de pantalla
    int anchoVentanaMenu; //ancho de la ventana del menu, se calcula en base a la resolucion de pantalla
    int altoVentanaMenu; //alto de la ventana del menu, se calcula en base a la resolucion de pantalla
    int logoAncho; //ancho del logo, se calcula en base a la resolucion de pantalla y el factor de escalado
    int logoAlto; //alto del logo, se calcula en base a la resolucion de pantalla y el factor de escalado
    int logoPosY; //posicion Y del logo, se calcula en base al alto de la ventana y el factor de escalado
    int botonAncho; //ancho de los botones, se calcula en base al factor de escalado
    int botonAlto; //alto de los botones, se calcula en base al factor de escalado
    int espaciadoVertical; //espaciado vertical entre los botones, se calcula en base al factor de escalado
    int margenInicialY; //margen inicial Y para los botones, se calcula en base a la posicion del logo y el factor de escalado
    int tamanioFuenteTexto; 
    int tamanioFuenteHUD;
    int paddingSuperior; // //padding superior para que la interfaz no quede pegada al borde superior
} EscaladoUI;

//la hacemos extern para que sea accesible desde otros archivos
extern EscaladoUI escalado;

void calcularEscaladoUI(EscaladoUI* escalado, int anchoPantalla, int altoPantalla);

#endif
