#include "Window.h"


static void __ttf_font_hints(TTF_Font *font) {
    TTF_SetFontStyle(font, FONT_STYLE);
    TTF_SetFontOutline(font, FONT_OUTLINE); // 0 is default
    TTF_SetFontKerning(font, FONT_KERNING); // 1 says yes kerning, apparently it makes rendering nicer or something
    TTF_SetFontHinting(font, FONT_HINTING);
}

Font_Info *ttf_load_fonts(const char *filepath) {
    Font_Info *font_info = NULL;
    font_info = malloc(sizeof *font_info);
    ptr_assert(font_info, "ttf_load_font(): error allocating memory for font_info\n");

    for (int i=0; i<FONT_COUNT; i++) {
        font_info->fonts[i] = TTF_OpenFont(filepath, Font_Sizes[i]);
        ptr_assert(font_info->fonts[i], "ttf_load_font(): error loading font\n");
        __ttf_font_hints(font_info->fonts[i]);
    }
    
    return font_info;
}


SDL_Texture *ttf_render_time(RenderWindow* rw, Font_Info *font_info, float time, SDL_Color color) {
    int mins = time / 60; 
    int secs = floorf(time); // should give what's before decimal (6.789 -> 6)
    int ms = floorf(1000.0f * (time - secs)); // should give everything past the decimal (6.789 -> .789) 789 / 100 7
    //printf("%d\n", ms);

    char time_str[10];
    sprintf(time_str, "%d%d:%d%d.%d%d%d", mins/10, mins % 10, secs/10, secs % 10, ms / 100, (ms - (ms/100)*100) / 10, ms % 10);


    SDL_Surface *time_surface = TTF_RenderUTF8_Solid(font_info->fonts[TIME_FONT], time_str, color);
    ptr_assert(time_surface, SDL_GetError());

    SDL_Texture *time_texture = SDL_CreateTextureFromSurface(rw->renderer, time_surface);
    ptr_assert(time_surface, SDL_GetError());

    SDL_FreeSurface(time_surface);
    return time_texture;
}


SDL_Texture *ttf_render_scramble(RenderWindow* rw, Font_Info *font_info, char *scramble, SDL_Color color) {
    ptr_assert(rw, "ttf_render_scramble(): rw is null\n");
    ptr_assert(font_info, "ttf_render_scramble(): font_info is null\n");

    printf("Original scramble: %s\n", scramble);

    size_t newline_pos = strcspn(scramble, "\n");
    char scramble1[128] = ""; // apparently initializing this all to a blank string fixes the weird bug
    char scramble2[128] = ""; // don't ask me!
    memcpy(scramble1, scramble, sizeof(char) * newline_pos);
    scramble1[strlen(scramble1)] = 0; // null terminator!
    memcpy(scramble2, scramble + newline_pos + 1, sizeof(char) * (strlen(scramble) - (newline_pos + 1)));
    scramble2[strlen(scramble2)] = 0; // null terminator!

    printf("Scramble1: %s\nScramble2: %s\n", scramble1, scramble2);

    SDL_Surface *scramble_surface1 = TTF_RenderUTF8_Blended(font_info->fonts[SCRAMBLE_FONT], scramble1, color);
    ptr_assert(scramble_surface1, SDL_GetError());
    SDL_Surface *scramble_surface2 = TTF_RenderUTF8_Blended(font_info->fonts[SCRAMBLE_FONT], scramble2, color);
    ptr_assert(scramble_surface2, SDL_GetError());

    SDL_Surface *main_surface = SDL_CreateRGBSurface(0,
        scramble_surface1->w, 
        2*scramble_surface1->h + 2*SCRAMBLE_FONT_PADDING,
        32,
        0, 0, 0, 255
    );
    Uint32 scramble_bg_color = SDL_MapRGBA(main_surface->format, 255, 0, 0, 255);
    SDL_FillRect(main_surface, NULL, scramble_bg_color);

    SDL_Rect dst1 = (SDL_Rect){
        .x = 0,
        .y = 0,
        .w = scramble_surface1->w,
        .h = scramble_surface1->h
    };
    SDL_Rect dst2 = (SDL_Rect){
        .x = (main_surface->w / 2) - (scramble_surface2->w / 2),
        .y = scramble_surface1->h + (SCRAMBLE_FONT_PADDING / 2),
        .w = scramble_surface2->w,
        .h = scramble_surface2->h
    };

    sdl_assert(SDL_BlitSurface(scramble_surface1, NULL, main_surface, &dst1),
    "ttf_render_scramble(): error blitting surfaces\n");
    sdl_assert(SDL_BlitSurface(scramble_surface2, NULL, main_surface, &dst2),
    "ttf_render_scramble(): error blitting surfaces\n");

    SDL_Texture *scramble_texture = SDL_CreateTextureFromSurface(rw->renderer, main_surface);

    return scramble_texture;
}


SDL_Texture *ttf_render_inspection_text(
    RenderWindow *rw,
    Font_Info *font_info,
    char *inspection_text,
    SDL_Color color
) {
    ptr_assert(rw, "ttf_render_inspection_text(): rw is null\n");
    ptr_assert(font_info, "ttf_render_inspection_text(): font_info is null\n");

    SDL_Surface *inspection_text_surface = TTF_RenderUTF8_Blended(font_info->fonts[INSPECT_FONT], inspection_text, color);
    ptr_assert(inspection_text_surface, SDL_GetError());

    SDL_Texture *inspection_text_texture = SDL_CreateTextureFromSurface(rw->renderer, inspection_text_surface);
    ptr_assert(inspection_text_texture, SDL_GetError());

    return inspection_text_texture;
}


void ttf_free_fonts(Font_Info *font_info) {
    ptr_assert(font_info, "ttf_free_font(): font_info is null\n");
    for (int i=0; i<FONT_COUNT; i++) {
        TTF_CloseFont(font_info->fonts[i]);
    }
}