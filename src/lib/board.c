#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <board.h>

board* board_create(int size)
{
    board* b = malloc(sizeof(*b));
    if (!b)
    {
        fprintf(stderr, "Not enough memory!\n");
        return NULL;
    }

    b->cells = malloc(sizeof(*b->cells) * size);
    if (!b->cells)
    {
        fprintf(stderr, "Not enough memory!\n");
        free(b);
        return NULL;
    }
    for (int i = 0; i < size; i++)
    {
        b->cells[i] = malloc(size * sizeof(**b->cells));
        if (!b->cells)
        {
            fprintf(stderr, "Not enough memory!\n");
            for (int j = 0; j < i; j++)
                free(b->cells[j]);
            free(b->cells);
            free(b);
            return NULL;
        }
        for (int j = 0; j < size; j++)
        {
            b->cells[i][j].val = 0;
            b->cells[i][j].fixed = false;
        }
    }
    b->size = size;
    b->box_size = (int)sqrt(size);
    return b;
}

void board_destroy(board* b)
{
    for (int i = 0; i < b->size; i++)
        free(b->cells[i]);
    free(b->cells);
    free(b);
}

void board_print(board* b)
{
    for (int row = 0; row < b->size; row++)
    {
        for (int col = 0; col < b->size; col++)
        {
            printf("(%d, %d)", 
                    b->cells[row][col].val,
                    b->cells[row][col].fixed);
        }
        putchar('\n');
    }
    putchar('\n');
}

bool board_is_empty(board* b)
{
    for (int i = 0; i < b->size; i++)
    {
        for (int j = 0; j < b->size; j++)
            if (b->cells[i][j].val != 0)
                return false;
    }
    return true;
}

bool board_is_equal(board* a, board* b)
{
    if (a->size != b->size)
        return false;
    for (int i = 0; i < a->size; i++)
    {
        for (int j = 0; j < a->size; j++)
        {
            if (a->cells[i][j].val != b->cells[i][j].val)
                return false;
        }
    }
    return true;
}

static bool add_invalid_cell(position* invalid_cells, int* count, int row, int col)
{
    for (int i = 0; i < *count; i++)
    {
        position invalid_cell = invalid_cells[i];
        if (invalid_cell.row == row && invalid_cell.col == col)
            return false;
    }
    invalid_cells[*count].row = row;
    invalid_cells[*count].col = col;
    (*count)++;
    return true;
}

static bool is_valid(board* b, bool check_fixed, bool check_not_fixed,
                     position* invalid_cells, int* count,
                     int** rows, int** cols, int** boxes)
{
    bool ret = true;
    for (int row = 0; row < b->size; row++)
    {
        for (int col = 0; col < b->size; col++)
        {
            if (b->cells[row][col].val == 0)
                continue;
            if (!check_fixed || !check_not_fixed)
            {
                if (!check_fixed && b->cells[row][col].fixed)
                    continue;
                else if (!check_not_fixed && !b->cells[row][col].fixed)
                    continue;
            }
            int digit = b->cells[row][col].val;
            if (rows[row][digit] == 1)
            {
                add_invalid_cell(invalid_cells, count, row, col);
                ret = false;
            }
            rows[row][digit] = 1;

            if (cols[col][digit] == 1)
            {
                add_invalid_cell(invalid_cells, count, row, col);
                ret = false;
            }
            cols[col][digit] = 1;

            int box = (row / 3) * 3 + col / 3;
            if (boxes[box][digit] == 1)
            {
                add_invalid_cell(invalid_cells, count, row, col);
                ret = false;
            }
            boxes[box][digit] = 1;
        }
    }
    return ret;
}

bool board_is_valid(board* b, position* invalid_cells, int* count)
{
    *count = 0;
    int** rows = malloc(b->size * sizeof(*rows));
    int** cols = malloc(b->size * sizeof(*cols));
    int** boxes = malloc(b->size * sizeof(*boxes));
    if (!rows || !cols || !boxes)
    {
        fprintf(stderr, "Not enough memory!\n");
        free(rows);
        free(cols);
        free(boxes);
        return false;
    }
    for (int i = 0; i < b->size; i++)
    {
        rows[i] = malloc((b->size + 1) * sizeof(**rows));
        cols[i] = malloc((b->size + 1) * sizeof(**cols));
        boxes[i] = malloc((b->size + 1) * sizeof(**boxes));
        if (!rows[i] || !cols[i] || !boxes[i])
        {
            fprintf(stderr, "Not enough memory!\n");
            for (int j = 0; j <= i; j++)
            {
                free(rows[j]);
                free(cols[j]);
                free(boxes[j]);
            }
            return false;
        }
        memset(rows[i], 0, (b->size + 1) * sizeof(**rows));
        memset(cols[i], 0, (b->size + 1) * sizeof(**cols));
        memset(boxes[i], 0, (b->size + 1) * sizeof(**boxes));
    }

    // Check if there is errors in fixed numbers
    bool ret = is_valid(b, true, false, invalid_cells, count,
        rows, cols, boxes);
    if (ret)
        // Check if there is an error in not fixed numbers
        ret = is_valid(b, false, true, invalid_cells, count,
            rows, cols, boxes);

    for (int i = 0; i < b->size; i++)
    {
        free(rows[i]);
        free(cols[i]);
        free(boxes[i]);
    }
    free(rows);
    free(cols);
    free(boxes);
    return ret;
}

void board_clear(board* b)
{
    for (int i = 0; i < b->size; i++)
    {
        for (int j = 0; j < b->size; j++)
        {
            b->cells[i][j].val = 0;
            b->cells[i][j].fixed = 0;
        }
    }
}

bool board_set(board* dst, board* src)
{
    if (dst->size != src->size)
        return false;
    for (int i = 0; i < dst->size; i++)
    {
        for (int j = 0; j < dst->size; j++)
        {
            dst->cells[i][j] = src->cells[i][j];
        }
    }
    return true;
}

board* board_copy(board* b)
{
    board* bcopy = malloc(sizeof(*bcopy));
    if (!bcopy)
    {
        fprintf(stderr, "Not enough memory!\n");
        return NULL;
    }

    bcopy->cells = malloc(sizeof(*bcopy->cells) * b->size);
    if (!bcopy->cells)
    {
        fprintf(stderr, "Not enough memory!\n");
        free(bcopy);
        return NULL;
    }
    for (int i = 0; i < b->size; i++)
    {
        bcopy->cells[i] = malloc(sizeof(**bcopy->cells) * b->size);
        if (!bcopy->cells[i])
        {
            fprintf(stderr, "Not enough memory!\n");
            for (int j = 0; j < i; j++)
                free(bcopy->cells[j]);
            free(bcopy->cells);
            free(bcopy);
            return NULL;
        }
        memcpy(bcopy->cells[i], b->cells[i], 
            sizeof(**bcopy->cells) * b->size);
    }
    bcopy->size = b->size;
    bcopy->box_size = b->box_size;
    return bcopy;
}

bool board_place(board* b, int row, int col, int digit)
{
    if (row < 0 || col < 0 || row >= b->size || col >= b->size
        || b->cells[row][col].fixed == true)
        return false;
    b->cells[row][col].val = digit;
    return true;
}
