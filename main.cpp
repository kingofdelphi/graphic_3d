#include "glmdecl.h"
#include "display.h"
#include "container.h"
#include "cube.h"
#include "vshader.h"
#include "fragmentshader.h"
#include "shaderprogram.h"
#include "primitives.h"
#include "objloader.h"
#include "camera.h"
#include <stdio.h>
#include <vector>
#include <tuple>

using namespace glm;
using namespace std;

Camera camera, light_cam;

mat4x4 getPerspectiveMatrix() {
    const float l = -1, r = 1, t = 1, b = -1;
    const float n = 1.0, f = 100.0;
    mat4x4 pers = transpose(mat4x4(
                2 * n / (r - l), 0, (r + l) / (r - l), 0,
                0, 2 * n / (t - b), (t + b) / (t - b), 0,
                0, 0, (f + n) / (f - n), -2 * f * n / (f - n),
                0, 0, -1, 0
                ));
    return pers;
}

class ShadowVertexShader : public VertexShader {
    public:
        Vertex transform(const Vertex & v) {
            Vertex r(v);
            auto & unfms = program->uniforms_mat4;
            r.pos = unfms["mview"] * unfms["mmodel"] * r.pos;
            r.pos = unfms["pers"] * r.pos;
            //r.normal = vec3(unfms["mnormal"] * vec4(r.normal, 0));
            return r;
        }
};

class GouraudVertexShader : public VertexShader {
    public:
        Vertex transform(const Vertex & v) {
            Vertex r(v);
            auto & unfms = program->uniforms_mat4;
            r.pos = unfms["mmodel"] * r.pos;
            r.old_pos = unfms["world_to_light"] * r.pos;//world space coords to light space homogeneous coords
            r.pos = unfms["mview"] * r.pos;
            r.pos = unfms["pers"] * r.pos;
            r.normal = vec3(unfms["mnormal"] * vec4(r.normal, 0));
            return r;
        }
};

class PassThroughFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            Vertex t(fragment);
            return t;
        }
};

float **zbuffer = nullptr;
int width, height;

class PhongFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            Vertex r(fragment);
            vec4 pos = r.old_pos;
            pos /= pos.w;
            pos.x = width * (1 + pos.x) * 0.5;
            pos.y = height * (1 - pos.y) * 0.5;
            float notshadow = 0;
            int X = int(pos.x);
            int Y = int(pos.y);
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int AX = X + i, AY = Y + j;
                    if (0 <= AX && AX < width && 0 <= AY && AY < height) {
                        if (pos.z - 0.018398 < zbuffer[AY][AX]) notshadow += 1;
                    }
                }
            }
            notshadow /= 9.0;
            glm::vec4 color(0.5, 0.5, 0.5, 1.0);
            r.normal = normalize(r.normal);
            //lighting calculations
            for (auto & i : program->lights) {
                float f = dot(-i.normal, r.normal);
                //f = 0.3;
                if (f < 0) f = 0;
                float Cd = 0.5;
                vec4 scale = i.color * Cd * f;
                color += notshadow * scale;
            }
            r.color *= color;
            r.color.x = std::min(std::max(r.color.x, 0.f), 1.f);
            r.color.y = std::min(std::max(r.color.y, 0.f), 1.f);
            r.color.z = std::min(std::max(r.color.z, 0.f), 1.f);
            return r;
        }
};

void resize(int w, int h) {
    if (zbuffer) {
        for (int i = 0; i < height; ++i) delete [] zbuffer[i];
        delete [] zbuffer;
        zbuffer = nullptr;
    }
    width = w;
    height = h;
    zbuffer = new float * [height];
    for (int i = 0; i < height; ++i) {
        zbuffer[i] = new float[width];
    }
}

void copyZBuffer(Container & cont) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            zbuffer[i][j] = cont.display->getZBuffer()[i][j];
        }
    }
}

//Cube cb(-1, 0, -2.5, 1, 1, 1);
Cube cb2(0, -.5/2, -2, .5, .5, .5);
Object obj;
void renderScene(Container & cont) {
    //add objects to the container
    if (cont.program == nullptr) throw "no vertex shader";
    auto & unfms = cont.program->uniforms_mat4;
    unfms["mmodel"] = glm::mat4x4(1.0);
    unfms["mnormal"] = glm::mat4x4(1.0);
    float gap = 1;
    float span = 10;
    float steps = 10;
    for (int i = -10; i <= 10; ++i) {
        glm::vec3 normal(0, 0, 1);
        glm::vec4 a(i * gap, -.5, span, 1);
        glm::vec4 b(i * gap, -.5, -span, 1);
        glm::vec4 acolor(1, 0, 0, 1);
        glm::vec4 bcolor(1, 1, 1, 1);
        Line l(Vertex(a, acolor, normal),
                Vertex(b, bcolor, normal)
              );
        //cont.addLine(l);
    }

    for (int i = -10; i <= 10; ++i) {
        glm::vec3 normal(1, 0, 0);
        glm::vec4 a(-span, -.5, i * gap, 1);
        glm::vec4 b(span, -.5, i * gap, 1);
        glm::vec4 acolor(0, 1, 0, 1);
        glm::vec4 bcolor(1, 0, 1, 1);
        Line l(Vertex(a, acolor, normal),
                Vertex(b, bcolor, normal)
              );
        //cont.addLine(l);
    }

    glm::vec3 normal(0, 0, 1);

    vector<vec4> colors = {
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 1, 1),
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 0, 1, 1),
    };
    
    vector<vec3> normals = {
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
    };

    vector<vec4> vertices = {
        glm::vec4(-2, -.5, -1, 1),
        glm::vec4(-2, -.5, -4, 1),
        glm::vec4(2, -.5, -4, 1),
        glm::vec4(2, -.5, -1, 1),
    };

    cont.program->attribPointer("position", &vertices);
    cont.program->attribPointer("color", &colors);
    cont.program->normalAttribPointer(&normals);

    cont.addMesh(make_tuple(0, 1, 2));
    cont.addMesh(make_tuple(2, 3, 0));

    cont.flush(); //execute requests
    obj.draw(cont);
    //cb.push(cont);
    //cb2.push(cont);
    cont.flush(); //execute requests
}

void logic(Display & disp) {
    SDL_Event e;
    VertexShader * vshader = new GouraudVertexShader();
    VertexShader * shadow_vshader = new ShadowVertexShader();
    FragmentShader * fshader = new PhongFragmentShader();
    FragmentShader * shadow_fshader = new PassThroughFragmentShader();
    ShaderProgram scene_program;
    scene_program.init(vshader, fshader);
    glm::mat4x4 pers = getPerspectiveMatrix();
    scene_program.uniforms_mat4["pers"] = pers;
    ShaderProgram shadow_program;
    shadow_program.init(shadow_vshader, shadow_fshader);
    shadow_program.uniforms_mat4["pers"] = pers;
    Container cont;
    cont.setDisplay(&disp);
    obj.load("suzanne.obj");
    float ang = 0;//3.1415/4;//3.1415 * .5;
    bool vsw = false;
    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return ;
            else if(e.type == SDL_WINDOWEVENT) {
                if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    disp.resize(w, h);
                    resize(w, h);
                }
            }  else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    vsw ^= 1;
                }
            }
        }
        //keystates
        //
        const Uint8 * keystate = SDL_GetKeyboardState(0);
        glm::vec3 camvec(camera.getRotationMatrix() * glm::vec4(0, 0, -1, 0));
        const float delta = .1;
        if (keystate[SDL_SCANCODE_Q]) {
            ang += .1;
        } else if (keystate[SDL_SCANCODE_E]) {
            ang -= .1;
        } 
        if (keystate[SDL_SCANCODE_A]) {
            camera.yrot += .05;
        } else if (keystate[SDL_SCANCODE_D]) {
            camera.yrot -= .05;
        }

        if (keystate[SDL_SCANCODE_W]) {
            camera.pos += delta * camvec;
        } else if (keystate[SDL_SCANCODE_S]) {
            camera.pos -= delta * camvec;
        }
        //logic
        //cb2.update();
        //rendering
        glm::mat4x4 mrot = glm::rotate(glm::mat4(1.0), ang, glm::vec3(0, 1, 0));
        glm::vec3 lght_vector(mrot * glm::vec4(0, 0, -1, 0));
        Light lght(
                glm::vec4(0, 0, 0, 1.0), 
                glm::vec4(1, 1, 1, 1.0),
                lght_vector
                );
        disp.clear();
        //render from light's point of view
        light_cam.pos = glm::vec3(0, 1, 1);
        light_cam.yrot = ang; //orient camera in the direction of light
        cont.setProgram(&shadow_program);
        shadow_program.uniforms_mat4["mview"] = light_cam.getViewMatrix();
        renderScene(cont);
        if (!vsw) {
            scene_program.clearLights();
            scene_program.addLight(lght);
            copyZBuffer(cont);//backup the z buffer
            //render from camera's point of view
            disp.clear();
            cont.setProgram(&scene_program);
            scene_program.uniforms_mat4["mview"] = camera.getViewMatrix();
            scene_program.uniforms_mat4["to_light_space"] = pers * 
                light_cam.getViewMatrix() * glm::inverse(camera.getViewMatrix());
            scene_program.uniforms_mat4["world_to_light"] = pers * light_cam.getViewMatrix();
            renderScene(cont);
            //ready for rendering
        }
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
