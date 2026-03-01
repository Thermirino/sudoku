#include <SDL.h>
#include <SDL_video.h>
#include <input.h>

event get_event(void)
{
    SDL_Event e;
    SDL_PollEvent(&e);

    event out = { .type = NO_EVENT };
    if (e.type == SDL_QUIT)
    {
        out.type = QUIT;
    }
    else if (e.type == SDL_WINDOWEVENT && 
             e.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        out.type = WINDOW_RESIZED;
        out.resize.window_width = e.window.data1;
        out.resize.window_height = e.window.data2;
    }
    else if (e.type == SDL_KEYDOWN)
    {
        out.type = KEYBOARD_BUTTON;
        switch (e.key.keysym.sym)
        {
        case SDLK_1:
        case SDLK_KP_1:
            out.keyboard.key = KEYBOARD_1;
            break;
        case SDLK_2:
        case SDLK_KP_2:
            out.keyboard.key = KEYBOARD_2;
            break;
        case SDLK_3:
        case SDLK_KP_3:
            out.keyboard.key = KEYBOARD_3;
            break;
        case SDLK_4:
        case SDLK_KP_4:
            out.keyboard.key = KEYBOARD_4;
            break;
        case SDLK_5:
        case SDLK_KP_5:
            out.keyboard.key = KEYBOARD_5;
            break;
        case SDLK_6:
        case SDLK_KP_6:
            out.keyboard.key = KEYBOARD_6;
            break;
        case SDLK_7:
        case SDLK_KP_7:
            out.keyboard.key = KEYBOARD_7;
            break;
        case SDLK_8:
        case SDLK_KP_8:
            out.keyboard.key = KEYBOARD_8;
            break;
        case SDLK_9:
        case SDLK_KP_9:
            out.keyboard.key = KEYBOARD_9;
            break;
        case SDLK_0:
        case SDLK_KP_0:
        case SDLK_BACKSPACE:
        case SDLK_DELETE:
            out.keyboard.key = KEYBOARD_0;
            break;
        case SDLK_o:
            out.keyboard.key = KEYBOARD_O;
            break;
        case SDLK_s:
            out.keyboard.key = KEYBOARD_S;
            break;
        case SDLK_EQUALS:
            out.keyboard.key = KEYBOARD_PLUS;
            break;
        case SDLK_MINUS:
            out.keyboard.key = KEYBOARD_MINUS;
            break;
        default:
            out.type = NO_EVENT;
        }
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        out.type = MOUSE_BUTTON;
        out.mouse.x = e.button.x;
        out.mouse.y = e.button.y;
    }
    else if (e.type == SDL_MOUSEMOTION)
    {
        out.type = MOUSE_MOTION;
        out.mouse.x = e.button.x;
        out.mouse.y = e.button.y;
    }
    return out;
}
