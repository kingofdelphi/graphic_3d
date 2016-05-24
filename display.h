#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

class Display {
    const int width = 640;
    const int height = 480;
    SDL_Window * window;
    SDL_Renderer * renderer;
    public:
    Display();
    ~Display();

    SDL_Renderer * getRenderer() {
        return renderer;
    }

    SDL_Window * getWindow() {
        return window;
    }

};

#endif
