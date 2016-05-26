#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <iostream>
#include "glmdecl.h"

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
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
        SDL_RenderClear(renderer);
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

    void drawFragment(float x, float y, float z, glm::vec3 color) {
        int X = (int)x, Y = (int)y;
        if (zbuffer[Y][X] > z) {
            zbuffer[(int)Y][(int)X] = z;
            color.x *= 255;
            color.y *= 255;
            color.z *= 255;
            SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255); 
            SDL_RenderDrawPoint(renderer, X, Y);
        }
    }

    void createZBuffer();
    void freeZBuffer();

    void flush() {
        SDL_RenderPresent(renderer);
    }

    private:
    void setSize(int w, int h) {
        width = w;
        height = h;
    }

};

#endif
