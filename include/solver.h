#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>
#include <board.h>

typedef struct
{
    board** boards;
    int capacity;
    int size;
} solutions;

typedef bool (*cb_func)(board* b, void* ctx);

solutions* solutions_create(void);
void solutions_destroy(solutions* sols);
bool solutions_add(solutions* sols, board* b);

solutions* board_solve(board* b, cb_func cb, void* ctx);
int board_nsolve(board* b);

#endif
