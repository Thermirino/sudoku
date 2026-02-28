#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

typedef struct
{
    int row, col;
} position;

typedef struct
{
    int val;
    bool fixed;
} cell;

typedef struct
{
    cell** cells;
    int size;     // board side size (in cells)
    int box_size; // box side size
} board;

/* Create empty board */
board* board_create(int size);
void board_destroy(board* b);
void board_print(board* b);
bool board_is_empty(board* b);
bool board_is_equal(board* a, board* b);
bool board_is_valid(board* b, position* invalid_cells, int* count);
void board_clear(board* b);
bool board_set(board* dst, board* src);
board* board_copy(board* b);
bool board_place(board* b, int row, int col, int digit);

/* Fill board with numbers */
bool board_fill(board* b);
/* Make n cells empty */
bool board_remove_cells(int ncells, board* b);

#endif
