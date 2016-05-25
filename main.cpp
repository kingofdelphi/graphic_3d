#include "glmdecl.h"
#include "display.h"
#include "container.h"
#include "cube.h"
#include <stdio.h>
using namespace glm;
using namespace std;

void logic(Display & disp) {
    SDL_Event e;
    SDL_Renderer * r = disp.getRenderer();
    Cube cb(0, 0, -1.5, 1, 1, 1);
    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return ;
            else if(e.type == SDL_WINDOWEVENT) {
                if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    disp.resize(w, h);
                }
            } 
        }
        SDL_SetRenderDrawColor(r, 255, 0, 0, 0);
        disp.clear();
        SDL_RenderClear(r);
        Container cont;
        cb.update();
        cb.push(cont);
        cont.render(disp);
        SDL_RenderPresent(r);
    }
}

int main(int argc, char ** argv) {
    Display disp;
    logic(disp);
    return 0;
}
