#include "glmdecl.h"
#include "display.h"
#include "container.h"
#include "cube.h"
#include "vshader.h"
#include "primitives.h"
#include <stdio.h>
using namespace glm;
using namespace std;

mat4x4 getPerspectiveMatrix() {
    const float l = -1, r = 1, t = 1, b = -1;
    const float n = 1.0, f = 100.0;
    mat4x4 pers = transpose(mat4x4(
                2 * n / (r - l), 0, (r + l) / (r - l), 0,
                0, 2 * n / (t - b), (t + b) / (t - b), 0,
                0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
                0, 0, -1, 0
                ));
    return pers;
}

void logic(Display & disp) {
    SDL_Event e;
    SDL_Renderer * r = disp.getRenderer();
    Cube cb(0, 0, -1.5, 1, 1, 1);
    VertexShader vshader;
    vshader.setTransformMatrix(getPerspectiveMatrix());
    Container cont;
    Light lght(
            glm::vec4(0, 0, 0, 1.0), 
            glm::vec3(1, 1, 1),
            glm::vec3(0, 0, -1) 
            );
    vshader.addLight(lght);
    cont.setVertexShader(vshader);
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
        //logic
        cb.update();
        //add objects to the container
        //clear to remove previously added objects
        cont.clear();
        cb.push(cont);
        //ready for rendering
        disp.clear();
        cont.render(disp);
    }
}

int main(int argc, char ** argv) {
    Display disp;
    logic(disp);
    return 0;
}
