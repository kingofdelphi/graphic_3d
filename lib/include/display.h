#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <iostream>
#include "glmdecl.h"
#include "primitives.h"
#include "depthbuffer.h"

class Vertex; //forward declaration (to resolve cyclic header dependencies)

class Display {
    SDL_Window * window;
    SDL_Renderer * renderer;
    DBuffer * zbuffer;
    public:
    Display();
    ~Display();

    void clear() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    SDL_Renderer * getRenderer() {
        return renderer;
    }

    void setZBuffer(DBuffer * dbuf) {
        zbuffer = dbuf;
    }

    DBuffer * getZBuffer() {
        return zbuffer;
    }

    void drawFragment(const Vertex & v);
    
    void flush() {
        SDL_RenderPresent(renderer);
    }

    private:

};

#endif
