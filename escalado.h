#ifndef ESCALADO_H_INCLUDED
#define ESCALADO_H_INCLUDED

#define ALTURA_VENTANA_BASE 800
#define PADDING_SUPERIOR_BASE 60

typedef struct {
    float escalaGlobal;
    int anchoVentanaMenu;
    int altoVentanaMenu;
    int logoAncho;
    int logoAlto;
    int logoPosY;
    int botonAncho;
    int botonAlto;
    int espaciadoVertical;
    int margenInicialY;
    int tamanioFuenteTexto;
    int tamanioFuenteHUD;
    int paddingSuperior;
} EscaladoUI;

extern EscaladoUI escalado;

void calcularEscaladoUI(EscaladoUI* escalado, int anchoPantalla, int altoPantalla);

#endif
