#include "container.h"
#include <iostream>

Container::Container() {
}

void Container::addMesh(const Mesh & mesh) {
    meshes.push_back(mesh);
}

void Container::addLine(const Line & line) {
    lines.push_back(line);
}

void Container::render(Display & disp) {
    using namespace glm;
    const float l = -1, r = 1, t = 1, b = -1;
    const float n = 1.0, f = 100.0;
    mat4x4 pers = transpose(mat4x4(
                2 * n / (r - l), 0, (r + l) / (r - l), 0,
                0, 2 * n / (t - b), (t + b) / (t - b), 0,
                0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
                0, 0, -1, 0
                ));
    //projection
    for (auto & i : lines) {
        i.start.pos = pers * i.start.pos;
        i.finish.pos = pers * i.finish.pos;
        i.start.pos /= i.start.pos.w;
        i.finish.pos /= i.finish.pos.w;
    }

    for (auto & i : meshes) {
        float z1 = i.a.pos.z;
        float z2 = i.b.pos.z;
        float z3 = i.c.pos.z;
        i.a.pos = pers * i.a.pos;
        i.b.pos = pers * i.b.pos;
        i.c.pos = pers * i.c.pos;
        i.a.pos /= i.a.pos.w;
        i.b.pos /= i.b.pos.w;
        i.c.pos /= i.c.pos.w;
    }
    //toscreen
    float w = disp.getWidth();
    float h = disp.getHeight();
    mat4x4 screen = transpose(mat4x4(
                w / 2, 0, 0, w / 2,
                0, -h / 2, 0, h / 2,
                0, 0, 1, 0,
                0, 0, 0, 1
                ));

    for (auto & i : lines) {
        i.start.pos = screen * i.start.pos;
        i.finish.pos = screen * i.finish.pos;
        i.draw(disp);
    }

    for (auto & i : meshes) {
        i.a.pos = screen * i.a.pos;
        i.b.pos = screen * i.b.pos;
        i.c.pos = screen * i.c.pos;
        i.draw(disp);
    }

}
