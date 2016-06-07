#include "../include/display.h"

Display::Display() : zbuffer(nullptr) {
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
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
    float x = v.attrs[0].x, y = v.attrs[0].y, z = v.attrs[0].z;
    glm::vec4 color = v.attrs.size() < 2 ? glm::vec4(1, 0, 0, 1) : v.attrs[1];
    //color.x = MIN(MAX(0, color.x), 1.0);
    //color.y = MIN(MAX(0, color.y), 1.0);
    //color.z = MIN(MAX(0, color.z), 1.0);
    int X = (int)(round(x)), Y = (int)(round(y));
    if (zbuffer->inbounds(X, Y) && z + .00001 < zbuffer->buffer[Y][X]) {
        zbuffer->buffer[Y][X] = z;
        color.x *= 255;
        color.y *= 255;
        color.z *= 255;
        SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255); 
        SDL_RenderDrawPoint(renderer, X, Y);
    }
}
