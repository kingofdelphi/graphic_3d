#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "display.h"

struct Line {
    glm::vec4 start, finish;
    Line(const glm::vec4 & pstart, const glm::vec4 & pend) : start(pstart), finish(pend) { 
    }
    void draw(Display & disp) const {
        SDL_RenderDrawLine(disp.getRenderer(), start.x, start.y, finish.x, finish.y);
    }
};

#endif
