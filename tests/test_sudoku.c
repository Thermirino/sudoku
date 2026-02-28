#include <unity.h>
#include <board.h>
#include <sudoku.h>
#include <draw.h>
#include <solver.h>

#if defined(_MSC_VER) && defined(_CRTDBG_MAP_ALLOC)
#include <stdlib.h>
#include <crtdbg.h>
#endif

void test_sudoku(void)
{
	int window_width = 1024;
	int window_height = 512;
	int board_size = 9;
	game_state* gs = game_init(window_width, window_height, board_size, ONE_MOVE);
	game_start(gs);
	game_end(gs);
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
	RUN_TEST(test_sudoku);
	return UNITY_END();
}
