#include "container.h"
#include <iostream>

Container::Container() 
    : fshader(nullptr), display(nullptr), vshader(nullptr) {
}

void Container::addMesh(const Mesh & mesh) {
    meshes.push_back(mesh);
}

void Container::addLine(const Line & line) {
    lines.push_back(line);
}

Line Container::clipLine(Line line) {
    if (line.start.pos.w > line.finish.pos.w) 
        std::swap(line.start, line.finish);
    //line.a.pos.x =
    return line;
}

void Container::flush() {

    if (vshader == nullptr) throw "Error: Vertex Shader not specified";
    if (fshader == nullptr) throw "Error: Fragment Shader not specified";
    using namespace glm;

    //projection
    for (auto & i : lines) {
        i.start = vshader->transform(i.start);
        i.finish = vshader->transform(i.finish);
    }

    for (auto & i : meshes) {
        i.a = vshader->transform(i.a);
        i.b = vshader->transform(i.b);
        i.c = vshader->transform(i.c);
    }

    //clipping
    for (auto & i : lines) {
        //i = clipLine(i);
    }

    //perspective divide
    for (auto & i : lines) {
        i.start.pos /= i.start.pos.w;
        i.finish.pos /= i.finish.pos.w;
    }

    for (auto & i : meshes) {
        i.a.pos /= i.a.pos.w;
        i.b.pos /= i.b.pos.w;
        i.c.pos /= i.c.pos.w;
    }

    //fragment shader
    fshader->clearLights();
    for (auto & i : vshader->getLights()) {
        fshader->addLight(i);
    }

    //toscreen
    float w = display->getWidth();
    float h = display->getHeight();

    mat4x4 screen = transpose(mat4x4(
                w / 2, 0, 0, w / 2,
                0, -h / 2, 0, h / 2,
                0, 0, 1, 0,
                0, 0, 0, 1
                ));

    for (auto & i : lines) {
        i.start.pos = screen * i.start.pos;
        i.finish.pos = screen * i.finish.pos;
        i.draw(*this);
    }

    for (auto & i : meshes) {
        i.a.pos = screen * i.a.pos;
        i.b.pos = screen * i.b.pos;
        i.c.pos = screen * i.c.pos;
        i.draw(*this);
    }

    clearRequests();

}
