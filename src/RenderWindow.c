#include "Window.h"

RenderWindow *rw_init(
    const char  *window_name,
    Uint32      SDL_flags,
    int         IMG_flags,
    Uint32      window_flags,
    Uint32      renderer_flags
) {

    RenderWindow *rw = NULL;
    rw = malloc(sizeof *rw);
    ptr_assert(rw, "rw_init(): error allocating memory for render window\n");

    if (SDL_Init(SDL_flags) < 0) {
        fprintf(stderr, "SDL_Init failed: %s", SDL_GetError());
        exit(1);
    }
    if (!IMG_Init(IMG_flags)) {
        fprintf(stderr, "IMG_Init failed: %s", SDL_GetError());
        exit(1);
    }
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init failed: %s", SDL_GetError());
        exit(1);
    }

    rw->window = SDL_CreateWindow(window_name, WIN_POS_X, WIN_POS_Y, WIN_WIDTH, WIN_HEIGHT, window_flags);
    rw->renderer = SDL_CreateRenderer(rw->window, -1, renderer_flags);
    rw->bg_color = WINDOW_BG_COLOR_DEFAULT;

    return rw;
}

void rw_cleanup(RenderWindow *rw) {
    ptr_assert(rw, "rw_cleanup(): rw is null\n");

    SDL_DestroyWindow(rw->window);
    SDL_DestroyRenderer(rw->renderer);
}


void rw_render_time(RenderWindow *rw, SDL_Texture *time_texture) {
    ptr_assert(rw, "rw_render_time(): rw is null\n");
    ptr_assert(time_texture, "rw_render_time(): time_texture is null\n");

    int w, h;
    SDL_QueryTexture(time_texture, NULL, NULL, &w, &h);

    SDL_Rect src;
    src.x = 0; src.y = 0;
    src.w = w; src.h = h;

    SDL_Rect dst;
    dst.x = (WIN_WIDTH / 2) - (w / 2);
    dst.y = (WIN_HEIGHT / 2) - (h / 2);
    dst.w = w; 
    dst.h = h;

    SDL_RenderCopy(rw->renderer, time_texture, &src, &dst);
}


void rw_render_scramble(RenderWindow *rw, SDL_Texture *scramble_texture) {
    ptr_assert(rw, "rw_render_scramble(): rw is null\n");
    ptr_assert(scramble_texture, "rw_render_scramble(): scramble_texture is null\n");

    int w, h;
    SDL_QueryTexture(scramble_texture, NULL, NULL, &w, &h);

    SDL_Rect dst;
    dst.x = (WIN_WIDTH / 2) - (w/2);
    dst.y = (WIN_HEIGHT / 4) - (h / 2);
    dst.w = w;
    dst.h = h;

    SDL_RenderCopy(rw->renderer, scramble_texture, NULL, &dst);
}



void rw_render_inspection_text(RenderWindow *rw, SDL_Texture *inspection_text_texture) {
    ptr_assert(rw, "rw_render_inspection_text(): rw is null\n");
    ptr_assert(inspection_text_texture, "rw_render_inspection_text(): inspection_text_texture is null\n");


    int w, h;
    SDL_QueryTexture(inspection_text_texture, NULL, NULL, &w, &h);
    
    SDL_Rect dst;
    dst.x = (WIN_WIDTH / 2) - (w / 2);
    dst.y = (WIN_HEIGHT / 2) - (h / 2);
    dst.w = w;
    dst.h = h;

    SDL_RenderCopy(rw->renderer, inspection_text_texture, NULL, &dst);
}



void rw_render_scroller(RenderWindow *rw, Scroller *scroller, bool hovering) {
    ptr_assert(rw, "rw_render_scroller(): rw is null\n");
    ptr_assert(scroller, "rw_render_scroller(): scroller is null\n");

    SDL_Rect dst = scroller->dst_rects[scroller->current_type];

    if (hovering) {
        // we make the thing a bit bigger, then we need to recenter it about the point 

        dst.w *= 1.2;
        dst.h *= 1.2;
        dst.x -= (dst.w) * (0.2f / 2.0f);
        dst.y -= (dst.h) * (0.2f / 2.0f);
    }

    sdl_assert(SDL_RenderCopy(rw->renderer, scroller->textures[scroller->current_type], NULL, &dst),
    "rw_render_scroller(): error copying scroller texture to renderer: %s\n", SDL_GetError());
}