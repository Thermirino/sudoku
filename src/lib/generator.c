#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <board.h>
#include <solver.h>

#define swap(a, b) { int tmp = a; a = b; b = tmp; }

static void shuffle(int* ar, int size)
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(ar[i], ar[j]);
    }
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
    b->cells[row][col].fixed = true;
}

static void board_remove_number(board* b, int row, int col, int num,
                                int** rows, int** cols, int** boxes)
{
    int box = row / b->box_size * b->box_size + col / b->box_size;
    rows[row][num] = 0;
    cols[col][num] = 0;
    boxes[box][num] = 0;
    b->cells[row][col].val = 0;
    b->cells[row][col].fixed = false;
}

static bool fill(board* b, int row, int col,
                 int** rows, int** cols, int** boxes);

static bool board_place_next_number(board* b, int row, int col,
                                    int** rows, int** cols, int** boxes)
{
    if (row == b->size - 1 && col == b->size - 1)
        return true;
    else if (col == b->size - 1)
        return fill(b, row + 1, 0, rows, cols, boxes);
    else
        return fill(b, row, col + 1, rows, cols, boxes);
}

static bool fill(board* b, int row, int col,
                 int** rows, int** cols, int** boxes)
{
    int* nums = malloc(sizeof(*nums) * b->size);
    if (!nums)
    {
        fprintf(stderr, "Not enough memory!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < b->size; i++)
        nums[i] = i + 1;
    shuffle(nums, b->size);

    for (int i = 0; i < b->size; i++)
    {
        int num = nums[i];
        if (!could_place(b, row, col, num, rows, cols, boxes))
            continue;
        board_place_number(b, row, col, num,
                            rows, cols, boxes);
        if (board_place_next_number(b, row, col, rows, cols, boxes))
        {
            free(nums);
            return true;
        }
        board_remove_number(b, row, col, num,
                                rows, cols, boxes);
    }

    free(nums);
    return false;
}

bool board_fill(board* b)
{
    int** rows = malloc(sizeof(*rows) * b->size);
    int** cols = malloc(sizeof(*cols) * b->size);
    int** boxes = malloc(sizeof(*boxes) * b->size);
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
            return false;
        }
    }

    bool res =  fill(b, 0, 0, rows, cols, boxes);

    for (int i = 0; i < b->size; i++)
    {
        free(rows[i]);
        free(cols[i]);
        free(boxes[i]);
    }
    free(rows);
    free(cols);
    free(boxes);
    return res;
}

bool board_remove_cells(int ncells, board* b)
{
    int total_cells = b->size * b->size;
    if (ncells > total_cells) 
        ncells = total_cells;

    int* indices = malloc(total_cells * sizeof(int));
    if (!indices)
    {
        fprintf(stderr, "Not enough memory!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < total_cells; i++) 
        indices[i] = i;
    shuffle(indices, total_cells);

    int removed = 0;
    for (int i = 0; i < total_cells && removed < ncells; i++)
    {
        int row = indices[i] / b->size;
        int col = indices[i] % b->size;

        if (b->cells[row][col].val == 0)
            continue;

        int temp_num = b->cells[row][col].val;
        b->cells[row][col].val = 0;
        b->cells[row][col].fixed = false;

        if (board_nsolve(b) != 1)
        {
            b->cells[row][col].val = temp_num;
            b->cells[row][col].fixed = true;
        }
        else 
            removed++;
    }

    free(indices);
    return (removed == ncells);
}
