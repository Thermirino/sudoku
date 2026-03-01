#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdbool.h>
#include <board.h>
#include <time.h>

// Number of empty cells
#define ONE_MOVE_LVL 1
#define EASY_LVL 35
#define MEDIUM_LVL 46
#define HARD_LVL 55

// difficulty levels
typedef enum
{
    ONE_MOVE = 0,
    EASY,
    MEDIUM,
    HARD
} dlvl;

typedef struct game_state game_state;

struct game_state
{
    int win_width, win_height;
    int board_x, board_y;                  // coords of the upper left corner
    int board_side_size;                   // board side size in pixels
    int dlvls_y, dlvls_x;                  // coords of the upper left corner
    int dlvls_w, dlvls_h;                  // table width and height
    int time_y, time_x;                    // coords of the center
    int solved_popup_y, solved_popup_x;    // coords of the upper left corner
    int solved_popup_w, solved_popup_h;
    int try_again_btn_y, try_again_btn_x;
    int try_again_btn_w, try_again_btn_h;
    int help_y, help_x;
    int solver_delay_y, solver_delay_x;
    int solver_delay;
    int cur_dlvl;                          // current difficulty level (starting from 0)
    position selected_cell;
    position* invalid_cells;
    int ninvalid_cells;
    board* brd;
    board* board_solved;
    bool solved;
    time_t start_time;
    time_t finish_time;
};

game_state* game_init(int window_width, int window_height,
                      int board_size, dlvl level);
bool game_restart(game_state* gs);
void game_start(game_state* gs);
void game_end(game_state* gs);

#endif
