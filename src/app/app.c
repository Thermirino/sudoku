#include <sudoku.h>

#if defined(_MSC_VER) && defined(_CRTDBG_MAP_ALLOC)
#include <crtdbg.h>
#endif

int main(void)
{   
#if defined(_MSC_VER) && defined(_CRTDBG_MAP_ALLOC)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    int window_width = 1024;
    int window_height = 512;
    int board_size = 9;
    game_state* gs = game_init(window_width, window_height, board_size, ONE_MOVE);
    if (!gs)
        return -1;
    game_start(gs);
    game_end(gs);
}
