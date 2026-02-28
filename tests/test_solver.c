#include <unity.h>
#include <board.h>
#include <solver.h>

#if defined(_MSC_VER) && defined(_CRTDBG_MAP_ALLOC)
#include <stdlib.h>
#include <crtdbg.h>
#endif

static void board_set_9x9(board* b, int ar[9][9])
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			b->cells[i][j].val = ar[i][j];
			if (ar[i][j] == 0)
				b->cells[i][j].fixed = 0;
			else
				b->cells[i][j].fixed = 1;
		}
	}
}


void test_board(void)
{
	int size = 9;
	position invalid_cells[9];
	int count;
	board* b = board_create(size);
	TEST_ASSERT_TRUE(board_is_empty(b));
	TEST_ASSERT_TRUE(board_fill(b));
	TEST_ASSERT_FALSE(board_is_empty(b));
	TEST_ASSERT_TRUE(board_is_valid(b, invalid_cells, &count));
	TEST_ASSERT_EQUAL(1, board_nsolve(b));

	int board_9x9[9][9] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
	};

	// Error in fixed cell
	count = 0;
	board_set_9x9(b, board_9x9);
	b->cells[0][0].val = 1;
	b->cells[0][1].val = 1;
	b->cells[0][0].fixed = true;
	b->cells[0][1].fixed = true;
	TEST_ASSERT_FALSE(board_is_valid(b, invalid_cells, &count));
	TEST_ASSERT_TRUE(count == 1);
	TEST_ASSERT_TRUE(invalid_cells[0].row == 0 && invalid_cells[0].col == 1);

	// Error in not fixed cell
	count = 0;
	board_set_9x9(b, board_9x9);
	b->cells[0][0].val = 1;
	b->cells[0][1].val = 1;
	b->cells[0][0].fixed = false;
	b->cells[0][1].fixed = true;
	TEST_ASSERT_FALSE(board_is_valid(b, invalid_cells, &count));
	TEST_ASSERT_TRUE(count == 1);
	TEST_ASSERT_TRUE(invalid_cells[0].row == 0 && invalid_cells[0].col == 0);

	// Error in not fixed cell
	count = 0;
	board_set_9x9(b, board_9x9);
	b->cells[0][0].val = 1;
	b->cells[0][1].val = 1;
	b->cells[0][0].fixed = true;
	b->cells[0][1].fixed = false;
	TEST_ASSERT_FALSE(board_is_valid(b, invalid_cells, &count));
	TEST_ASSERT_TRUE(count == 1);
	TEST_ASSERT_TRUE(invalid_cells[0].row == 0 && invalid_cells[0].col == 1);

	// Error in multiple not fixed cells (0,0), (0,2)
	count = 0;
	board_set_9x9(b, board_9x9);
	b->cells[0][0].val = 1;
	b->cells[0][1].val = 1;
	b->cells[0][2].val = 1;
	b->cells[0][0].fixed = false;
	b->cells[0][1].fixed = true;
	b->cells[0][2].fixed = false;
	TEST_ASSERT_FALSE(board_is_valid(b, invalid_cells, &count));
	TEST_ASSERT_TRUE(count == 2);
	TEST_ASSERT_TRUE(invalid_cells[0].row == 0 && invalid_cells[0].col == 0);
	TEST_ASSERT_TRUE(invalid_cells[1].row == 0 && invalid_cells[1].col == 2);

	board_destroy(b);
}

void test_generator(void)
{
	int size = 9;
	board* b = board_create(size);
	TEST_ASSERT_TRUE(board_is_empty(b));

	for (int i = 1; i < 10; i++)
	{
		TEST_ASSERT_TRUE(board_fill(b));
		TEST_ASSERT_TRUE(board_remove_cells(i, b));
		// board_print(b);
		TEST_ASSERT_EQUAL(1, board_nsolve(b));
	}

	board_destroy(b);
}

void test_solver(void)
{
	int one_solution[9][9] = {
		{5, 3, 0, 0, 7, 0, 0, 0, 0},
		{6, 0, 0, 1, 9, 5, 0, 0, 0},
		{0, 9, 8, 0, 0, 0, 0, 6, 0},
		{8, 0, 0, 0, 6, 0, 0, 0, 3},
		{4, 0, 0, 8, 0, 3, 0, 0, 1},
		{7, 0, 0, 0, 2, 0, 0, 0, 6},
		{0, 6, 0, 0, 0, 0, 2, 8, 0},
		{0, 0, 0, 4, 1, 9, 0, 0, 5},
		{0, 0, 0, 0, 8, 0, 0, 7, 9}
	};
	int one_solution2[9][9] = {
		{5, 3, 0, 0, 7, 0, 0, 0, 0},
		{6, 0, 0, 1, 9, 5, 0, 0, 0},
		{0, 9, 8, 0, 0, 0, 0, 6, 0},
		{8, 0, 0, 0, 6, 0, 0, 0, 3},
		{4, 0, 0, 8, 0, 3, 0, 0, 1},
		{7, 0, 0, 0, 2, 0, 0, 0, 6},
		{0, 6, 0, 0, 0, 0, 2, 8, 0},
		{0, 0, 0, 4, 1, 9, 0, 0, 5},
		{0, 0, 0, 0, 8, 0, 0, 7, 9}
	};

	int two_solutions[9][9] = {
		{9, 0, 6, 0, 7, 0, 4, 0, 3},
		{0, 0, 0, 4, 0, 0, 2, 0, 0},
		{0, 7, 0, 0, 2, 3, 0, 1, 0},
		{5, 0, 0, 0, 0, 0, 1, 0, 0},
		{0, 4, 0, 2, 0, 8, 0, 6, 0},
		{0, 0, 3, 0, 0, 0, 0, 0, 5},
		{0, 3, 0, 7, 0, 0, 0, 5, 0},
		{0, 0, 7, 0, 0, 5, 0, 0, 0},
		{4, 0, 5, 0, 1, 0, 7, 0, 8}
	};

	int size = 9;
	board* b = board_create(size);
	board_set_9x9(b, one_solution);
	TEST_ASSERT_EQUAL(1, board_nsolve(b));
	board_set_9x9(b, one_solution2);
	TEST_ASSERT_EQUAL(1, board_nsolve(b));
	board_set_9x9(b, two_solutions);
	TEST_ASSERT_EQUAL(2, board_nsolve(b));
	board_destroy(b);
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char** argv)
{
#if defined(_MSC_VER) && defined(_CRTDBG_MAP_ALLOC)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	(void)argc;
	(void)argv;
	UNITY_BEGIN();
	RUN_TEST(test_board);
	RUN_TEST(test_generator);
	RUN_TEST(test_solver);
	return UNITY_END();
}
