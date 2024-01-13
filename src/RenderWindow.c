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



void rw_render_text(RenderWindow *rw, SDL_Texture *text_texture, int x, int y) {
    ptr_assert(rw, "rw_render_text(): rw is null\n");
    ptr_assert(text_texture, "rw_render_text(): text_texture is null\n");

    int w, h;
    SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
    x -= w/2;
    y -= h/2;
    
    SDL_Rect dst = (SDL_Rect){.x = x, .y = y, .w = w, .h = h};

    SDL_RenderCopy(rw->renderer, text_texture, NULL, &dst);
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









/* void init_main_state(MainState *m,
                    char * (*scramble_generate_func)(void),
                    char * (*get_best_time_func)(void),
                    void (*solve_save)(uint8_t, float, char *),
                    RenderWindow *rw,
                    Font_Info *font_info
) {
    // no ptr_assert, this is the address of stack_allocated memory

    m->get_best_time = get_best_time_func;
    m->generate_scramble = scramble_generate_func;
    m->solve_save = solve_save;

    m->solving = false;
    m->inspecting = false;
    m->scrambling = true;
    m->solve_ready = false;

    m->running = true;

    m->current_time = 0.0f;
    m->previous_time = 0.0f;

    char *scramble = (* m->generate_scramble)();
    m->textures[SCRAMBLE_TEXTURE] = ttf_render_scramble(rw, font_info->fonts[SCRAMBLE_FONT], scramble, SDL_COLOR_BLACK);
    m->scramble = scramble;

    char *best_time = (* m->get_best_time)();
    m->textures[BEST_TIME_TEXTURE] = ttf_render_text(rw, font_info->fonts[OTHER_FONT], best_time, SDL_COLOR_BLACK);
    m->best_time = best_time;

    m->textures[INSPECTION_TEXTURE] = ttf_render_text(rw, font_info->fonts[INSPECT_FONT], "Inspecting...", SDL_COLOR_WHITE);

}

bool handle_input(MainState *m, RenderWindow *rw, Font_Info *font_info, Scroller *scroller) {
    while(SDL_PollEvent(&m->event)) {
        if (m->event.type == SDL_QUIT) {
            m->running = false;
            return false;
        } else if (m->event.type == SDL_KEYDOWN) {
            switch (m->event.key.keysym.sym) {
                case SDLK_SPACE: {
                    if (m->solving) {
                        (* m->solve_save)(scroller->current_type, m->current_time, m->scramble);
                        m->previous_time = m->current_time;

                        SDL_DestroyTexture(m->textures[SCRAMBLE_TEXTURE]);
                        free(m->scramble);
                        m->scramble = (* m->generate_scramble)();
                        m->textures[SCRAMBLE_TEXTURE] = ttf_render_scramble(rw, font_info->fonts[SCRAMBLE_FONT], m->scramble, SDL_COLOR_BLACK);

                        SDL_DestroyTexture(m->textures[BEST_TIME_TEXTURE]);
                        free(m->best_time);
                        m->best_time = (* m->get_best_time());
                        m->textures[BEST_TIME_TEXTURE] = ttf_render_text(rw, font_info->fonts[OTHER_FONT], m->best_time, SDL_COLOR_BLACK);

                        m->solving = false;
                        m->scrambling = true;
                    } else if (m->scrambling) {
                        m->inspecting = true;
                        m->scrambling = false;
                    }
                } break;
                case SDLK_f: {
                    free(m->scramble);
                    m->scramble = (* m->generate_scramble)();
                    m->textures[SCRAMBLE_TEXTURE] = ttf_render_text(rw, font_info->fonts[SCRAMBLE_FONT], m->scramble, SDL_COLOR_BLACK);
                } break;
                case SDLK_ESCAPE: {
                    m->running = false;
                    return false;
                }
            }
        } else if (m->event.type == SDL_KEYUP) {
            if (m->inspecting && !m->solve_ready) {
                m->solve_ready = true;
            } else if (m->inspecting && m->solve_ready) {
                m->inspecting = false;
                m->solving = true;
                m->solve_ready = false;
            }
        }
    }
    return true;
}

void destroy_main_state(MainState *m) {
    for (int i=0; i<NUM_STATE_TEXTURES; i++) {
        if (m->textures[i] != NULL) SDL_DestroyTexture(m->textures[i]);
    }
    if (m->best_time != NULL) free(m->best_time);
    if (m->scramble != NULL) free(m->scramble);
}


 */