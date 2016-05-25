#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

class Display {
    int width;
    int height;
    SDL_Window * window;
    SDL_Renderer * renderer;
    public:
    Display(int w = 640, int h = 480);
    ~Display();

    SDL_Renderer * getRenderer() {
        return renderer;
    }

    SDL_Window * getWindow() {
        return window;
    }

    float getWidth() const {
        return width;
    }

    float getHeight() const {
        return height;
    }

    void setWindowSize(int w, int h) {
        width = w;
        height = h;
    }

};

#endif
