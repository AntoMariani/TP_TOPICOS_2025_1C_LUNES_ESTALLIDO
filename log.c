#include "log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

char nombreLog[128] = "";

int iniciarLog() {
    time_t ahora = time(NULL); 

    strftime(nombreLog, sizeof(nombreLog), "log/jugada_%Y-%m-%d_%H-%M-%S.log", localtime(&ahora));

    FILE* archivo = fopen(nombreLog, "w");

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

int registrarEvento(const char* tipo, int fila, int col, int minasVecinas) {
    if (nombreLog[0] == '\0') {
        int estado = iniciarLog();
        if (estado != TODO_OK)
            return estado;
    }

    FILE* archivo = fopen(nombreLog, "a"); 

    if (!archivo)
    return ERROR_LOG_ESCRITURA;

    char fechaHora[64];
    time_t ahora = time(NULL);
    strftime(fechaHora, sizeof(fechaHora), "%Y-%m-%d %H:%M:%S", localtime(&ahora));
    fprintf(archivo, "[%s] ", fechaHora); 
    printf("[%s] Click en casilla fila=%d, columna=%d, minasVecinas=%d\n", fechaHora, fila, col,minasVecinas);

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
