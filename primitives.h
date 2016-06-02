#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "glmdecl.h"
#include "display.h"
#include <iostream>
#include <map>

class Container;

struct Vertex {
    glm::vec4 pos;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec4 old_pos;
    std::map<std::string, glm::vec4> attributes;
    Vertex(
            const glm::vec4 & vpos, 
            const glm::vec4 & col = glm::vec4(0, 0, 0, 1),
            const glm::vec3 & norm = glm::vec3(0, 0, 0) 
          )
        : pos(vpos), color(col), normal(norm) {
        }

    void addAttribute(std::string s, const glm::vec4 & v) {
        if (attributes.find(s) != attributes.end()) 
            throw "attribute name already taken";
        attributes[s] = v;
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
