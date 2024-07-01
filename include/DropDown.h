#ifndef BUTTON_H
#define BUTTON_H

#include "utils.h"

typedef struct {
    int width; // width of button
    int height; // height of button
    char *text; // button text
    SDL_Color color; // background color of button

    void (*on_hover)(Button *, void *); // defines hover behavior
    void (*on_click)(Button *, void *); // defines behavior of a click
} Button;


#endif // BUTTON_H