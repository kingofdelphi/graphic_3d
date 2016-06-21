#include "../include/display.h"

Display::Display() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw "error video";
    }
    int width = 640;
    int height = 480;
    window = SDL_CreateWindow("Glib", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!window) throw "cannot create window";
}

Display::~Display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::drawFragment(const Vertex & v) {
    float x = v.attrs[0].x, y = v.attrs[0].y;
    if (v.attrs.size() >= 2) {
        glm::vec4 color = v.attrs[1];
        int X = (int)(round(x)), Y = (int)(round(y));
        color.x *= 255;
        color.y *= 255;
        color.z *= 255;
        SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255); 
        SDL_RenderDrawPoint(renderer, X, Y);
    }
}
