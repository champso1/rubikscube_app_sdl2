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




/**
 * This calls initialiation functions for sdl stuff, sdl_image stuff, and sdl_ttf stuff.
 * 
 * \param window_name The name for the window, passed to `SDL_CreateWindow()` 
 * \param SDL_flags The flags to be passed to `SDL_Init()`
 * \param IMG_flags The flags to be passed to `IMG_Init()`
 * \param window_flags The flags to be passed to `SDL_CreateWindow()`
 * \param renderer_flags The flags to be passed to `SDL_CreateRenderer()`
 * 
 * \returns A pointer to a RenderWindow (or program exits if there is a failure)
*/
extern RenderWindow *rw_init(
    const char  *window_name,
    Uint32      SDL_flags,
    int         IMG_flags,
    Uint32      window_flags,
    Uint32      renderer_flags
);

/**
 * This calls the associated SDL destory/cleanup functions for the renderers and windows.
 * \param rw The RenderWindow pointer containing the renderer and window
 * \returns Nothing
*/
extern void            rw_cleanup(RenderWindow *rw);



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

/**
 * Renders the time texture
 * 
 * \param rw Pointer to `RenderWindow` structure
 * \param text Pointer to the texture containing the time
 * 
 * \returns Nothing
*/
extern void rw_render_time(RenderWindow *rw, SDL_Texture *time_texture);


/**
 * Renders the scramble
 * 
 * \param rw Pointer to `RenderWindow` structure
 * \param text Pointer to the texture containing the scramble
 * 
 * \returns Nothing
*/
extern void rw_render_scramble(RenderWindow *rw, SDL_Texture *scramble_texture);


/**
 * Renders text to show during inspection time
 * 
 * \param rw Pointer to `RenderWindow` structure
 * \param inspection_text_texture Texture of the text.
 * 
 * \returns Nothing.
*/
extern void rw_render_inspection_text(RenderWindow *rw, SDL_Texture *inspection_text_texture);






/********************************* 
***********  SCROLLER  ***********
**********************************/

/**
 * Turns text into a scroller texture, where the text (black) is rendered in a gray box
 * 
 * \param rw The `RenderWindow`.
 * \param font The font to use.
 * \param text The text to render.
 * 
 * \returns An `SDL_Texture *` to be used for `scroller_init()`
*/
extern SDL_Texture *scroller_generate_texture(RenderWindow *rw, TTF_Font *font, const char *text);


/** 
 * Generates a scroller with all the textures for all the different cube types
 * 
 * \param rw The `RenderWindow` structure.
 * \param font The font to use.
*/
extern Scroller *scroller_init(RenderWindow *rw, TTF_Font *font);

/**
 * Internal function to be used in the `scroller_check_click()`, where it will,
 * on click, cycle the cube type to the next type
 * 
 * \param scroller The scroller.
 * 
 * \returns Nothing.
*/
static inline void __scroller_cycle_type(Scroller *scroller) {
    // won't need to assert; this is used inside another function which already has checked
    scroller->current_type = (scroller->current_type + 1) % NUM_CUBE_TYPES;
}

/** 
 * As of now, this just outputs a message to the console if the surface is clicked.
 * 
 * \param mouse_x Mouse x position
 * \param mouse_y Mouse y position
 * \param scroller_texture The scroller.
 * 
 * \returns Nothing
*/
extern void scroller_check_click(int mouse_x, int mouse_y, Scroller *scroller);


/**
 * Similar to `scroller_check_click`, but this just checks if the mouse is currently hovering over the scroller
 * 
 * \param mouse_x Mouse x position
 * \param mouse_y Mouse y position
 * \param scroller The scroller.
 * 
 * \returns A boolean: `true` if the mouse is hovering over the texture, `false` if not.
*/
extern bool scroller_check_hover(int mouse_x, int mouse_y, Scroller *scroller);


/**
 * Renders a scroller texture at a predefined position.
 * If the mouse is hovering over the scroller, the padding is increased so that the scroller appears larger
 * 
 * \param rw Pointer to `RenderWindow` structure
 * \param scroller_texture The scroller texture to render
 * \param hovering A boolean signifying if the mouse is hovering over the scroller or not
 * 
 * \returns Nothing.
*/
extern void rw_render_scroller(RenderWindow *rw, Scroller *scroller, bool hovering);



/**
 * Frees memory allocated in a `Scroller`
 * 
 * \param scroller The `Scroller`
 * 
 * \returns Nothing. 
*/
extern void scroller_free(Scroller *scroller);









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
    FONT_COUNT
} Fonts;

const static int Font_Sizes[FONT_COUNT] = {72, 24, 24, 30};

typedef struct {
    TTF_Font *fonts[FONT_COUNT];
} Font_Info;

/**
 * Initializes a Font_Info structure containing all the different font sizes
 * 
 * \param filepath Path to the .ttf file
 * 
 * \returns A pointer to a `Font_Info` structure containing the initialized data.
*/
extern Font_Info *ttf_load_fonts(const char *filepath);

/**
 * Renders a time in the format MM::SS::mmm into an `SDL_Texture`
 * 
 * \param rw Pointer to `RenderWindow` structure
 * \param font_info Pointer to `Font_Info` structure
 * \param time time is a float in seconds
 * \param color Color the time should be
 * 
 * \returns `SDL_Texture *` containing the rendered time 
 * which can then be passed to `rw_render_time()`.
*/
extern SDL_Texture *ttf_render_time(
    RenderWindow *rw,
    Font_Info *font_info,
    float time,
    SDL_Color color
);

/**
 * Renders the scramble to a texture.
 * 
 * \param rw Pointer to `RenderWindow` structure
 * \param font_info Pointer to `Font_Info` structure
 * \param scramble String containing the scramble.
 * \param color Color the scramble will be
 * 
 * \returns `SDL_Texture *` containing the scramble text 
 * which can then be passed to `rw_render_scramble()`.
*/
extern SDL_Texture *ttf_render_scramble(
    RenderWindow *rw,
    Font_Info *font_info,
    char *scramble,
    SDL_Color color
);

/**
 * Creates a texture of the text to show during inspection time
 * 
 * \param rw Pointer to `RenderWindow` structure
 * \param font_info Pointer to `Font_Info structure
 * \param inspection_text The text to be rendered to a texture
 * \param color Color the text will be
 * 
 * \returns `SDL_Texture *` containing the text. 
 * To be passed to `rw_render_inspection_text()`.
*/
extern SDL_Texture *ttf_render_inspection_text(
    RenderWindow *rw,
    Font_Info *font_info,
    char *inspection_text,
    SDL_Color color
);


/**
 * Closes the fonts that were opened from `ttf_load_font()`
 * 
 * \param font_info Pointer to `Font_Info` struct
 * \returns Nothing
*/
extern void ttf_free_fonts(Font_Info *font_info);   



#endif // WINDOW_H