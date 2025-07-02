#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MIN_DIMENSION 8
#define MAX_DIMENSION 32

#define TAM_CASILLA_FIJA 32

#define MAX_ANCHO  1360
#define MAX_ALTO    728


#define ALTURA_VENTANA_BASE 800

#define TODO_OK 0
#define ERROR_SDL -990
#define ERROR_SDL_TTF -991
#define ERROR_SDL_IMAGE -992
#define NO_GENERO_VENTANA -993
#define NO_GENERO_RENDERER -994
#define NO_ABRIO_ARCHIVO -995
#define NO_SE_PUDO_CREAR_LOG -996
#define ERROR_LOG_ESCRITURA -997
#define ERROR_DIMENSION -998
#define NO_ASIGNO_MEM_TABLERO -999
#define NO_ASIGNO_MEM_FILA_TABLERO -1000
#define ERROR_CANTIDAD_MINAS -1001
#define ERROR_FORMATO -1003
#define NO_CARGO_RECURSOS -1004

#define CN 0 // Negro
#define CA 1 // Amarillo
#define CB 2 // Blanco
#define CV 3 // Verde
#define CR 4 // Rojo
#define CX 5 // Azul
#define CY 6 // Rosa
#define CNH 7 //naranja hud
#define VERDE_ESFERAS    8
#define NARANJA_GOKU     9
#define CIAN_RADAR      10
#define ROJO_ALERTA     11
#define CASILLA_OCULTA  12
#define CASILLA_REVELADA 13



//estructura juego normal
typedef struct {
    bool esMina;
    bool revelada;
    bool marcada;
    int minasVecinas;
    int esfera;
    int esferaAlPerder;
} Casilla;

//historial
typedef struct {
    Casilla** tablero;
    int minasMarcadas; //cuantas minas estaban marcadas
    int dimension;
} FotoTablero;

//historial
typedef struct {
    FotoTablero* lista;
    int cantidad;
    int posActual;
} HistorialFotosTablero;

//estructura juego normal
typedef struct {
    int dimension;
    int dimensionInicial;
    int totalMinas;
    int totalMinasInicial;
    int minasMarcadas;
    int minaExplotadaFila;
    int minaExplotadaCol;
    bool finalizado;
    bool minasColocadas;
    Uint32 tiempoInicio;
    Uint32 tiempoFin;
    Casilla** tablero;
    int tamCasilla;
    int tamPixel;
    bool cheatActivo;
    int cheatUsosRestantes;
    Uint32 cheatTiempoInicio;
    HistorialFotosTablero historial;
} Juego;


//Un enum (enumeración) es un tipo de dato que permite definir un conjunto de valores constantes con nombre
//El typedef permite crear un alias para ese tipo facilitando el uso.
//opcion = OPCION_PRINCIPAL_JUGAR;

typedef enum {
    OPCION_PRINCIPAL_JUGAR,
    OPCION_PRINCIPAL_ESTADISTICAS,
    OPCION_PRINCIPAL_SALIR
} opcionesMenuPrincipal;

typedef enum {
    OPCION_PARTIDA_NUEVA,
    OPCION_PARTIDA_CARGAR,
    OPCION_PARTIDA_MENU_PRINCIPAL
} opcionesMenuTipoPartida;

typedef enum {
    DIFICULTAD_FACIL,
    DIFICULTAD_MEDIO,
    DIFICULTAD_DIFICIL,
    DIFICULTAD_SSJ,
    DIFICULTAD_CUSTOM,
    DIFICULTAD_VOLVER
} opcionesMenuDificultad;

typedef enum {
    OPCION_NICKNAME_CONFIRMAR,
    OPCION_NICKNAME_VOLVER
} opcionesMenuNickname;

void chequearError(int error, int errorEsperado);


#define FRAMES_MINA_EXPLOTADA 2
#define FRAMES_NUMEROS 2
#define TOTAL_NUMEROS 8
#define FRAMES_ESFERAS 2
#define TOTAL_ESFERAS 7
#define LOGO_FRAMES 2

#define CHEAT_FRAME
#define DURACION_CHEAT_MS 1500
#define CHEAT_MAX_FACIL 1
#define CHEAT_MAX_MEDIO 2
#define CHEAT_MAX_DIFICIL 3
#define CHEAT_MAX_SUPER 4
#define MAX_CLICKS_CHEAT 7
#define FRAMES_RASTREADOR 2

#define MAX_NOMBRE 5
#define MAX_DIFICULTAD 16
#define MAX_ESTADISTICAS 3
#define MAX_ESTAD_TOTALES 100


#define MAX_ENTRADAS 3


#endif // DEFINES_H_INCLUDED
