#include "Window.h"

void dropdown_init(DropDown *dd, RenderWindow *rw, TTF_Font *font, Vector2i pos, Vector2i size) {
    // don't ptr_assert dd, this will be stack allocated and passed as reference
    dd->pos = pos;
    dd->size = size;
    
    dd->textures = malloc((sizeof *dd->textures) * 2); // give space for two textures
    ptr_assert(dd->textures, "init_dropdown(): error allocating memory for dropdown textures");

    dd->textures_len = 0;
    dd->textures_cap = 2;
}

void dropdown_add_text(DropDown *dd, RenderWindow *rw, TTF_Font *font, const char *text) {
    if (dd->textures_cap == dd->textures_len) {
        dd->textures = realloc(dd->textures, (sizeof *dd->textures) * dd->textures_cap * 2);
        dd->textures_cap *= 2;
    }

    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, text, SDL_COLOR_BLACK);
    ptr_assert(text_surface, "dropdown_add_text(): error creating text surface: %s\n", SDL_GetError());

    SDL_Surface *main_surface = SDL_CreateRGBSurface(0,
        text_surface->w+15, text_surface->h + 15,
        32, 0, 0, 0, 255
    );
    Uint32 color = SDL_MapRGBA(main_surface->format, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_FillRect(main_surface, NULL, color);

    SDL_Rect dst = (SDL_Rect){.x = dd->pos.x, .y = dd->pos.y, .w = dd->size.x, .h = dd->size.y};

    sdl_assert(SDL_BlitSurface(text_surface, NULL, main_surface, &dst),
    "dropdown_add_text(): error blitting text surface onto main surface: %s\n", SDL_GetError());

    SDL_FreeSurface(text_surface);

    SDL_Texture *tex = SDL_CreateTextureFromSurface(rw->renderer, main_surface);
    ptr_assert(tex, "dropdown_add_text(): error creating texture from main surface: %s\n", SDL_GetError());

    dd->textures[dd->textures_len++] = tex;
}

void dropdown_close(DropDown *dd) {
    ptr_assert(dd->textures, "close_dropdown(): dd->textures is NULL\n");

    for (int i=0; i<dd->textures_len; i++) {
        SDL_DestroyTexture(dd->textures[i]);
    }
    free(dd->textures);
}