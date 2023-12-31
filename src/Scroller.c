#include "Window.h"

const char *Cube_Types_str[NUM_CUBE_TYPES] = {"3x3x3", "4x4x4", "5x5x5", "6x6x6", "7x7x7"};


Scroller *scroller_init(RenderWindow *rw, TTF_Font *font) {
    ptr_assert(rw, "scroller_init(): rw is null\n");
    ptr_assert(font, "scroller_init(): font is null\n");

    Scroller *scroller = NULL;
    scroller = malloc(sizeof *scroller);
    ptr_assert(scroller, "scroller_init(): error allocating memory for scroller\n");

    for (int i=0; i<NUM_SCROLLER_TEXTURES; i++) {

        SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, Cube_Types_str[i], SCROLLER_TEXT_COLOR);
        ptr_assert(text_surface, "scroller_init(): error creating text surface: %s\n", SDL_GetError());

        SDL_Surface *main_surface = SDL_CreateRGBSurface(0, 
            text_surface->w + 2*SCROLLER_HORI_PADDING,
            text_surface->h + 2*SCROLLER_VERT_PADDING,
            32, 0, 0, 0, 255
        );
        Uint32 color = SDL_MapRGBA(main_surface->format, 240, 240, 240, SDL_ALPHA_OPAQUE); // gray(ish)
        SDL_FillRect(main_surface, NULL, color);

        SDL_Rect dst = (SDL_Rect){.x = SCROLLER_HORI_PADDING, .y = SCROLLER_VERT_PADDING, .w = text_surface->w, .h = text_surface->h};

        sdl_assert(SDL_BlitSurface(text_surface, NULL, main_surface, &dst),
        "scroller_generate_texture(): error blitting surface: %s\n", SDL_GetError());

        SDL_FreeSurface(text_surface);

        SDL_Texture *scroller_texture = SDL_CreateTextureFromSurface(rw->renderer, main_surface);
        ptr_assert(scroller_texture, "scroller_generate_texture(): error creating scroller texture from blitted surface: %s\n", SDL_GetError());

        dst.x = SCROLLER_POS_X - main_surface->w / 2;
        dst.y = SCROLLER_POS_Y - main_surface->h / 2;
        dst.w = main_surface->w;
        dst.h = main_surface->h;
        scroller->dst_rects[i] = dst;
        scroller->textures[i] = scroller_texture;

    }
    scroller->current_type = THREE_BY_THREE;


    return scroller;
}

void scroller_check_click(int mouse_x, int mouse_y, Scroller *scroller) {
    ptr_assert(scroller, "scroller_check_click(): scroller is null\n");

    SDL_Rect scroller_rect = scroller->dst_rects[scroller->current_type];
    if (mouse_x > scroller_rect.x && mouse_x < (scroller_rect.x + scroller_rect.w) &&
        mouse_y > scroller_rect.y && mouse_y < (scroller_rect.y + scroller_rect.h)) {
        
        __scroller_cycle_type(scroller);
    }
}

bool scroller_check_hover(int mouse_x, int mouse_y, Scroller *scroller) {
    SDL_Rect scroller_rect = scroller->dst_rects[scroller->current_type];

    if (mouse_x > scroller_rect.x && mouse_x < (scroller_rect.x + scroller_rect.w) &&
        mouse_y > scroller_rect.y && mouse_y < (scroller_rect.y + scroller_rect.h)) {
        // collision
        return true;
    }
    return false;
}

void scroller_free(Scroller *scroller) {
    ptr_assert(scroller, "scroller_free(): scroller is null\n");
    ptr_assert(scroller->textures, "scroller_free(): scroller textures are null\n");

    for (int i=0; i<NUM_SCROLLER_TEXTURES; i++) {
        SDL_DestroyTexture(scroller->textures[i]);
    }
    free(scroller);
}