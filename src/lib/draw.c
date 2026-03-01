#include <SDL_video.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_timer.h>
#include <draw.h>

#define nelements(arr) (sizeof(arr) / sizeof(*arr))

const char* difficulty_levels[] = {
    "one move", "easy", "medium", "hard"
};
size_t ndlevels = nelements(difficulty_levels);

color colors[] = {
    { 0xff, 0xff, 0xff },
    { 0, 0, 0 },
    { 0xff, 0xbe, 0x0b },
    { 0xfb, 0x56, 0x07 },
    { 0xff, 0x00, 0x6e },
    { 0x83, 0x38, 0xec },
    { 0x3a, 0x86, 0xff },
};
size_t ncolors = nelements(colors);

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static TTF_Font* digits_font = NULL;
static TTF_Font* text_font = NULL;
static int text_font_size = -1;
static int SCREEN_WIDTH;
static int SCREEN_HEIGHT;

static void draw_label(const char* text, int x, int y,
                       color_name cname, int alpha, TTF_Font* font);

bool draw_init(int screen_width, int screen_height)
{
    if (screen_width <= 0)
    {
        screen_width = 1024;
    }
    if (screen_height <= 0)
    {
        screen_height = 512;
    }
    SCREEN_WIDTH = screen_width;
    SCREEN_HEIGHT = screen_height;

    bool ret = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        ret = false;
        goto failed;
    }
    window = SDL_CreateWindow("Sudoku", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        ret = false;
        goto failed;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        ret = false;
        goto failed;
    }
    if (TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        ret = false;
        goto failed;
    }
    text_font_size = screen_height / 20;
    text_font = TTF_OpenFont("assets/fonts/sans.ttf", text_font_size);
    if (text_font == NULL)
    {
        fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
        return false;
    }
    digits_font = NULL;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    draw_clear();

    return true;

failed:
    TTF_CloseFont(text_font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return ret;
}

void draw_end(void)
{
    TTF_CloseFont(digits_font);
    TTF_CloseFont(text_font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    renderer = NULL;
    window = NULL;
    digits_font = NULL;
    text_font = NULL;
}

void update_coords(game_state* gs, int window_width, int window_height)
{
    gs->win_width = window_width;
    gs->win_height = window_height;

    int min_screen_side_size = window_width < window_height ?
                               window_width : window_height;
    int padding_size = (int)(min_screen_side_size * 0.1);
    int board_side_size = (min_screen_side_size - padding_size * 2) / 
                           gs->brd->size * gs->brd->size;
    gs->board_x = (window_width - board_side_size) / 2;
    gs->board_y = (window_height - board_side_size) / 2;
    gs->board_side_size = board_side_size;

    gs->dlvls_h = window_height / 5;
    gs->dlvls_w = window_width / 5;
    gs->dlvls_y = window_height / 2 - gs->dlvls_h / 2;
    gs->dlvls_x = gs->board_x / 2 - gs->dlvls_w / 2;

    gs->time_y = padding_size / 2;
    gs->time_x = window_width / 2;

    gs->solved_popup_w = window_width / 2;
    gs->solved_popup_h = window_height / 3;
    gs->solved_popup_y = (window_height - gs->solved_popup_h) / 2;
    gs->solved_popup_x = (window_width - gs->solved_popup_w) / 2;

    gs->try_again_btn_w = gs->solved_popup_w / 3;
    gs->try_again_btn_h = gs->solved_popup_h / 3;
    gs->try_again_btn_x = gs->solved_popup_x + (gs->solved_popup_w - gs->try_again_btn_w) / 2;
    gs->try_again_btn_y = gs->solved_popup_y + gs->solved_popup_h - 
                          (gs->solved_popup_h - gs->try_again_btn_h) * 2 / 3;

    int hpadding = (window_width - gs->board_side_size) / 2;
    gs->help_y = gs->board_y;
    gs->help_x = (gs->board_x + gs->board_side_size) + (int)(hpadding * 0.2);

    gs->solver_delay_y = gs->board_y;
    gs->solver_delay_x = gs->dlvls_x + gs->dlvls_w / 2;
}

void draw_set_color(color_name cname, int alpha)
{
    color c = colors[cname];
    SDL_SetRenderDrawColor(renderer,
        (Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)alpha);
}

static void draw_label(const char* text, int x, int y,
                       color_name cname, int alpha, TTF_Font* font)
{
    color c = colors[cname];
    SDL_Color fg = { (Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)alpha };
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, fg);
    int w = surface->w;
    int h = surface->h;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){ x - w / 2, y - h / 2, w, h });
    SDL_DestroyTexture(texture);
}

static void draw_label_lu(const char* text, int x, int y,
                          color_name cname, int alpha, TTF_Font* font)
{
    color c = colors[cname];
    SDL_Color fg = { (Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)alpha };
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, fg);
    int w = surface->w;
    int h = surface->h;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){ x, y, w, h });
    SDL_DestroyTexture(texture);
}

void draw_board(board* b, int y, int x, int board_side_size)
{
    UNUSED(b);
    int cell_size = board_side_size / b->size;

    if (digits_font == NULL)
        digits_font = TTF_OpenFont("assets/fonts/sans.ttf", cell_size / 2);
    if (digits_font == NULL)
    {
        fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
        return;
    }

    color outer_border_color = colors[OUTER_BORDER_COLOR];
    SDL_SetRenderDrawColor(renderer,
        (Uint8)outer_border_color.r, (Uint8)outer_border_color.g, (Uint8)outer_border_color.b,
        OUTER_BORDER_COLOR_ALPHA);
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = board_side_size,
        .h = board_side_size
    };
    SDL_RenderDrawRect(renderer, &rect);

    color cell_border_color = colors[CELL_BORDER_COLOR];
    SDL_SetRenderDrawColor(renderer,
        (Uint8)cell_border_color.r, (Uint8)cell_border_color.g, (Uint8)cell_border_color.b,
        CELL_BORDER_COLOR_ALPHA);
    for (int i = 1; i < b->size; i++)
    {
        int x1 = rect.x + i * cell_size;
        SDL_RenderDrawLine(renderer, x1, rect.y, x1, rect.y + board_side_size);

        int y1 = rect.y + i * cell_size;
        SDL_RenderDrawLine(renderer, rect.x, y1, rect.x + board_side_size, y1);
    }

    color box_border_color = colors[BOX_BORDER_COLOR];
    SDL_SetRenderDrawColor(renderer,
        (Uint8)box_border_color.r, (Uint8)box_border_color.g, (Uint8)box_border_color.b,
        BOX_BORDER_COLOR_ALPHA);
    for (int i = b->box_size; i < b->size; i += b->box_size)
    {
        int x1 = rect.x + i * cell_size;
        SDL_RenderDrawLine(renderer, x1, rect.y, x1, rect.y + board_side_size);

        int y1 = rect.y + i * cell_size;
        SDL_RenderDrawLine(renderer, rect.x, y1, rect.x + board_side_size, y1);
    }

    for (int row = 0; row < b->size; row++)
    {
        for (int col = 0; col < b->size; col++)
        {
            int num = b->cells[row][col].val;
            bool fixed = b->cells[row][col].fixed;
            if (num != 0)
            {
                static char text[2] = { 0, 0 };
                text[0] = (char)num + '0';
                int yl = rect.y + row * cell_size + cell_size / 2;
                int xl = rect.x + col * cell_size + cell_size / 2;
                draw_label(text, xl, yl,
                    fixed == true ? FIXED_DIGIT_COLOR : USER_DIGIT_COLOR,
                    fixed == true ? FIXED_DIGIT_COLOR_ALPHA : USER_DIGIT_COLOR_ALPHA,
                    digits_font);
            }
        }
    }
}

bool coords_to_cell(board* b, int board_y, int board_x, int y, int x,
                    int board_side_size, int* row, int* col)
{
    if (y < board_y || x < board_x ||
        y >= board_y + board_side_size || x >= board_x + board_side_size)
        return false;
    int cell_size = board_side_size / b->size;
    *row = (y - board_y) / cell_size;
    *col = (x - board_x) / cell_size;
    return true;
}

void draw_board_higlight_cell(board* brd, 
                              int board_y, int board_x, int board_side_size,
                              int row, int col, int r, int g, int b, int a)
{
    int cell_size = board_side_size / brd->size;
    SDL_Rect rect = {
        .x = board_x + col * cell_size,
        .y = board_y + row * cell_size,
        .w = cell_size,
        .h = cell_size
    };
    SDL_SetRenderDrawColor(renderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    SDL_RenderFillRect(renderer, &rect);
}

void draw_board_higlight_row(board* brd,
                             int board_y, int board_x, int board_side_size,
                             int row, int r, int g, int b, int a)
{
    int cell_size = board_side_size / brd->size;
    SDL_Rect rect = {
        .x = board_x,
        .y = board_y + row * cell_size,
        .w = board_side_size,
        .h = cell_size
    };
    SDL_SetRenderDrawColor(renderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    SDL_RenderFillRect(renderer, &rect);
}

void draw_board_higlight_col(board* brd,
                             int board_y, int board_x, int board_side_size,
                             int col, int r, int g, int b, int a)
{
    int cell_size = board_side_size / brd->size;
    SDL_Rect rect = {
        .x = board_x + col * cell_size,
        .y = board_y,
        .w = cell_size,
        .h = board_side_size
    };
    SDL_SetRenderDrawColor(renderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    SDL_RenderFillRect(renderer, &rect);
}

void draw_board_higlight_box(board* brd,
                             int board_y, int board_x, int board_side_size,
                             int box, int r, int g, int b, int a)
{
    int cell_size = board_side_size / brd->size;
    int row = box / brd->box_size * brd->box_size;
    int col = box % brd->box_size * brd->box_size;
    SDL_Rect rect = {
        .x = board_x + col * cell_size,
        .y = board_y + row * cell_size,
        .w = cell_size * brd->box_size,
        .h = cell_size * brd->box_size
    };
    SDL_SetRenderDrawColor(renderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    SDL_RenderFillRect(renderer, &rect);
}

bool coords_to_dlvl(int dlvls_y, int dlvls_x,
                    int dlvls_w, int dlvls_h, int y, int x, int* lvl)
{
    if (y < dlvls_y || x < dlvls_x ||
        y >= dlvls_y + dlvls_h || x >= dlvls_x + dlvls_w)
        return false;
    int n = sizeof(difficulty_levels) / sizeof(*difficulty_levels);
    int h = dlvls_h / n;
    *lvl = (y - dlvls_y) / h;
    if (*lvl >= n)
        *lvl = n - 1;
    return true;
}

void draw_difficulty_levels(int y, int x, int width, int height)
{
    SDL_SetRenderDrawColor(renderer, (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)255);
    SDL_RenderDrawLine(renderer, x, y, x + width, y);
    SDL_RenderDrawLine(renderer, x + width, y, x + width, y + height);
    // SDL_RenderDrawLine(renderer, x + width, y + height, x, y + height);
    SDL_RenderDrawLine(renderer, x, y + height, x, y);

    int n = sizeof(difficulty_levels) / sizeof(*difficulty_levels);
    int h = height / n;
    for (int i = 0; i < n; i++)
    {
        const char* text = difficulty_levels[i];
        int len = (int)strlen(text);
        int y0 = y + h / 2;
        int x0 = x + (width - len) / 2;
        draw_set_color(DLEVELS_LINE_COLOR, DLEVELS_LINE_COLOR_ALPHA);
        SDL_RenderDrawLine(renderer, x, y + h, x + width, y + h);
        draw_label(text, x0, y0, DLEVELS_TEXT_COLOR, DLEVELS_TEXT_COLOR_ALPHA, 
            text_font);
        y += h;
    }
}

void draw_difficulty_levels_highlight(int y, int x,
                                      int width, int height, int lvl)
{
    int n = sizeof(difficulty_levels) / sizeof(*difficulty_levels);
    int h = height / n;
    y += h * lvl;
    SDL_Rect rect = {
        .x = x + 1,
        .y = y + 1,
        .w = width - 1,
        .h = h - 1
    };
    color highlight_color = colors[DLEVELS_HIGLIGHT_COLOR];
    SDL_SetRenderDrawColor(renderer, 
        (Uint8)highlight_color.r, (Uint8)highlight_color.g, (Uint8)highlight_color.b, 
        (Uint8)DLEVELS_HIGLIGHT_COLOR_ALPHA);
    SDL_RenderFillRect(renderer, &rect);
}

static char* str_elapsed_time(int elapsed)
{
    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;
    static char str[16];
    snprintf(str, sizeof(str), "%02d:%02d:%02d", hours, minutes, seconds);
    return str;
}

void draw_time(int y, int x, time_t start_time, time_t cur_time)
{
    int elapsed = (int)difftime(cur_time, start_time);
    char* str = str_elapsed_time(elapsed);
    draw_label(str, x, y, TIME_TEXT_COLOR, TIME_TEXT_COLOR_ALPHA, text_font);
}

void draw_solved_popup(int y, int x, int width, int height,
                       int btn_y, int btn_x, int btn_w, int btn_h,
                       time_t start_time, time_t finish_time)
{
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = width,
        .h = height
    };
    draw_set_color(SOLVED_POPUP_BG_COLOR, SOLVED_POPUP_BG_COLOR_ALPHA);
    SDL_RenderFillRect(renderer, &rect);
    char text[100];
    int elapsed = (int)difftime(finish_time, start_time);
    snprintf(text, sizeof(text), "Finished in %s", str_elapsed_time(elapsed));
    draw_label(text, x + width / 2, y + height / 3, 
        SOLVED_POPUP_TEXT_COLOR, SOLVED_POPUP_TEXT_COLOR_ALPHA,
        text_font);

    SDL_Rect rect2 = {
        .x = btn_x,
        .y = btn_y,
        .w = btn_w,
        .h = btn_h
    };
    draw_set_color(TRYAGAIN_BUTTON_BG_COLOR, TRYAGAIN_BUTTON_BG_COLOR_ALPHA);
    SDL_RenderFillRect(renderer, &rect2);
    draw_label("Play again", rect2.x + rect2.w / 2, rect2.y + rect2.h / 2,
        TRYAGAIN_BUTTON_TEXT_COLOR, TRYAGAIN_BUTTON_TEXT_ALPHA,
        text_font);
}

void draw_help_info(int y, int x)
{
    draw_label_lu("1-9 - place digit", x, y,
            HELP_INFO_TEXT_COLOR, HELP_INFO_TEXT_ALPHA, text_font);
    y += text_font_size;
    draw_label_lu("del - remove digit", x, y,
            HELP_INFO_TEXT_COLOR, HELP_INFO_TEXT_ALPHA, text_font);
    y += text_font_size;
    draw_label_lu("o   - open cell", x, y, 
            HELP_INFO_TEXT_COLOR, HELP_INFO_TEXT_ALPHA, text_font);
    y += text_font_size;
    draw_label_lu("s   - turn on/off autosolver", x, y, 
            HELP_INFO_TEXT_COLOR, HELP_INFO_TEXT_ALPHA, text_font);
    y += text_font_size;
    draw_label_lu("+/- - change solver delay", x, y, 
            HELP_INFO_TEXT_COLOR, HELP_INFO_TEXT_ALPHA, text_font);
}

void draw_solver_delay(int y, int x, int delay)
{
    static char text[30];
    snprintf(text, sizeof(text), "solver delay: %d\n", delay);
    draw_label(text, x, y, 
            SOLVER_DELAY_TEXT_COLOR, SOLVER_DELAY_TEXT_ALPHA, text_font);
}

void draw_clear(void)
{
    color bg_color = colors[BG_COLOR];
    SDL_SetRenderDrawColor(renderer, 
        (Uint8)bg_color.r, (Uint8)bg_color.g, (Uint8)bg_color.b, BG_COLOR_ALPHA);
    SDL_RenderClear(renderer);

}

void draw_present(void)
{
    SDL_RenderPresent(renderer);
}

void draw_sleep(int time_ms)
{
    SDL_Delay(time_ms);
}
