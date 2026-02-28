#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>
#include <time.h>
#include <board.h>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define BG_COLOR WHITE
#define BG_COLOR_ALPHA 0xff
#define OUTER_BORDER_COLOR BLACK
#define OUTER_BORDER_COLOR_ALPHA 0xff
#define BOX_BORDER_COLOR BLACK
#define BOX_BORDER_COLOR_ALPHA 0xff
#define CELL_BORDER_COLOR BLACK
#define CELL_BORDER_COLOR_ALPHA 0x5a
#define INVALID_CELL_COLOR PINK
#define INVALID_CELL_COLOR_ALPHA 0x5a
#define HIGHLIGHT_COLOR BLUE
#define HIGHLIGHT_COLOR_ALPHA 0x2a
#define FIXED_DIGIT_COLOR BLACK
#define FIXED_DIGIT_COLOR_ALPHA 0xff
#define USER_DIGIT_COLOR ORANGE
#define USER_DIGIT_COLOR_ALPHA 0xff

#define DLEVELS_HIGLIGHT_COLOR YELLOW
#define DLEVELS_HIGLIGHT_COLOR_ALPHA 0x5a
#define DLEVELS_LINE_COLOR BLACK
#define DLEVELS_LINE_COLOR_ALPHA 0xff
#define DLEVELS_TEXT_COLOR BLACK
#define DLEVELS_TEXT_COLOR_ALPHA 0xff

#define TIME_TEXT_COLOR BLACK
#define TIME_TEXT_COLOR_ALPHA 0xff

#define SOLVED_POPUP_BG_COLOR BLUE
#define SOLVED_POPUP_BG_COLOR_ALPHA 0xff
#define SOLVED_POPUP_TEXT_COLOR BLACK
#define SOLVED_POPUP_TEXT_COLOR_ALPHA 0xff
#define TRYAGAIN_BUTTON_BG_COLOR ORANGE
#define TRYAGAIN_BUTTON_BG_COLOR_ALPHA 0xff
#define TRYAGAIN_BUTTON_TEXT_COLOR BLACK
#define TRYAGAIN_BUTTON_TEXT_ALPHA 0xff
#define HELP_INFO_TEXT_COLOR BLACK
#define HELP_INFO_TEXT_ALPHA 0xff
#define SOLVER_DELAY_TEXT_COLOR BLACK
#define SOLVER_DELAY_TEXT_ALPHA 0xff

typedef enum 
{
    WHITE = 0,
    BLACK,
    YELLOW,
    ORANGE,
    PINK,
    PURPLE,
    BLUE
} color_name;

typedef struct
{
    int r, g, b;
} color;

extern color colors[];
extern size_t ncolors;

extern const char* difficulty_levels[];
extern size_t ndlevels;

bool draw_init(int screen_width, int screen_height);
void draw_set_color(color_name cname, int alpha);

bool coords_to_cell(board* b, int board_y, int board_x,
                    int y, int x, int board_side_size, 
                    int* row, int* col);
void draw_board(board* b, int y, int x,
                int board_side_size);
void draw_board_higlight_cell(board* brd,
                              int board_y, int board_x, int board_side_size,
                              int row, int col, int r, int g, int b, int a);
void draw_board_higlight_row(board* brd,
                             int board_y, int board_x, int board_side_size,
                             int row, int r, int g, int b, int a);
void draw_board_higlight_col(board* brd,
                             int board_y, int board_x, int board_side_size,
                             int col, int r, int g, int b, int a);
void draw_board_higlight_box(board* brd,
                             int board_y, int board_x, int board_side_size,
                             int box, int r, int g, int b, int a);

bool coords_to_dlvl(int dlvls_y, int dlvls_x,
                    int dlvls_w, int dlvls_h, int y, int x, int* lvl);
void draw_difficulty_levels(int y, int x, 
                            int width, int height);
void draw_difficulty_levels_highlight(int y, int x, 
                                      int width, int height, int lvl);

void draw_time(int y, int x, time_t start_time);
void draw_help_info(int y, int x);
void draw_solved_popup(int y, int x, int width, int height,
                       int* btn_y, int* btn_x, int* btn_w, int* btn_h,
                       time_t start_time);
void draw_solver_delay(int y, int x, int delay);

void draw_clear(void);
void draw_end(void);
void draw_present(void);
void draw_sleep(int time_ms);

#endif
