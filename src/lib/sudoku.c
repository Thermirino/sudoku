#include <stdlib.h>
#include <sudoku.h>
#include <board.h>
#include <solver.h>
#include <draw.h>
#include <input.h>

static game_state* game_state_create(int window_width, int window_height,
                                     int board_size, int empty_cells);
static void game_state_destroy(game_state* gs);

static int get_empty_cells(dlvl level)
{
    int empty_cells;
    switch (level)
    {
    case ONE_MOVE:
        empty_cells = ONE_MOVE_LVL;
        break;
    case EASY:
        empty_cells = EASY_LVL;
        break;
    case MEDIUM:
        empty_cells = MEDIUM_LVL;
        break;
    case HARD:
        empty_cells = HARD_LVL;
        break;
    default:
        empty_cells = EASY_LVL;
        break;
    }
    return empty_cells;
}

game_state* game_init(int window_width, int window_height,
                      int board_size, dlvl level)
{
    srand((unsigned int)time(NULL));
    if (!draw_init(window_width, window_height))
        return NULL;

    int empty_cells = get_empty_cells(level);
    game_state* gs = game_state_create(window_width, window_height, 
                                       board_size, empty_cells);
    return gs;
}

bool game_restart(game_state* gs)
{
    int empty_cells = get_empty_cells(gs->cur_dlvl);
    if (!board_fill(gs->brd))
    {
        return false;
    }
    if (!board_set(gs->board_solved, gs->brd))
    {
        return false;
    }
    if (!board_remove_cells(empty_cells, gs->brd))
    {
        return false;
    }
    gs->ninvalid_cells = 0;

    gs->selected_cell.row = -1;
    gs->selected_cell.col = -1;
    gs->solved = false;
    gs->start_time = time(NULL);
    gs->finish_time = gs->start_time;
    return true;
}

void game_end(game_state* gs)
{
    draw_end();
    game_state_destroy(gs);
}

static game_state* game_state_create(int window_width, int window_height,
                                     int board_size, int empty_cells)
{
    game_state* gs = malloc(sizeof(*gs));
    if (!gs)
        return NULL;
    gs->brd = board_create(board_size);
    if (!gs->brd)
    {
        free(gs);
        return NULL;
    }
    if (!board_fill(gs->brd))
    {
        board_destroy(gs->brd);
        free(gs);
        return NULL;
    }
    gs->board_solved = board_copy(gs->brd);
    if (!gs->board_solved)
    {
        board_destroy(gs->brd);
        free(gs);
        return NULL;
    }
    if (!board_remove_cells(empty_cells, gs->brd))
    {
        board_destroy(gs->board_solved);
        board_destroy(gs->brd);
        free(gs);
        return NULL;
    }
    gs->invalid_cells = malloc(sizeof(*gs->invalid_cells) * (gs->brd->size * gs->brd->size));
    if (!gs->invalid_cells)
    {
        board_destroy(gs->board_solved);
        board_destroy(gs->brd);
        free(gs);
        return NULL;
    }
    gs->ninvalid_cells = 0;
    gs->solver_delay = 50;
    gs->cur_dlvl = 0;
    gs->selected_cell.row = -1;
    gs->selected_cell.col = -1;
    gs->solved = false;
    gs->start_time = time(NULL);
    gs->finish_time = gs->start_time;

    update_coords(gs, window_width, window_height);
    return gs;
}

static void game_state_destroy(game_state* gs)
{
    board_destroy(gs->brd);
    board_destroy(gs->board_solved);
    free(gs->invalid_cells);
    free(gs);
}

static void game_board_draw(game_state* gs)
{
    draw_board(gs->brd, gs->board_y, gs->board_x, gs->board_side_size);
    color highlight_color = colors[HIGHLIGHT_COLOR];
    if (gs->selected_cell.row != -1)
        draw_board_higlight_row(gs->brd, gs->board_y, gs->board_x, gs->board_side_size,
            gs->selected_cell.row, 
            highlight_color.r, highlight_color.g, highlight_color.b, HIGHLIGHT_COLOR_ALPHA);
    if (gs->selected_cell.col != -1)
        draw_board_higlight_col(gs->brd, gs->board_y, gs->board_x, gs->board_side_size,
            gs->selected_cell.col, 
            highlight_color.r, highlight_color.g, highlight_color.b, HIGHLIGHT_COLOR_ALPHA);
    if (gs->selected_cell.row != -1 && gs->selected_cell.col != -1)
    {
        int box = gs->selected_cell.row / gs->brd->box_size * gs->brd->box_size +
            gs->selected_cell.col / gs->brd->box_size;
        draw_board_higlight_box(gs->brd, gs->board_y, gs->board_x, gs->board_side_size,
                box, 
                highlight_color.r, highlight_color.g, highlight_color.b, HIGHLIGHT_COLOR_ALPHA);
    }
    if (gs->ninvalid_cells > 0)
    {
        color invalid_cell_color = colors[INVALID_CELL_COLOR];
        for (int i = 0; i < gs->ninvalid_cells; i++)
            draw_board_higlight_cell(gs->brd, gs->board_y, gs->board_x, gs->board_side_size,
                gs->invalid_cells[i].row, gs->invalid_cells[i].col,
                invalid_cell_color.r, invalid_cell_color.g, invalid_cell_color.b, INVALID_CELL_COLOR_ALPHA);
    }
}

static void game_dlvls_draw(game_state* gs)
{
    draw_difficulty_levels_highlight(gs->dlvls_y, gs->dlvls_x,
        gs->dlvls_w, gs->dlvls_h, gs->cur_dlvl);
    draw_difficulty_levels(gs->dlvls_y, gs->dlvls_x,
        gs->dlvls_w, gs->dlvls_h);
}

static void game_time_draw(game_state* gs)
{
    draw_time(gs->time_y, gs->time_x, gs->start_time, time(NULL));
}

static void game_help_info_draw(game_state* gs)
{
    draw_help_info(gs->help_y, gs->help_x);
}

static void game_solver_delay_draw(game_state* gs)
{
    draw_solver_delay(gs->solver_delay_y, gs->solver_delay_x, gs->solver_delay);
}

static void solved_popup_draw(game_state* gs)
{
    draw_solved_popup(gs->solved_popup_y, gs->solved_popup_x,
        gs->solved_popup_w, gs->solved_popup_h,
        gs->try_again_btn_y, gs->try_again_btn_x, gs->try_again_btn_w, gs->try_again_btn_h,
        gs->start_time, gs->finish_time);
}

static void render_frame(game_state* gs)
{
    draw_clear();
    game_solver_delay_draw(gs);
    game_help_info_draw(gs);
    if (!gs->solved)
        game_time_draw(gs);
    game_dlvls_draw(gs);
    game_board_draw(gs);
    if (gs->solved)
        solved_popup_draw(gs);
    draw_present();
}

static bool cbf(board* b, void* ctx)
{
    (void)b;
    game_state* gs = (game_state*)ctx;
    render_frame(gs);
    draw_present();
    draw_sleep(gs->solver_delay);
    event e;
    while ((e = get_event()).type != NO_EVENT)
    {
        if (e.type == WINDOW_RESIZED)
        {
            update_coords(gs, e.resize.window_width, e.resize.window_height);
        }
        if (e.type == KEYBOARD_BUTTON && e.keyboard.key == KEYBOARD_S)
        {
            return false;
        }
    }
    return true;
}

void game_start(game_state* gs)
{
    event e;
    time_t last_render_time = time(NULL);
    render_frame(gs);
    while ((e = get_event()).type != QUIT)
    {
        if (e.type == WINDOW_RESIZED)
        {
            update_coords(gs, e.resize.window_width, e.resize.window_height);
        }
        else if (e.type == MOUSE_BUTTON)
        {
            if (!gs->solved)
            {
                int x = e.mouse.x;
                int y = e.mouse.y;
                int row;
                int col;
                int lvl;
                if (coords_to_cell(gs->brd, gs->board_y, gs->board_x, y, x,
                    gs->board_side_size, &row, &col))
                {
                    gs->selected_cell.col = col;
                    gs->selected_cell.row = row;
                }
                else if (coords_to_dlvl(gs->dlvls_y, gs->dlvls_x, gs->dlvls_w, gs->dlvls_h,
                                        y, x, &lvl))
                {
                    gs->cur_dlvl = lvl;
                    if (!game_restart(gs))
                    {
                        return;
                    }
                }
            }
            else if (e.mouse.x >= gs->try_again_btn_x && e.mouse.y >= gs->try_again_btn_y &&
                     e.mouse.x <= gs->try_again_btn_x + gs->try_again_btn_w &&
                     e.mouse.y <= gs->try_again_btn_y + gs->try_again_btn_h)
            {
                if (!game_restart(gs))
                {
                    return;
                }
            }
        }
        else if (e.type == KEYBOARD_BUTTON)
        {
            if (e.keyboard.key == KEYBOARD_S)
            {
                solutions* sols = board_solve(gs->brd, cbf, gs);
                if (sols->capacity > 0)
                    board_set(gs->brd, sols->boards[0]);
                solutions_destroy(sols);
            }
            else if(e.keyboard.key == KEYBOARD_PLUS)
            {
                if (gs->solver_delay < 200)
                    gs->solver_delay += 10;
            }
            else if (e.keyboard.key == KEYBOARD_MINUS)
            {
                if (gs->solver_delay > 0)
                gs->solver_delay -= 10;
            }
            else if (gs->selected_cell.row != -1 && gs->selected_cell.col != -1 &&
                gs->brd->cells[gs->selected_cell.row][gs->selected_cell.col].fixed == 0)
            {
                int digit = -1;
                switch (e.keyboard.key)
                {
                case KEYBOARD_1:
                    digit = 1;
                    break;
                case KEYBOARD_2:
                    digit = 2;
                    break;
                case KEYBOARD_3:
                    digit = 3;
                    break;
                case KEYBOARD_4:
                    digit = 4;
                    break;
                case KEYBOARD_5:
                    digit = 5;
                    break;
                case KEYBOARD_6:
                    digit = 6;
                    break;
                case KEYBOARD_7:
                    digit = 7;
                    break;
                case KEYBOARD_8:
                    digit = 8;
                    break;
                case KEYBOARD_9:
                    digit = 9;
                    break;
                case KEYBOARD_0:
                    digit = 0;
                    break;
                case KEYBOARD_O:
                    digit = gs->board_solved->cells[gs->selected_cell.row][gs->selected_cell.col].val;
                default:
                    break;
                }
                if (digit != -1)
                    board_place(gs->brd, gs->selected_cell.row, gs->selected_cell.col,
                        digit);
                }
        }

        board_is_valid(gs->brd, gs->invalid_cells, &gs->ninvalid_cells);
        if (!gs->solved && board_is_equal(gs->brd, gs->board_solved))
        {
            gs->solved = true;
            gs->finish_time = time(NULL);
        }

        if (e.type != NO_EVENT ||
            difftime(time(NULL), last_render_time) > 0)
        {
            render_frame(gs);
            last_render_time = time(NULL);
        }
    }
}
