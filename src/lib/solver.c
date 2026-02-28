#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <solver.h>

#define nelements(ar) (sizeof(ar) / sizeof(*ar))
#define swap(a, b) { int tmp = a; a = b; b = tmp; }

solutions* solutions_create(void)
{
    solutions* sols = malloc(sizeof(*sols));
    if (!sols)
    {
        fprintf(stderr, "Not enough memory!\n");
        return NULL;
    }
    sols->boards = NULL;
    sols->capacity = 0;
    sols->size = 0;
    return sols;
}

void solutions_destroy(solutions* sols)
{
    if (sols->capacity > 0)
    {
        for (int i = 0; i < sols->size; i++)
            board_destroy(sols->boards[i]);
    }
    free(sols->boards);
    free(sols);
}

bool solutions_add(solutions* sols, board* b)
{
    if (sols->capacity == sols->size)
    {
        int new_capacity = sols->capacity == 0 ? 10 : sols->capacity * 2;
        board** t = realloc(sols->boards, sizeof(*t) * new_capacity);
        if (!t)
        {
            fprintf(stderr, "Not enough memory!");
            return false;
        }
        sols->boards = t;
        sols->capacity = new_capacity;
    }
    sols->boards[sols->size++] = board_copy(b);
    return true;
}

static bool could_place(board* b, int row, int col, int num,
                    int** rows, int** cols, int** boxes)
{
    int box = row / b->box_size * b->box_size + col / b->box_size;
    return rows[row][num] + cols[col][num] + boxes[box][num] == 0;
}

static void board_place_number(board* b, int row, int col, int num,
                                int** rows, int** cols, int** boxes)
{
    int box = row / b->box_size * b->box_size + col / b->box_size;
    rows[row][num] = 1;
    cols[col][num] = 1;
    boxes[box][num] = 1;
    b->cells[row][col].val = num;
}

static void board_remove_number(board* b, int row, int col, int num,
                                int** rows, int** cols, int** boxes)
{
    int box = row / b->box_size * b->box_size + col / b->box_size;
    rows[row][num] = 0;
    cols[col][num] = 0;
    boxes[box][num] = 0;
    b->cells[row][col].val = 0;
}

static void solve(solutions* sols, board* b, int row, int col,
                    int** rows, int** cols, int** boxes,
                    cb_func cb, void* ctx);

static void board_place_next_number(solutions* sols, board* b, int row, int col,
                                    int** rows, int** cols, int** boxes,
                                    cb_func cb, void* ctx)
{
    if (row == b->size - 1 && col == b->size - 1)
    {
        solutions_add(sols, b);
    }
    else if (col == b->size - 1)
        solve(sols, b, row + 1, 0, rows, cols, boxes, cb, ctx);
    else
        solve(sols, b, row, col + 1, rows, cols, boxes, cb, ctx);
}

static bool stopped;

static void solve(solutions* sols, board* b, int row, int col,
                    int** rows, int** cols, int** boxes,
                    cb_func cb, void* ctx)
{
    if (b->cells[row][col].val == 0)
    {

        for (int num = 1; num <= b->size; num++)
        {
            if (stopped)
                return;
            if (!could_place(b, row, col, num, rows, cols, boxes))
                continue;
            board_place_number(b, row, col, num,
                                rows, cols, boxes);

            // Call callback function (to update board in the screen)
            if (cb != NULL)
                if (cb(b, ctx) == false && sols->size == 0)
                {
                    // Interrupt the solver if the user requests it
                    board_remove_number(b, row, col, num,
                                        rows, cols, boxes);
                    stopped = true;
                    return;
                }

            board_place_next_number(sols, b, row, col,
                                    rows, cols, boxes,
                                    cb, ctx);

            // For autosolver dont remove numbers from the field when
            // the solution is completed
            if (cb != NULL && sols->size != 0)
                continue;

            board_remove_number(b, row, col, num,
                                rows, cols, boxes);
        }
    }
    else
    {
        board_place_next_number(sols, b, row, col,
                                rows, cols, boxes,
                                cb, ctx);
    }
}

solutions* board_solve(board* b, cb_func cb, void* ctx)
{
    stopped = false;
    int** rows = malloc(sizeof(*rows) * b->size);
    int** cols = malloc(sizeof(*cols) * b->size);
    int** boxes = malloc(sizeof(*boxes) * b->size);
    if (!rows || !cols || !boxes)
    {
        fprintf(stderr, "Not enough memory!\n");
        free(rows);
        free(cols);
        free(boxes);
        return NULL;
    }

    for (int i = 0; i < b->size; i++)
    {
        int max_num = b->size;
        rows[i] = calloc((max_num + 1), sizeof(**rows));
        cols[i] = calloc((max_num + 1), sizeof(**cols));
        boxes[i] = calloc((max_num + 1), sizeof(**boxes));

        if (!rows[i] || !cols[i] || !boxes[i])
        {
            fprintf(stderr, "Not enough memory!\n");
            for (int j = 0; j < i; j++)
            {
                free(rows[j]);
                free(cols[j]);
                free(boxes[j]);
            }
            free(rows);
            free(cols);
            free(boxes);
            return NULL;
        }
    }

    for (int row = 0; row < b->size; row++)
    {
        for (int col = 0; col < b->size; col++)
        {
            int num = b->cells[row][col].val;
            if (num != 0)
                board_place_number(b, row, col, num, rows, cols, boxes);
        }
    }

    solutions* sols = solutions_create();
    solve(sols, b, 0, 0, rows, cols, boxes, cb, ctx);

    for (int i = 0; i < b->size; i++)
    {
        free(rows[i]);
        free(cols[i]);
        free(boxes[i]);
    }
    free(rows);
    free(cols);
    free(boxes);
    return sols;
}

int board_nsolve(board* b)
{
    solutions* s = board_solve(b, NULL, NULL);
    if (s == NULL)
        return 0;
    int n = s->size;
    solutions_destroy(s);
    return n;
}
