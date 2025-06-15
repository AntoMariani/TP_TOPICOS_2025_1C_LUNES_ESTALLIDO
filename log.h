#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include "defines.h"

int iniciarLog();
int registrarEvento(const char* tipo, int fila, int col, int minasVecinas);

#endif // LOG_H_INCLUDED
