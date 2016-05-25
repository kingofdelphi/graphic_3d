#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "display.h"
#include <iostream>

struct Line {
#define sign(x) ((x) == 0 ? 1 : ((x) > 0 ? 1 : -1))
    glm::vec4 start, finish;

    Line(const glm::vec4 & pstart, const glm::vec4 & pend) : start(pstart), finish(pend) { 
    }

    void draw(Display & disp) const {
        SDL_Renderer * renderer = disp.getRenderer();
        int x1 = start.x, x2 = finish.x;
        int y1 = start.y, y2 = finish.y;
        int dx = x2 - x1, dy = y2 - y1;
        int xinc = sign(dx), yinc = sign(dy);
        int delta = 0, x = x1, y = y1;
        int c = 0, inc = 0, param = 0;
        bool xmotion;
        int xmove = xinc, ymove = yinc;
        if (abs(dx) > abs(dy)) {
            ymove = 0;
            c = abs(dx) + 1;
            inc = xinc * dy;
            param = dx;
            xmotion = true;
        } else {
            xmove = 0;
            c = abs(dy) + 1;
            inc = yinc * dx;
            param = dy;
            xmotion = false;
        }
        int Aparam = param * sign(param);
        //for a line to not have holes, this invariant must hold: x(n+1) - x(n) <= 1, y(n+1) - y(n) <= 1
        for (int i = 0; i < c; ++i, x += xmove, y += ymove) {
            SDL_RenderDrawPoint(renderer, x, y);
            delta += inc;
            if (abs(delta) >= Aparam) {
                //int q = delta / param;
                //if (abs(q) > 1) throw "how come?";
                if (xmotion) y += yinc; else x += xinc;
                delta -= (xmotion ? yinc : xinc) * param;
            }
        }
    }

};

#endif
