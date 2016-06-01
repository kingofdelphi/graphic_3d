#include "glmdecl.h"
#include "display.h"
#include "container.h"
#include "cube.h"
#include "vshader.h"
#include "fragmentshader.h"
#include "primitives.h"
#include "objloader.h"
#include "camera.h"
#include <stdio.h>

using namespace glm;
using namespace std;

Camera camera;

mat4x4 getPerspectiveMatrix() {
    //return glm::mat4x4(1.0);
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
            r.pos = mview * mmodel * r.pos;
            r.pos = tmat * r.pos;
            r.normal = vec3(mnormal * vec4(r.normal, 0));
            return r;
        }
};

Camera light_cam;

class GouraudVertexShader : public VertexShader {
    public:
        Vertex transform(const Vertex & v) {
            Vertex r(v);
            r.old_pos = mview * mmodel * r.pos; //store previous world position
            r.pos = tmat * r.old_pos;

            //world normal
            r.normal = vec3(mnormal * vec4(r.normal, 0));
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
glm::mat4x4 pers;
glm::mat4x4 vm;
glm::mat4x4 invcamera;

class PhongFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            Vertex r(fragment);
            vec4 old_pos;
            float xp = r.pos.x * 2.0 / width - 1;
            float yp = 1 - r.pos.y * 2.0 / height;
            old_pos.z = snormal.w / (snormal.x * xp + snormal.y * yp -
                    snormal.z);
            //r.pos.z = 1 / r.pos.z;
            //std::cout << abs(old_pos.z - r.pos.z) << "\n";
            //old_pos.z = r.pos.z;
            old_pos.x = xp * -old_pos.z;
            old_pos.y = yp * -old_pos.z;
            old_pos.w = 1.0;
            vec4 pos = invcamera * old_pos;
            float pz = pos.z;
            pos /= -pz;
            pos.z = pz;
            pos.x = width * (1 + pos.x) * 0.5;
            pos.y = height * (1 - pos.y) * 0.5;
            float notshadow = 0;
            int X = int(pos.x);
            int Y = int(pos.y);
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int AX = X + i, AY = Y + j;
                    if (0 <= AX && AX < width && 0 <= AY && AY < height) {
                        if (pos.z + .060252 > zbuffer[AY][AX]) notshadow += 1;
                    }
                }
            }
            notshadow /= 9.0;
            glm::vec4 color(0.5, 0.5, 0.5, 1.0);
            r.normal = normalize(r.normal);
            //color += glm::vec4(0.3, .3, .3, 0);
            //lighting calculations
            for (auto & i : lights) {
                float f = dot(-i.normal, r.normal);
                //f = 0.3;
                if (f < 0) f = 0;
                float Cd = 0.3;
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

Cube cb(-1, 0, -2.5, 1, 1, 1);
Cube cb2(0, -.5/2, -2, .5, .5, .5);

void renderScene(Container & cont) {
    //add objects to the container
    if (cont.vshader == nullptr) throw "no vertex shader";
    if (cont.fshader == nullptr) throw "no fragment shader";
    cont.vshader->setModelMatrix(glm::mat4x4(1.0));
    cont.vshader->setNormalMatrix(glm::mat4x4(1.0));
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
    glm::vec4 acolor(0, 1, 0, 1);
    glm::vec4 bcolor(1, 0, 0, 1);
    glm::vec4 a(-2, -.5, -1, 1);
    glm::vec4 b(-2, -.5, -4, 1);
    glm::vec4 c(2, -.5, -4, 1);
    glm::vec4 d(2, -.5, -1, 1);

    glm::vec4 e(-.2, -.25, -.5, 1);
    glm::vec4 f(-.2, .2, -.5, 1);
    glm::vec4 g(.2, .2, -.5, 1);
    glm::vec4 h(.2, -.2, -.5, 1);

    Mesh l(Vertex(a, acolor, normal),
            Vertex(b, bcolor, normal),
            Vertex(c, acolor, normal)
          );

    Mesh r(Vertex(c, acolor, normal),
            Vertex(d, bcolor, normal),
            Vertex(a, acolor, normal)
          );

    Mesh m1(Vertex(e, acolor, glm::vec3(0, 0, 1)),
            Vertex(f, acolor, glm::vec3(0, 0, 1)),
            Vertex(g, acolor, glm::vec3(0, 0, 1))
           );

    Mesh m2(Vertex(g, acolor, glm::vec3(0, 0, 1)),
            Vertex(h, acolor, glm::vec3(0, 0, 1)),
            Vertex(e, acolor, glm::vec3(0, 0, 1))
           );


    cont.addMesh(l);
    cont.addMesh(r);
    //cont.addMesh(m1);
    //cont.addMesh(m2);

    cont.flush(); //execute requests
    //Helper::drawObj(cont, "suzanne.obj");
    //cb.push(cont);
    cb2.push(cont);
    cont.flush(); //execute requests
}

void logic(Display & disp) {
    SDL_Event e;
    VertexShader * vshader = new GouraudVertexShader();
    VertexShader * shadow_vshader = new ShadowVertexShader();
    pers = getPerspectiveMatrix();
    vshader->setTransformMatrix(pers);
    shadow_vshader->setTransformMatrix(pers);
    FragmentShader * fshader = new PhongFragmentShader();
    FragmentShader * shadow_fshader = new PassThroughFragmentShader();
    Container cont;
    cont.setDisplay(&disp);
    float ang = 0;//3.1415 * .5;
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
        vshader->clearLights();
        vshader->addLight(lght);
        disp.clear();
        //render from light's point of view
        light_cam.pos = glm::vec3(0, 1, 1);
        light_cam.yrot = ang; //orient camera in the direction of light
        vm = pers * light_cam.getViewMatrix();
        cont.setVertexShader(shadow_vshader);
        cont.setFragmentShader(shadow_fshader);
        cont.vshader->setViewMatrix(light_cam.getViewMatrix());
        renderScene(cont);
        if (!vsw) {
            copyZBuffer(cont);//backup the z buffer
            //render from camera's point of view
            disp.clear();
            cont.setVertexShader(vshader);
            cont.setFragmentShader(fshader);

            invcamera = light_cam.getViewMatrix() * glm::inverse(camera.getViewMatrix());
            cont.vshader->setViewMatrix(camera.getViewMatrix());
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
