#include "glmdecl.h"
#include "display.h"
#include "container.h"
#include "cube.h"
#include "vshader.h"
#include "fragmentshader.h"
#include "primitives.h"
#include "objloader.h"
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

class GouraudVertexShader : public VertexShader {
    public:
        Vertex transform(const Vertex & v) {
            Vertex r(v);
            r.pos = tmat * mmodel * r.pos;
            r.normal = vec3(mnormal * vec4(r.normal, 0));
            //lighting calculations
            vec4 color(0, 0, 0, 1.0);
            for (auto & i : lights) {
                float f = dot(-i.normal, r.normal);
                if (f < 0) f = 0;
                float Cd = 0.6;
                vec4 scale = i.color * Cd * f;
                color += scale;
            }
            r.color *= color;
            r.color.x = std::min(std::max(r.color.x, 0.f), 1.f);
            r.color.y = std::min(std::max(r.color.y, 0.f), 1.f);
            r.color.z = std::min(std::max(r.color.z, 0.f), 1.f);
            return r;
        }
};

class PassThroughFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            return fragment;
        }
};

class PhongFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            Vertex r(fragment);
            //lighting calculations
            glm::vec4 color(0, 0, 0, 1.0);
            for (auto & i : lights) {
                float f = dot(-i.normal, r.normal);
                if (f < 0) f = 0;
                float Cd = 0.6;
                vec4 scale = i.color * Cd * f;
                color += scale;
            }
            r.color *= color;
            r.color.x = std::min(std::max(r.color.x, 0.f), 1.f);
            r.color.y = std::min(std::max(r.color.y, 0.f), 1.f);
            r.color.z = std::min(std::max(r.color.z, 0.f), 1.f);
            return r;
        }
};

class MyVertexShader : public VertexShader {
    public:
        Vertex transform(const Vertex & v) {
            Vertex r(v);
            r.pos = tmat * mmodel * r.pos;
            return r;
        }
};

void logic(Display & disp) {
    SDL_Event e;
    Cube cb(0, 0, -1.5, 1, 1, 1);
    VertexShader * vshader = new GouraudVertexShader();
    //VertexShader * vshader = new MyVertexShader();
    vshader->setTransformMatrix(getPerspectiveMatrix());
    //FragmentShader * fshader = new PhongFragmentShader();
    FragmentShader * fshader = new PassThroughFragmentShader();
    Container cont;
    cont.setDisplay(&disp);
    Light lght(
            glm::vec4(0, 0, 0, 1.0), 
            glm::vec4(1, 1, 1, 1.0),
            glm::vec3(0, 0, -1) 
            );
    vshader->addLight(lght);
    cont.setVertexShader(vshader);
    cont.setFragmentShader(fshader);
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
        //rendering
        disp.clear();
        //add objects to the container
        cb.push(cont);
        cont.flush(); //execute requests
        //Helper::drawObj(cont, "suzanne.obj");
        //ready for rendering
        disp.flush(); //refresh the screen
    }

    delete vshader;
    delete fshader;
}

int main(int argc, char ** argv) {
    Display disp;
    logic(disp);
    return 0;
}
