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













// more functions

/* enum {
    SCRAMBLE_TEXTURE,
    INSPECTION_TEXTURE,
    TIME_TEXTURE,
    BEST_TIME_TEXTURE,
    NUM_STATE_TEXTURES
} MainState_Textures;

typedef struct {
    float previous_time;
    float current_time;
    char *scramble;
    char *best_time;
    bool solving;
    bool inspecting;
    bool scrambling;
    bool solve_ready;
    bool running;
    SDL_Event event;
    char *  (*get_best_time)(void);
    char *  (*generate_scramble)(void);
    void    (*solve_save)(uint8_t, float, char *);

    SDL_Texture *textures[NUM_STATE_TEXTURES];
} MainState; 


extern void     init_main_state(
                    MainState * main_state,
                    char * (*scramble_generate_func)(void),
                    char * (*get_best_time_func)(void),
                    void (*solve_save)(uint8_t, float, char *),
                    RenderWindow *rw,
                    Font_Info *font_info
                );
extern bool     handle_input(
                    MainState *main_state,
                    RenderWindow *rw,
                    Font_Info *font_info,
                    Scroller *scroller
                );
extern void     render_textures(MainState *main_state);
extern void     destroy_main_state(MainState *main_state); */



#endif // WINDOW_H