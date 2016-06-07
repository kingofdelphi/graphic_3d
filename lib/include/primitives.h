#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "glmdecl.h"
#include "display.h"
#include <iostream>
#include <vector>
#include <map>

class Container;

struct Vertex {
    std::map<std::string, glm::vec4> attributes;
    std::vector<glm::vec4> attrs;
    Vertex() {
    }

    //glm::vec4 operator[](const std::string s) const {
    //    if (attributes.find(s) == attributes.end()) {
    //        std::cout << "reading a non existing attribute " << s << "\n";
    //        throw;
    //    }
    //    return attributes.at(s);
    //}

    glm::vec4 & operator[](const std::string s) {
        if (attributes.find(s) == attributes.end()) {
            //std::cout << "creating a new attribute";
        }
        return attributes[s];
    }

    void addAttribute(std::string s, const glm::vec4 & v) {
        if (attributes.find(s) != attributes.end()) 
            std::cout << "overriding existing attribute";
        attributes[s] = v;
    }

};

//point light
struct Light {
    glm::vec4 pos;
    glm::vec4 color;
    Light(
            const glm::vec4 & vpos, 
            const glm::vec4 & col = glm::vec4(1, 1, 1, 1)
         )
        : pos(vpos), color(col) {
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
