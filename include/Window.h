#ifndef WINDOW_H
#define WINDOW_H

#include "utils.h"



/********************************* 
*********  RENDERWINDOW  *********
**********************************/


#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define WIN_POS_X SDL_WINDOWPOS_UNDEFINED
#define WIN_POS_Y SDL_WINDOWPOS_UNDEFINED

#define SDL_COLOR_BLACK (SDL_Color){0, 0, 0, SDL_ALPHA_OPAQUE}
#define SDL_COLOR_WHITE (SDL_Color){0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE}
#define SDL_COLOR_GRAY (SDL_Color){240, 240, 240, SDL_ALPHA_OPAQUE}
#define SDL_COLOR_RED (SDL_Color){0xFF, 0, 0, SDL_ALPHA_OPAQUE}
#define SDL_COLOR_GREEN (SDL_Color){0, 0xFF, 0, SDL_ALPHA_OPAQUE}
#define SDL_COLOR_BLUE (SDL_Color){0, 0, 0xFF, SDL_ALPHA_OPAQUE}

#define WINDOW_BG_COLOR_DEFAULT SDL_COLOR_RED


typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Color bg_color;
} RenderWindow;



/********************************* 
***********  SCROLLER  ***********
**********************************/


#define SCROLLER_BG_COLOR SDL_COLOR_WHITE
#define SCROLLER_TEXT_COLOR SDL_COLOR_BLACK

#define SCROLLER_POS_X WIN_WIDTH/2
#define SCROLLER_POS_Y (3*WIN_HEIGHT)/4
#define SCROLLER_VERT_PADDING 10
#define SCROLLER_HORI_PADDING 20

typedef enum {
    THREE_BY_THREE,
    FOUR_BY_FOUR,
    FIVE_BY_FIVE,
    SIX_BY_SIX,
    SEVEN_BY_SEVEN,
    NUM_CUBE_TYPES
} Cube_Type;
extern const char *Cube_Types_str[NUM_CUBE_TYPES];

#define NUM_SCROLLER_TEXTURES NUM_CUBE_TYPES
typedef struct {
    SDL_Texture *textures[NUM_SCROLLER_TEXTURES];
    Cube_Type current_type;
    SDL_Rect dst_rects[NUM_SCROLLER_TEXTURES];
} Scroller;



typedef struct {
    SDL_Texture **textures; // the textures of the items to show for each element in the list
    size_t textures_len;
    size_t textures_cap;
    int current;
    Vector2i pos; // where the dropdown menu is located on the screen
    Vector2i size; // how much space it takes up
} DropDown;



extern RenderWindow     *rw_init(
                            const char  *window_name,
                            Uint32      SDL_flags,
                            int         IMG_flags,
                            Uint32      window_flags,
                            Uint32      renderer_flags
                        );
extern void             rw_cleanup(RenderWindow *rw);



static inline void rw_set_bg_color(RenderWindow *rw, SDL_Color color) {
    rw->bg_color = color;
}
static inline void rw_clear(RenderWindow *rw) {
    SDL_SetRenderDrawColor(rw->renderer, rw->bg_color.r, rw->bg_color.g, rw->bg_color.b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rw->renderer);
}
static inline void rw_present(RenderWindow *rw) {
    SDL_RenderPresent(rw->renderer);
}

extern void rw_render_time(RenderWindow *rw, SDL_Texture *time_texture);
extern void rw_render_scramble(RenderWindow *rw, SDL_Texture *scramble_texture);
extern void rw_render_text(RenderWindow *rw, SDL_Texture *text_texture, int x, int y);
extern void rw_render_scroller(RenderWindow *rw, Scroller *scroller, bool hovering);
extern void rw_render_dropdown(RenderWindow *rw, DropDown *dd);



/********************************* 
***********  SCROLLER  ***********
**********************************/

static inline void __scroller_cycle_type(Scroller *scroller) {
    // won't need to assert; this is used inside another function which already has checked
    scroller->current_type = (scroller->current_type + 1) % NUM_CUBE_TYPES;
}

extern SDL_Texture  *scroller_generate_texture(RenderWindow *rw, TTF_Font *font, const char *text);
extern Scroller     *scroller_init(RenderWindow *rw, TTF_Font *font);
extern void         scroller_check_click(int mouse_x, int mouse_y, Scroller *scroller);
extern bool         scroller_check_hover(int mouse_x, int mouse_y, Scroller *scroller);
extern void         scroller_free(Scroller *scroller);


extern void dropdown_init(DropDown *dd, RenderWindow *rw, TTF_Font *font, Vector2i pos, Vector2i size);
extern void dropdown_close(DropDown *dd);
extern void dropdown_add_text(DropDown *dd, RenderWindow *rw, TTF_Font *font, const char *text);







/******************************* 
************  FONT *************
********************************/

#define FONT_STYLE TTF_STYLE_NORMAL
#define FONT_OUTLINE 0
#define FONT_KERNING 1
#define FONT_HINTING TTF_HINTING_NORMAL

#define SCRAMBLE_FONT_PADDING 10

typedef enum {
    TIME_FONT,
    SCRAMBLE_FONT,
    INSPECT_FONT,
    SCROLLER_FONT,
    OTHER_FONT,
    FONT_COUNT
} Fonts;

const static int Font_Sizes[FONT_COUNT] = {72, 24, 24, 30, 16};

typedef struct {
    TTF_Font *fonts[FONT_COUNT];
} Font_Info;



extern Font_Info    *ttf_load_fonts(const char *filepath);
extern SDL_Texture  *ttf_render_time(        // in MM:SS:mSmSmS format
                        RenderWindow *rw,
                        TTF_Font *font,
                        float time,
                        SDL_Color color
                    );
extern SDL_Texture  *ttf_render_scramble(RenderWindow *rw,
                        TTF_Font *font,
                        char *scramble,
                        SDL_Color color
                    );
extern SDL_Texture  *ttf_render_text(RenderWindow *rw,
                        TTF_Font *font,
                        char *text,
                        SDL_Color color
                    );
extern void         ttf_free_fonts(Font_Info *font_info);   

#endif // WINDOW_H