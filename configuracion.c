#include "configuracion.h"

bool guardarConfiguracion(const ConfiguracionJuego* config) {
    FILE* archivo = fopen("buscaminas.conf", "w");
    if (!archivo) return false;

    fprintf(archivo, "dimensiones=%d\n", config->dimension);
    if (config->esPorcentaje) {
        fprintf(archivo, "minas=%d%%\n", config->valorMinas);
    } else {
        fprintf(archivo, "minas=%d\n", config->valorMinas);
    }

    fclose(archivo);
    return true;
}

bool cargarConfiguracion(ConfiguracionJuego* config) {
    FILE* archivo = fopen("buscaminas.conf", "r");
    if (!archivo) return false;

    config->dimension = 10; // defaults
    config->esPorcentaje = false;
    config->valorMinas = 10;

    char linea[100];
    while (fgets(linea, sizeof(linea), archivo)) {
        char clave[50], valor[50];
        if (sscanf(linea, "%[^=]=%s", clave, valor) == 2) {
            if (strcmp(clave, "dimensiones") == 0) {
                int dim = atoi(valor);
                if (dim >= 8 && dim <= 32) config->dimension = dim;
            } else if (strcmp(clave, "minas") == 0) {
                if (strchr(valor, '%')) {
                    int porc = atoi(valor);
                    if (porc >= 1 && porc <= 100) {
                        config->esPorcentaje = true;
                        config->valorMinas = porc;
                    }
                } else {
                    int minas = atoi(valor);
                    if (minas > 0) {
                        config->esPorcentaje = false;
                        config->valorMinas = minas;
                    }
                }
            }
        }
    }

    fclose(archivo);
    return true;
}
