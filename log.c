#include "log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

//var global para guardar el nombre del archivo de log
char nombreLog[128] = "";

//inicializa basado en fecha y hora
int iniciarLog() {
    time_t ahora = time(NULL); //obtiene tiempo actual

    //genera nombre log/jugada_AAAA-MM-DD_HH-MM-SS.log
    strftime(nombreLog, sizeof(nombreLog), "log/jugada_%Y-%m-%d_%H-%M-%S.log", localtime(&ahora));

    FILE* archivo = fopen(nombreLog, "w"); //escritura

    if (!archivo) {
        printf("No se pudo crear el archivo de log.\n");
        printf("Se tiene que crear el directorio /log en el mismo lugar que el ejecutable\n");
        return ERROR_LOG_ESCRITURA;
    }

    if (archivo) {
        puts("Juego inicializado correctamente.\n");
        fclose(archivo);
    }
    return TODO_OK;
}

// Registra un evento en el archivo de log
int registrarEvento(const char* tipo, int fila, int col, int minasVecinas) {
    //si el nombre del log esta vacio, inicializa el log
    if (nombreLog[0] == '\0') {
        int estado = iniciarLog();
        if (estado != TODO_OK)
            return estado;
    }

    FILE* archivo = fopen(nombreLog, "a");  //modo append

    if (!archivo)
    return ERROR_LOG_ESCRITURA;

    //obtiene fecha y hora actual para registro
    char fechaHora[64];
    time_t ahora = time(NULL);
    strftime(fechaHora, sizeof(fechaHora), "%Y-%m-%d %H:%M:%S", localtime(&ahora));
    fprintf(archivo, "[%s] ", fechaHora);

    //muestra en consola
    printf("[%s] Click en casilla fila=%d, columna=%d, minasVecinas=%d\n", fechaHora, fila, col,minasVecinas);

    //escribe segun evento
    if (strcmp(tipo, "INICIO") == 0) {
        fprintf(archivo, "Juego iniciado.\n");
    } else if (strcmp(tipo, "FIN") == 0) {
        fprintf(archivo, "Juego finalizado.\n");
    } else if (strcmp(tipo, "CLICK_IZQUIERDO") == 0) {
        fprintf(archivo, "Clic izquierdo en fila=%d, columna=%d, minasVecinas=%d\n", fila, col, minasVecinas);
    } else if (strcmp(tipo, "CLICK_DERECHO") == 0) {
        fprintf(archivo, "Clic derecho en fila=%d, columna=%d\n", fila, col);
    }

    fclose(archivo);
    return TODO_OK;
}
