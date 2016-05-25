#include "display.h"

Display::Display(int w, int h) : width(w), height(h) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw "error video";
    }
    window = SDL_CreateWindow("Glib", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!window) throw "cannot create window";
}

Display::~Display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}
