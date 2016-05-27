#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "display.h"
#include <iostream>

class Container;

struct Vertex {
    glm::vec4 pos;
    glm::vec3 normal;
    glm::vec4 color;
    Vertex(
            const glm::vec4 & vpos, 
            const glm::vec4 & col = glm::vec4(0, 0, 0, 1),
            const glm::vec3 & norm = glm::vec3(0, 0, 0) 
          )
        : pos(vpos), color(col), normal(norm) {
        }
};

struct Light {
    glm::vec4 pos;
    glm::vec3 normal;
    glm::vec4 color;
    Light(
            const glm::vec4 & vpos, 
            const glm::vec4 & col = glm::vec4(0, 0, 0, 1),
            const glm::vec3 & norm = glm::vec3(0, 0, 0) 
         )
        : pos(vpos), color(col), normal(norm) {
        }
};

#define sign(x) ((x) == 0 ? 1 : ((x) > 0 ? 1 : -1))

struct Line {
    Vertex start, finish;

    Line(const Vertex & va, const Vertex & vb) : start(va), finish(vb) { 
    }

    void draw(Container & cont) const;
};

struct Mesh {
    Vertex a, b, c;

    Mesh(const Vertex & va, const Vertex & vb, const Vertex & vc) : a(va), b(vb), c(vc) {
    }

    void draw(Container & cont) const;

};
#endif
