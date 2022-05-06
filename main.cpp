#define SDL_MAIN_HANDLED

#include "NES/nes.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>

static const int width = 480, height = 360;

int main(int ArgCount, char **Args)
{
    auto nes = new Nes();
    nes->set_rom();
    nes->start();

    // SDL_Window *Window =
    //     SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    // SDL_GLContext Context = SDL_GL_CreateContext(Window);
    // for (int Running = 1; Running;) {
    //     SDL_Event Event;
    //     while (SDL_PollEvent(&Event)) {
    //         if (Event.type == SDL_QUIT)
    //             Running = 0;
    //     }
    //     glViewport(0, 0, width, height);
    //     SDL_GL_SwapWindow(Window);
    // }

    delete nes;
    return 0;
}