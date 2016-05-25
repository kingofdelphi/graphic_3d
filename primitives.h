#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "display.h"
#include <iostream>
#include "color.h"

struct Line {
#define sign(x) ((x) == 0 ? 1 : ((x) > 0 ? 1 : -1))
    glm::vec4 start, finish;

    Line(const glm::vec4 & pstart, const glm::vec4 & pend) : start(pstart), finish(pend) { 
    }

    void draw(Display & disp) const;

};

struct Mesh {
    glm::vec4 a, b, c;
    Color color;
    Mesh(const glm::vec4 & pa, const glm::vec4 & pb, const glm::vec4 & pc, const Color & col = Color()) 
        : a(pa), b(pb), c(pc), color(col) { 
    }

    void setColor(Color col) {
        color = col;
    }

    void draw(Display & disp) const;

};
#endif
