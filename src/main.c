#include "utils.h"
#include "Window.h"
#include "Solve.h"

#define WIN_HEIGHT 600
#define WIN_WIDTH 800



int main(int argc, char *argv[]) {
    srand((unsigned int)time(0));
    
    RenderWindow *rw = rw_init("test", SDL_INIT_EVERYTHING, IMG_INIT_PNG, SDL_WINDOW_RESIZABLE, SDL_RENDERER_PRESENTVSYNC);
    Font_Info *font_info = ttf_load_fonts("res/Consolas.ttf");
    Scroller *scroller = scroller_init(rw, font_info->fonts[SCROLLER_FONT]);

    init_sql_db("res/sql/solves.db");

    SDL_DisplayMode mode;
    sdl_assert(SDL_GetDisplayMode(0, 0, &mode), "main(): could not get display mode\n");
    int refresh_rate = mode.refresh_rate;

    // some defaults:
    char *scramble = generate_scramble();
    SDL_Texture *scramble_texture = ttf_render_scramble(rw, font_info->fonts[SCRAMBLE_FONT], scramble, SDL_COLOR_BLACK);
    SDL_Texture *inspection_text_texture = ttf_render_text(rw, font_info->fonts[INSPECT_FONT], "Inspecting...", SDL_COLOR_WHITE);
    SDL_Texture *time_texture;

    bool solving = false;
    bool inspecting = false;
    bool scrambling = true;
    bool solve_ready = false;

    float current_time = 0.0f;
    float previous_time = 0.0f;

    char *best_time = get_best_time();
    SDL_Texture *best_time_texture = ttf_render_text(rw, font_info->fonts[OTHER_FONT], best_time, SDL_COLOR_BLACK);
    
    char *avg5 = get_avg5();
    SDL_Texture *avg5_texture = ttf_render_text(rw, font_info->fonts[OTHER_FONT], avg5, SDL_COLOR_BLACK);

    DropDown dd;
    dropdown_init(&dd, rw, font_info->fonts[OTHER_FONT], (Vector2i){.x=100, .y=100}, (Vector2i){.x=50, .y=50});
    dropdown_add_text(&dd, rw, font_info->fonts[OTHER_FONT], "test");

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE: {
                        if (solving) {
                            // if we are solving, we want to press space down to stop the timer
                            // we then want to save the time,
                            // free previous textures for the scramble, 
                            // render new scramble
                            solve_save(scroller->current_type, current_time, scramble);
                            previous_time = current_time;

                            SDL_DestroyTexture(scramble_texture);
                            free(scramble);
                            scramble = generate_scramble();
                            scramble_texture = ttf_render_scramble(rw, font_info->fonts[SCRAMBLE_FONT], scramble, SDL_COLOR_BLACK);
                            
                            SDL_DestroyTexture(best_time_texture);
                            free(best_time);
                            best_time = get_best_time();
                            best_time_texture = ttf_render_text(rw, font_info->fonts[OTHER_FONT], best_time, SDL_COLOR_BLACK);

                            SDL_DestroyTexture(avg5_texture);
                            free(avg5);
                            avg5 = get_avg5();
                            avg5_texture = ttf_render_text(rw, font_info->fonts[OTHER_FONT], avg5, SDL_COLOR_BLACK);

                            solving = false;
                            scrambling = true;
                        } else if (scrambling) {
                            // if we press space when we are scrambling,
                            // we enter inspection mode
                            inspecting = true;
                            scrambling = false;
                        }
                    } break;
                    case SDLK_f: {
                        free(scramble);
                        scramble = generate_scramble();
                        scramble_texture = ttf_render_scramble(rw, font_info->fonts[SCRAMBLE_FONT], scramble, SDL_COLOR_BLACK);
                    } break;
                    case SDLK_ESCAPE: {
                        running = false;
                        break;
                    }
                }
            } else if (event.type == SDL_KEYUP) {
                if (inspecting && !solve_ready) {
                    // we press the space bar to enter inspect mode,
                    // but we dont want the following keyup to trigger the solve
                    solve_ready = true;
                } else if (inspecting && solve_ready) {
                    // we only want to worry about keyup's when we are inspecting
                    // because that will be the cue to start solving 
                    inspecting = false;
                    solving = true;
                    solve_ready = false;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                scroller_check_click(event.button.x, event.button.y, scroller);
            }
        }
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        bool hovering = scroller_check_hover(mouse_x, mouse_y, scroller);

        rw_render_dropdown(rw, &dd);
        
        if (scrambling) { //scrambling
            current_time = 0;

            rw_set_bg_color(rw, SDL_COLOR_RED);
            rw_clear(rw);

            time_texture = ttf_render_time(rw, font_info->fonts[TIME_FONT], previous_time, SDL_COLOR_BLACK);

            rw_render_scroller(rw, scroller, hovering);
            rw_render_time(rw, time_texture);
            rw_render_scramble(rw, scramble_texture);
            rw_render_text(rw, best_time_texture, WIN_WIDTH/2, (7*WIN_HEIGHT)/8 + 15);
            rw_render_text(rw, avg5_texture, WIN_WIDTH/2, (7*WIN_HEIGHT)/8 + 30);
        } else if (inspecting) {
            rw_set_bg_color(rw, SDL_COLOR_BLUE);
            rw_clear(rw);

            rw_render_text(rw, inspection_text_texture, WIN_WIDTH/2, WIN_HEIGHT/2);
        } else if (solving) {
            rw_set_bg_color(rw, SDL_COLOR_GREEN);
            rw_clear(rw);

            time_texture = ttf_render_time(rw, font_info->fonts[TIME_FONT], current_time, SDL_COLOR_BLACK);
            current_time += 1.0f/((float)refresh_rate);

            rw_render_time(rw, time_texture);
        }

        rw_present(rw);

        SDL_DestroyTexture(time_texture);
    }

    if (scramble_texture != NULL) SDL_DestroyTexture(scramble_texture);
    if (time_texture != NULL) SDL_DestroyTexture(time_texture);
    if (avg5_texture != NULL) SDL_DestroyTexture(avg5_texture);

    if (scramble != NULL) free(scramble);
    if (avg5 != NULL) free(avg5);

    dropdown_close(&dd);
    ttf_free_fonts(font_info);
    scroller_free(scroller);
    close_sql_db();
    rw_cleanup(rw);
    SDL_Quit();

    return 0;
}