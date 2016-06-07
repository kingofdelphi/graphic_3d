#include "../lib/include/glib.h"

#include "../include/objloader.h"
#include "../include/cube.h"
#include "../include/camera.h"

#include <vector>
#include <tuple>

using namespace glm;
using namespace std;

Camera camera, light_cam;

float tscale = 1;
float asr = 1;
int width = 640, height = 480;

mat4x4 getPerspectiveMatrix() {
    float l = -tscale, r = tscale, t = tscale, b = -tscale;
    //l *= asr;
    //r *= asr;
    float n = 2, f = 100.0;
    float m = 1;
    mat4x4 pers = transpose(mat4x4(
                2 * m / (r - l), 0, (r + l) / (r - l), 0,
                0, 2 * m / (t - b), (t + b) / (t - b), 0,
                0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
                0, 0, -1, 0
                ));
    return pers;
}

class ShadowVertexShader : public VertexShader {
    public:
        Vertex transform(size_t index) {
            Vertex r;
            auto & amap = program->attribute_map;
            auto & unfms = program->uniforms_mat4;

            //add attributes to the vertex
            //currently: position must go first, color must go second, rest can be any
            r.attrs.push_back(amap["position"]->at(index));
            
            //process attributes
            r.attrs[0] = unfms["pers"] * unfms["mview"] * unfms["mmodel"] * r.attrs[0];
            return r;
        }
};

class GouraudVertexShader : public VertexShader {
    public:
        Vertex transform(size_t index) {
            Vertex r;
            auto & amap = program->attribute_map;
            auto & unfms = program->uniforms_mat4;
            //add attributes to the vertex
            //currently: position must go first, color must go second, rest can be any
            r.attrs.push_back(amap["position"]->at(index));
            r.attrs.push_back(amap["color"]->at(index));
            r.attrs.push_back(amap["normal"]->at(index));

            //process attributes
            r.attrs[0] = unfms["mmodel"] * r.attrs[0];
            r.attrs.push_back(unfms["world_to_light"] * r.attrs[0]);//world space coords to light space homogeneous coords

            r.attrs.push_back(r.attrs[0]); //vertex world space position
            
            r.attrs[0] = unfms["pers"] * unfms["mview"] * r.attrs[0];
            r.attrs[2] = unfms["mnormal"] * r.attrs[2];
            
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

class PhongFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            Vertex r(fragment);
            vec4 pos = r.attrs[3]; //oldpos
            pos /= pos.w;
            auto & dbuffer = program->dbuffer;
            glm::vec2 dev = dbuffer->todevice(pos.x, pos.y);
            pos.x = dev.x;
            pos.y = dev.y;
            int X = int(pos.x);
            int Y = int(pos.y);
            float bias = 0.0004;
            float s = -1, e = 1;
            float notshadow = 0;
            for (int i = s; i <= e; ++i) {
                for (int j = s; j <= e; ++j) {
                    int AX = X + i, AY = Y + j;
                    if (dbuffer->inbounds(AX, AY)) {
                        if (pos.z - bias < dbuffer->get(AX, AY)) notshadow += 1;
                    }
                }
            }
            notshadow /= (e - s + 1) * (e - s  + 1); 
            glm::vec4 color(0.5, 0.5, 0.5, 1.0);
            glm::vec3 N = normalize(vec3(r.attrs[2]));
            glm::vec3 ppos(r.attrs[4]);
            //lighting calculations
            for (auto & i : program->lights) {
                glm::vec3 norm(glm::normalize(glm::vec3(i.pos) - ppos));
                float f = dot(norm, N);
                //f = 0.3;
                if (f < 0) f = 0;
                float Cd = 0.5;
                vec4 scale = i.color * Cd * f;
                color += notshadow * scale;
                //color += vec4(0.5, 0.5, 0.5, 0) * notshadow;
            }
            r.attrs[1] *= color;
            r.attrs[1].x = std::min(std::max(r.attrs[1].x, 0.f), 1.f);
            r.attrs[1].y = std::min(std::max(r.attrs[1].y, 0.f), 1.f);
            r.attrs[1].z = std::min(std::max(r.attrs[1].z, 0.f), 1.f);
            return r;
        }
};

Cube cb(-1, 0, -2.5, 1, 1, 1);
Cube cb2(0, -.5/2, -2, .5, .5, .5);
Object obj;

void drawgrid(Container & cont) {
    auto & unfms = cont.program->uniforms_mat4;

    unfms["mmodel"] = glm::mat4x4(1.0);
    unfms["mnormal"] = glm::mat4x4(1.0);

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
        glm::vec4(-2, -.5, -1, 1),
        glm::vec4(-2, -.5, -4, 1),
        glm::vec4(2, -.5, -4, 1),
        glm::vec4(2, -.5, -1, 1),
    };

    auto & unfms = cont.program->uniforms_mat4;
    unfms["mmodel"] = glm::mat4x4(1.0);
    unfms["mnormal"] = glm::mat4x4(1.0);
    cont.program->attribPointer("position", &vertices);
    cont.program->attribPointer("color", &colors);
    cont.program->attribPointer("normal", &normals);

    cont.addMesh(make_tuple(0, 1, 2));
    cont.addMesh(make_tuple(2, 3, 0));

    cont.flush(); //execute requests
    obj.draw(cont);
    //cb.push(cont);
    //cb2.push(cont);
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
    
    glm::mat4x4 pers = getPerspectiveMatrix();
    scene_program.uniforms_mat4["pers"] = pers;

    ShaderProgram shadow_program;
    shadow_program.init(shadow_vshader, shadow_fshader);

    shadow_program.uniforms_mat4["pers"] = pers;

    DBuffer zscene(width, height);
    DBuffer zshadow(800, 800);

    scene_program.dbuffer = &zshadow;
    Container cont;
    cont.setDisplay(&disp);
    obj.load("suzanne.obj");
    bool vsw = false;
    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return ;
            else if(e.type == SDL_WINDOWEVENT) {
                if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    zscene.resize(w, h);
                    asr = w * 1.0 / h;
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
        } else if (keystate[SDL_SCANCODE_E]) {
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

        if (keystate[SDL_SCANCODE_Z]) {
            tscale -= .1;
        } else if (keystate[SDL_SCANCODE_X]) {
            tscale += .1;
        }
        //logic
        //cb2.update();
        //rendering
        Light lght(glm::vec4(0, 1, 2, 1), glm::vec4(1, 1, 1, 1.0));
        light_cam.pos = glm::vec3(lght.pos);
        light_cam.yrot = 0;
        disp.clear();
        zshadow.clear();
        //render from light's point of view
        cont.setProgram(&shadow_program);
        cont.display->setZBuffer(&zshadow);
        shadow_program.uniforms_mat4["mview"] = light_cam.getViewMatrix();

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
            cont.display->setZBuffer(&zscene);
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
