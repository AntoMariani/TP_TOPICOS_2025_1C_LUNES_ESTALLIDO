#ifndef CONFIGURACION_H_INCLUDED
#define CONFIGURACION_H_INCLUDED

#include <stdbool.h>
#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define esNumero(n) ('0' <= (n) && '9' >= (n))
bool guardarConfiguracion(const ConfiguracionJuego* config);
bool cargarConfiguracion(ConfiguracionJuego* config);

#endif // CONFIGURACION_H_INCLUDED
