#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/vec3.hpp>
#include "display.h"

struct Line {
    glm::vec3 start, end;
    Line(const glm::vec3 & pstart, const glm::vec3 & pend) : start(pstart), end(pend) { }
    void draw(Display & dpy) {
        SDL_RenderDrawLine(dpy.getRenderer(), start.x, start.y, end.x, end.y);
    }
};

#endif
