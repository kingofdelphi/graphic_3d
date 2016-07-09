#include "../lib/include/glib.h"

#include "../include/objloader.h"
#include "../include/cube.h"
#include "../include/camera.h"
#include "../include/sphere.h"
#include "../include/config.h"

#include <vector>
#include <tuple>

using namespace glm;
using namespace std;

glm::mat4x4 getPerspectiveMatrix(float asr, float scale = 1.0) {
    float l = -asr * scale, r = asr * scale, t = scale, b = -scale;
    float n = 1.5, f = 100.0;
    float m = 1; //this is the projection plane's depth
    glm::mat4x4 pers = glm::transpose(glm::mat4x4(
                2 * m / (r - l), 0, (r + l) / (r - l), 0,
                0, 2 * m / (t - b), (t + b) / (t - b), 0,
                0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
                0, 0, -1, 0
                ));
    return pers;
}

Cube cb(-1, 0, -2.5, 1, 1, 1);
Cube cb2(0, -.5/2, -2, .5, .5, .5);

Object obj;

const int N = 3;

Sphere sph[N] = {
    Sphere(0, 0, 0, .4, glm::vec4(1, 1, 0, 0), glm::vec3(0, 0, 0)),
    Sphere(0, 0, 5, .2, glm::vec4(0, 1, 0, 0), glm::vec3(-0.108, 0, 0)),
    Sphere(0, 0, 6.2, .1, glm::vec4(1, 1, 1, 0), glm::vec3(-0.193, 0.02, .00)),
};

void mv(Sphere & a, Sphere & b, float ma, float mb) {
    glm::vec3 dir = a.pos - b.pos;
    float distsq = glm::dot(dir, dir);
    dir = dir / sqrt(distsq);
    float G = 0.0005;
    float force = G * ma * mb / distsq;
    a.vel += force * -dir / ma;
    b.vel += force * dir / mb;
}

void upd() {
    float masses[N] = {120, 20, 3};
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            mv(sph[i], sph[j], masses[i], masses[j]);
        }
    }
    //starts from 1, thus assumes that sun is fixed
    for (int i = 1; i < N; ++i) {
        sph[i].update();
    }
}

void drawgrid(Container & cont) {
    auto & unfms = cont.program->uniforms_mat4;

    unfms["mmodel"] = glm::mat4x4(1.0);
    unfms["mnormal"] = glm::mat4x4(1.0);
    unfms["PVM"] = unfms["PV"];

    float gap = 1;
    float span = 10;
    float steps = 10;

    std::vector<glm::vec4> vertices;

    for (int i = -10; i <= 10; ++i) {
        glm::vec3 normal(0, 0, 1);
        glm::vec4 a(i * gap, -1, -span, 1);
        glm::vec4 b(i * gap, -1, span, 1);
        int c = vertices.size();
        vertices.push_back(a);
        vertices.push_back(b);
        cont.addLine(std::make_pair(c, c + 1));
    }

    for (int i = -10; i <= 10; ++i) {
        glm::vec3 normal(1, 0, 0);
        glm::vec4 a(-span, -1, i * gap, 1);
        glm::vec4 b(span, -1, i * gap, 1);
        int c = vertices.size();
        vertices.push_back(a);
        vertices.push_back(b);
        cont.addLine(std::make_pair(c, c + 1));
    }

    std::vector<glm::vec4> colors(vertices.size(), glm::vec4(1, 0, 0, 1));
    std::vector<glm::vec4> normals(vertices.size(), glm::vec4(0, 0, 1, 0));

    cont.program->attribPointer("position", &vertices);
    cont.program->attribPointer("color", &colors);
    cont.program->attribPointer("normal", &normals);

    cont.flush();

}

void renderScene(Container & cont) {
    //add objects to the container
    if (cont.program == nullptr) throw "no vertex shader";

    vector<vec4> colors = {
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 1, 1),
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 0, 1, 1),
    };

    vector<vec4> normals = {
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 1, 0),
    };

    vector<vec4> vertices = {
        glm::vec4(-2, -.5, 1, 1),
        glm::vec4(-2, -.5, -4, 1),
        glm::vec4(2, -.5, -4, 1),
        glm::vec4(2, -.5, 1, 1),
    };

    auto & unfms = cont.program->uniforms_mat4;
    unfms["mmodel"] = glm::mat4x4(1.0);
    unfms["mnormal"] = glm::mat4x4(1.0);
    unfms["PVM"] = unfms["PV"] * unfms["mmodel"];

    cont.program->attribPointer("position", &vertices);
    cont.program->attribPointer("color", &colors);
    cont.program->attribPointer("normal", &normals);

    cont.addMesh(make_tuple(0, 1, 2));
    cont.addMesh(make_tuple(2, 3, 0));

    cont.flush(); //execute requests
    //obj.draw(cont);
    //cb.push(cont);
    //cb2.push(cont);
    
    for (int i = 0; i < N; ++i) {
        sph[i].push(cont);
    }
    cont.flush(); //execute requests
    drawgrid(cont);
}

void logic(Display & disp) {
    SDL_Event e;

    VertexShader * vshader = new GouraudVertexShader();
    VertexShader * shadow_vshader = new ShadowVertexShader();

    FragmentShader * fshader = new PhongFragmentShader();
    FragmentShader * shadow_fshader = new PassThroughFragmentShader();

    ShaderProgram scene_program;
    scene_program.init(vshader, fshader);
    
    glm::mat4x4 pers = getPerspectiveMatrix(1.0);
    scene_program.uniforms_mat4["pers"] = pers;

    ShaderProgram shadow_program;
    shadow_program.init(shadow_vshader, shadow_fshader);

    shadow_program.uniforms_mat4["pers"] = getPerspectiveMatrix(1.0, 3);

    DBuffer zscene(640, 480);
    DBuffer zshadow(800, 800);

    scene_program.dbuffer = &zshadow;
    Container cont;
    cont.setDisplay(&disp);
    obj.load("suzanne.obj");
    bool vsw = false;

    Camera camera, light_cam;

    float asr = 1.0, tscale = 1.0;

    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return ;
            else if(e.type == SDL_WINDOWEVENT) {
                if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    zscene.resize(w, h);
                    asr = w * 1.0 / h;
                    scene_program.uniforms_mat4["pers"] = getPerspectiveMatrix(asr);
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
        if (keystate[SDL_SCANCODE_Q]) {
            camera.xrot -= .05;
        } else if (keystate[SDL_SCANCODE_E]) {
            camera.xrot += .05;
        } 

        if (keystate[SDL_SCANCODE_A]) {
            camera.yrot += .05;
        } else if (keystate[SDL_SCANCODE_D]) {
            camera.yrot -= .05;
        }

        glm::vec3 camvec(camera.getRotationMatrix() * glm::vec4(0, 0, -1, 0));

        const float delta = .1;

        if (keystate[SDL_SCANCODE_W]) {
            camera.pos += delta * camvec;
        } else if (keystate[SDL_SCANCODE_S]) {
            camera.pos -= delta * camvec;
        }

        if (keystate[SDL_SCANCODE_Z]) {
            tscale -= .1; //fov
            scene_program.uniforms_mat4["pers"] = getPerspectiveMatrix(asr, tscale);
        } else if (keystate[SDL_SCANCODE_X]) {
            tscale += .1;
            scene_program.uniforms_mat4["pers"] = getPerspectiveMatrix(asr, tscale);
        }

        //logic
        cb2.update();
        obj.update();
        upd();
        //rendering
        Light lght(glm::vec4(0, 1, 2, 1), glm::vec4(1, 1, 1, 1.0));
        light_cam.pos = glm::vec3(lght.pos);
        light_cam.yrot = 0;
        zshadow.clear();
        //render from light's point of view
        cont.setProgram(&shadow_program);
        cont.setZBuffer(&zshadow);
        shadow_program.uniforms_mat4["mview"] = light_cam.getViewMatrix();
        shadow_program.uniforms_mat4["PV"] = shadow_program.uniforms_mat4["pers"] * shadow_program.uniforms_mat4["mview"];

        //render back faces only from light's point of view
        cont.enableCull(FRONTFACE);
        renderScene(cont);
        if (!vsw) {
            cont.enableCull(BACKFACE);
            scene_program.clearLights();
            scene_program.addLight(lght);
            //render from camera's point of view
            disp.clear();
            zscene.clear();
            cont.setProgram(&scene_program);
            cont.setZBuffer(&zscene);
            scene_program.uniforms_mat4["mview"] = camera.getViewMatrix();
            scene_program.uniforms_mat4["PV"] = scene_program.uniforms_mat4["pers"] * scene_program.uniforms_mat4["mview"];
            scene_program.uniforms_mat4["world_to_light"] = shadow_program.uniforms_mat4["PV"];
            renderScene(cont);
            //ready for rendering
        }
        disp.flush(); //refresh the screen
    }

    delete vshader;
    delete shadow_vshader;

    delete fshader;
    delete shadow_fshader;
}

int main(int argc, char ** argv) {
    Display disp;
    logic(disp);
    return 0;
}
