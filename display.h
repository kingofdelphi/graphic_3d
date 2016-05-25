#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "color.h"
#include <iostream>

class Display {
    int width;
    int height;
    SDL_Window * window;
    SDL_Renderer * renderer;
    float ** zbuffer;
    public:
    Display(int w = 640, int h = 480);
    ~Display();

    void clear() {
        if (zbuffer) {
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    zbuffer[i][j] = 1e100;
                }
            }
        }
    }

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
    void resize(int w, int h) {
        if (zbuffer) freeZBuffer();
        setSize(w, h);
        createZBuffer();
    }

    void drawFragment(float x, float y, float z, Color col) {
        int X = (int)x, Y = (int)y;
        if (zbuffer[Y][X] > z) {
            zbuffer[(int)Y][(int)X] = z;
            SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 0); 
            SDL_RenderDrawPoint(renderer, X, Y);
        }
    }

    void createZBuffer();
    void freeZBuffer();

    private:
    void setSize(int w, int h) {
        width = w;
        height = h;
    }

};

#endif
