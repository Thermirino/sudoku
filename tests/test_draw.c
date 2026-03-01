#include <stdlib.h>
#include <unity.h>
#include <board.h>
#include <draw.h>
#include <input.h>
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

void test_board_draw(void)
{
	int board_size = 9;
	int window_width = 1024;
	int window_height = 512;
	int min_screen_side_size = window_width < window_height ?
		window_width : window_height;
	int padding_size = (int)(min_screen_side_size * 0.1);
	int board_side_size = (min_screen_side_size - padding_size * 2) / board_size * board_size;
	int board_x = (window_width - board_side_size) / 2;
	int board_y = (window_height - board_side_size) / 2;

	int dlvls_h = window_height / 5;
	int dlvls_w = window_width / 5;
	int dlvls_y = window_height / 2 - dlvls_h / 2;
	int dlvls_x = board_x / 2 - dlvls_w / 2;

	int time_y = padding_size / 2;
	int time_x = window_width / 2;
	TEST_ASSERT_TRUE(draw_init(window_width, window_height));

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

	// int one_solution2[9][9] = {
	// 	{5, 3, 0, 0, 7, 0, 0, 0, 0},
	// 	{6, 0, 0, 1, 9, 5, 0, 0, 0},
	// 	{0, 9, 8, 0, 0, 0, 0, 6, 0},
	// 	{8, 0, 0, 0, 6, 0, 0, 0, 3},
	// 	{4, 0, 0, 8, 0, 3, 0, 0, 1},
	// 	{7, 0, 0, 0, 2, 0, 0, 0, 6},
	// 	{0, 6, 0, 0, 0, 0, 2, 8, 0},
	// 	{0, 0, 0, 4, 1, 9, 0, 0, 5},
	// 	{0, 0, 0, 0, 8, 0, 0, 7, 9}
	// };

	// int two_solutions[9][9] = {
	// 	{9, 0, 6, 0, 7, 0, 4, 0, 3},
	// 	{0, 0, 0, 4, 0, 0, 2, 0, 0},
	// 	{0, 7, 0, 0, 2, 3, 0, 1, 0},
	// 	{5, 0, 0, 0, 0, 0, 1, 0, 0},
	// 	{0, 4, 0, 2, 0, 8, 0, 6, 0},
	// 	{0, 0, 3, 0, 0, 0, 0, 0, 5},
	// 	{0, 3, 0, 7, 0, 0, 0, 5, 0},
	// 	{0, 0, 7, 0, 0, 5, 0, 0, 0},
	// 	{4, 0, 5, 0, 1, 0, 7, 0, 8}
	// };

	time_t start_time = time(NULL);
	board* b = board_create(board_size);
	board_set_9x9(b, one_solution);
	draw_clear();
	draw_time(time_y, time_x, start_time, start_time);
	draw_difficulty_levels_highlight(dlvls_y, dlvls_x, dlvls_w, dlvls_h, 0);
	draw_difficulty_levels(dlvls_y, dlvls_x, dlvls_w, dlvls_h);
	draw_board(b, board_y, board_x, board_side_size);
	draw_board_higlight_row(b, board_y, board_x, board_side_size, 0, 
		rand() % 256, rand() % 256, rand() % 256, 50);
	draw_board_higlight_col(b, board_y, board_x, board_side_size, 0, 
		rand() % 256, rand() % 256, rand() % 256, 50);
	draw_board_higlight_box(b, board_y, board_x, board_side_size, 0, 
		rand() % 256, rand() % 256, rand() % 256, 50);
	draw_present();
	event e;
	while ((e = get_event()).type != QUIT)
		;
	board_destroy(b);

	int board_3x3[3][3] = {
        { 1,2,3 },
        { 4,5,6 },
        { 7,8,9 }
	};
	board_size = 3;
	b = board_create(board_size);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			b->cells[i][j].val = board_3x3[i][j];
			if (board_3x3[i][j] != 0)
				b->cells[i][j].fixed = 1;
			else
				b->cells[i][j].fixed = 0;
		}
	draw_clear();
	draw_time(time_y, time_x, start_time, start_time);
	draw_difficulty_levels_highlight(dlvls_y, dlvls_x, dlvls_w, dlvls_h, 0);
	draw_difficulty_levels(dlvls_y, dlvls_x, dlvls_w, dlvls_h);
	draw_board(b, board_y, board_x, board_side_size);
	draw_board_higlight_row(b, board_y, board_x, board_side_size, 0, 
		rand() % 256, rand() % 256, rand() % 256, 50);
	draw_board_higlight_col(b, board_y, board_x, board_side_size, 0, 
		rand() % 256, rand() % 256, rand() % 256, 50);
	draw_board_higlight_box(b, board_y, board_x, board_side_size, 0, 
		rand() % 256, rand() % 256, rand() % 256, 50);
	draw_present();
	while ((e = get_event()).type != QUIT)
		;
	board_destroy(b);

	draw_end();
}

void test_draw_difficult_levels(void)
{
	int window_width = 1024;
	int window_height = 512;
	TEST_ASSERT_TRUE(draw_init(window_width, window_height));

	int y = 100, x = 400, w = 200, h = 200;
	draw_difficulty_levels_highlight(y, x, w, h, 1);
	draw_difficulty_levels(y, x, w, h);

	draw_present();
	event e;
	while ((e = get_event()).type != QUIT)
		;

	draw_end();
}

void test_draw_time(void)
{
	int window_width = 1024;
	int window_height = 512;
	TEST_ASSERT_TRUE(draw_init(window_width, window_height));

	int y = window_height / 2, x = window_width / 2;
	time_t start_time = time(NULL);

	event e;
	while ((e = get_event()).type != QUIT)
	{
		draw_clear();
		draw_time(y, x, start_time, time(NULL));
		draw_present();
	}

	draw_end();
}

void test_draw_solved(void)
{
	int window_width = 1024;
	int window_height = 512;
	TEST_ASSERT_TRUE(draw_init(window_width, window_height));

	int solved_popup_y = window_height / 3, solved_popup_x = window_width / 3;
	int solved_popup_w = window_width / 2;
	int solved_popup_h = window_height / 3;
    int try_again_btn_w = solved_popup_w / 3;
    int try_again_btn_h = solved_popup_h / 3;
    int try_again_btn_x = solved_popup_x + (solved_popup_w - try_again_btn_w) / 2;
    int try_again_btn_y = solved_popup_y + solved_popup_h - 
                          (solved_popup_h - try_again_btn_h) * 2 / 3;
	time_t start_time = time(NULL);
	draw_solved_popup(solved_popup_y, solved_popup_x, solved_popup_w, solved_popup_h,
		              try_again_btn_y, try_again_btn_x, try_again_btn_w, try_again_btn_h,
		              start_time, start_time);

	event e;
	while ((e = get_event()).type != QUIT)
	{
		draw_present();
	}

	draw_end();
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
	RUN_TEST(test_board_draw);
	RUN_TEST(test_draw_difficult_levels);
	RUN_TEST(test_draw_time);
	RUN_TEST(test_draw_solved);
	return UNITY_END();
}
