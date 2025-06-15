
# TP Tópicos 2025 — Buscaminas Dragon Ball

Trabajo Práctico de Tópicos de Programación — 1er Cuatrimestre 2025 — Lunes Estallido

---
## Alumnos

| Apellido | DNI | Entrega |
| -------- | --- | ------- |
| Mariani, Antonella Lucia  | 41585421 | Sí |
| Nieva, Alejo Nicolas   | 46556068 | Sí |
| Savino, Gabriel Dario   | 39067659 | No |

---

## 🚀 ¿Qué hace el proyecto?

- Decidimos armar el buscaminas temático de Dragon Ball "BuscaEsferas"
- Todos los gráficos son acordes a la temática.
- 5 niveles de dificultad (incluye modo Custom que lee del buscaminas.conf).
> 📝 **NOTA**: Modo CUSTOM lee solo de buscaminas.conf, el resto de niveles están precargados.
- Ingreso de nickname para el guardado de estadísticas de partidas ganadas por dificultad.
- Registro completo de logs de partidas (`/log`).
> ⚠️ **IMPORTANTE**: El directorio `log/` debe existir al nivel raíz del proyecto.
- Cheat limitado para descubrir minas.
> 👀 **TRUCO**: Fijate que pasa si apretas la esfera mientras jugás.
- Logs de cada evento del juego (inicio, clicks, fin de partida).

---

## 📂 Estructura de carpetas

```
TP_TOPICOS_2025_1C_LUNES_ESTALLIDO/
│
├── img/
│   └── [imagenes y animaciones].png
├── log/
│   └── [logs de partida]
├── lib/
│   └── SDL2_Image/lib/
│               └──... 
│   └── SDL2_ttf/lib/
│               └──...
│   └── SDL2.dll
│   └── SDL2_image.dll
│   └── SDL2_ttf.dll
├── defines.c
├── defines.h
├── dibujos.c
├── dibujos.h
├── escalado.c
├── escalado.h
├── estadisticas.c
├── estadisticas.h
├── interfaz.c
├── interfaz.h
├── juego.c
├── juego.h
├── log.c
├── log.h
├── main.c
├── SDL_image.h
├── SDL_ttf.h
└── README.md
```

> ⚠️ **IMPORTANTE**: El directorio `log/` debe existir al nivel raíz del proyecto.

---

## ⚙️ Compilación

### 1️⃣ Clonar el proyecto

```bash
git clone https://github.com/AntoMariani/TP_TOPICOS_2025_1C_LUNES_ESTALLIDO.git
```

### 2️⃣ Generar un nuevo proyecto en Code::Blocks con los .h y .c correspondientes al proyecto y configurar:

#### Project → Build Options → Linker Settings → Link libraries:

- SDL2
- SDL2_image
- SDL2_ttf

#### Other linker options:

```
-lSDL2_image
-lSDL2
-lmingw32
-lSDL2main
-lSDL2_ttf
```

#### Search Directories → Compiler:

Agregar:

- SDL2.h
- SDL_image.h
- SDL_ttf.h

Normalmente:

```
SDL2/include
SDL2_image/include
SDL2_ttf/include
```

#### Search Directories → Linker:

```
{PATH}\TP_TOPICOS_2025_1C_LUNES_ESTALLIDO\lib\SDL2_Image\lib
{PATH}\TP_TOPICOS_2025_1C_LUNES_ESTALLIDO\lib\SDL2_ttf\lib
```

> `{PATH}` es la ruta donde clonaste el repo.

---

## 📦 Librerias

Cuando se genere el ejecutable (`.exe`) es importante copiar dentro de la misma carpeta donde esté el ejecutable las siguientes librerías que se encuentran dentro del directorio `lib/`:

```
/lib/SDL2.dll
/lib/SDL2_image.dll
/lib/SDL2_ttf.dll
```

> 👀 **IMPORTANTE:** En este README sumamos el SDL pero asumimos que ya lo tenés descargado.

---

## ⚠️ Si falla por problemas de librerías SDL:

Se debe descargar manualmente:

- **SDL_image 2.6.3:**  
  https://github.com/libsdl-org/SDL_image/releases/tag/release-2.6.3

- **SDL_ttf 2.22.0:**  
  https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0

  (descargar el `SDL2_ttf-devel-2.22.0-mingw.zip`)

Luego configurar:

#### Search Directories → Compiler:

```
SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2
SDL2_image-2.6.3\x86_64-w64-mingw32\include\SDL2
SDL2_ttf-2.22.0\x86_64-w64-mingw32\include\SDL2
```

#### Search Directories → Linker:

```
SDL2-2.0.22\x86_64-w64-mingw32\lib
SDL2_image-2.6.3\x86_64-w64-mingw32\lib
SDL2_ttf-2.22.0\x86_64-w64-mingw32\lib
```

---

## 🔮 TO - DO / Features a futuro

- Como primer acto de rebelión queremos cambiar esos números feos que pusimos.
- Agregar animaciones al perder y al ganar.
- Implementar carga y guardado de partidas.
> 📝 **NOTA**: Hoy en día el botón quedó generado a modo de placeholder.
- Agregar sonidos.
- Agregar menú al final de la partida con las opciones "MENÚ PRINCIPAL", "SALIR".
- Mejorar el menú de estadísticas (nicknames de solo 4 letras, incorporar cantidad de clicks).
- Validación de returns (manejo de errores más sólido).
- Refinar modularización de ciclos de juego.

---

## 📝 Sobre los comentarios

Nos contó un gokucito que a usted profe no le agradan los comentarios por lo que los eliminamos, nosotros somos los rey-comentario así que si llega a precisar una versión comentada del código nos comenta.

---

## 📝 Sobre el versionado

Lo más probable es que realicemos modificaciones posteriores a la entrega. El commit correspondiente a la misma es el mismo sobre el cuál estará generado este mismo README.md

---
