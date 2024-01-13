#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "sqlite3.h"


#define ptr_assert(_ptr, ...) if(!(_ptr)) {fprintf(stderr, __VA_ARGS__); exit(1);} // 0  (NULL) is bad
#define sdl_assert(_ptr, ...) if((_ptr)) {fprintf(stderr, __VA_ARGS__); exit(1);} // 0 is good


extern char *time_float_to_str(float time); //  to MM:SS:mSmSmS format

#endif // UTILS_H