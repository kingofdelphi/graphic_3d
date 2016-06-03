#include "display.h"

Display::Display(int w, int h) : width(w), height(h), zbuffer(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw "error video";
    }
    window = SDL_CreateWindow("Glib", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!window) throw "cannot create window";

    createZBuffer();
}

Display::~Display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::createZBuffer() {
    zbuffer = new float * [height];
    for (int i = 0; i < height; ++i) {
        zbuffer[i] = new float[width];
    }
}

void Display::freeZBuffer() {
    for (int i = 0; i < height; ++i) {
        delete [] zbuffer[i];
    }
    delete [] zbuffer;
    zbuffer = nullptr;
}

void Display::drawFragment(const Vertex & v) {
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
    float x = v["position"].x, y = v["position"].y, z = v["position"].z;
    glm::vec4 color = v["color"];
    //color.x = MIN(MAX(0, color.x), 1.0);
    //color.y = MIN(MAX(0, color.y), 1.0);
    //color.z = MIN(MAX(0, color.z), 1.0);
    int X = (int)(round(x)), Y = (int)(round(y));
    if (0 < X && X < width && 0 < Y && Y < height && z < zbuffer[Y][X]) {
        zbuffer[Y][X] = z;
        color.x *= 255;
        color.y *= 255;
        color.z *= 255;
        SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255); 
        SDL_RenderDrawPoint(renderer, X, Y);
    }
}
