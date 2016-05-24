#include "display.h"
#include "container.h"
#include <glm/vec3.hpp>

void logic(Display & disp) {
    SDL_Event e;
    SDL_Renderer * r = disp.getRenderer();
    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return ;
        }
        SDL_SetRenderDrawColor(r, 255, 0, 0, 0);
        SDL_RenderClear(r);
        Container cont;
        cont.addLine(Line(glm::vec3(0, 0, 0), glm::vec3(100, 100, 0)));
        SDL_SetRenderDrawColor(r, 0, 255, 0, 0);
        cont.render(disp);
        SDL_RenderPresent(r);
    }
}

int main(int argc, char ** argv) {
    Display disp;
    logic(disp);
    return 0;
}
