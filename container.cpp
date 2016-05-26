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

    //projection
    for (auto & i : lines) {
        i.start = vshader.transform(i.start);
        i.finish = vshader.transform(i.finish);
    }

    for (auto & i : meshes) {
        i.a = vshader.transform(i.a);
        i.b = vshader.transform(i.b);
        i.c = vshader.transform(i.c);
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

    disp.flush();

}
