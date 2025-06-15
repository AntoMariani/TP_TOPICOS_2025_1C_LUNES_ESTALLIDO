#include "defines.h"

void chequearError(int error, int errorEsperado)
{
    if(error == errorEsperado)
    {
        exit(error);
    }
}
