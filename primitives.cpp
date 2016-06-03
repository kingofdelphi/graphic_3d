#include "primitives.h"
#include "container.h"
#include <bits/stdc++.h>

using std::swap;

void Line::draw(Container & cont) const {
//    int x1 = start.pos.x, x2 = finish.pos.x;
//    int y1 = start.pos.y, y2 = finish.pos.y;
//    int dx = x2 - x1, dy = y2 - y1;
//    int xinc = sign(dx), yinc = sign(dy);
//    int delta = 0, x = x1, y = y1;
//    int inc = 0, param = 0;
//    bool xmotion;
//    int xmove = xinc, ymove = yinc;
//    float factor = 0;
//    if (abs(dx) > abs(dy)) {
//        factor = xinc;
//        ymove = 0;
//        inc = xinc * dy;
//        param = dx;
//        xmotion = true;
//    } else {
//        factor = yinc;
//        xmove = 0;
//        inc = yinc * dx;
//        param = dy;
//        xmotion = false;
//    }
//    int c = abs(param) + 1;
//    if (param == 0) { //a single point
//        cont.display->drawFragment(start);
//        return ;
//    }
//    glm::vec4 fcolor = factor * (finish.color - start.color) / (float)param;
//    glm::vec3 fnormal = factor * (finish.normal - start.normal) / (float)param;
//    glm::vec4 color = start.color;
//    glm::vec3 normal = start.normal;
//    float z = start.pos.z;
//    float fz = factor * (finish.pos.z - start.pos.z) / (float)param;
//    int Aparam = param * sign(param);
//    //for a line to not have holes, this invariant must hold: x(n+1) - x(n) <= 1, y(n+1) - y(n) <= 1
//    for (int i = 0; i < c; ++i) {
//        Vertex v(glm::vec4(x, y, z, 1.0), color, normal);
//        //apply fragment shading
//        v = cont.program->getFragmentShader()->shade(v);
//        //screen test
//        cont.display->drawFragment(v);
//        delta += inc;
//        if (abs(delta) >= Aparam) {
//            //int q = delta / param;
//            //if (abs(q) > 1) throw "how come?";
//            if (xmotion) y += yinc; else x += xinc;
//            delta -= (xmotion ? yinc : xinc) * param;
//        }
//        x += xmove;
//        y += ymove;
//        z += fz;
//        normal += fnormal;
//        color += fcolor;
//    }
}

void incr(Vertex & a, std::map<std::string, glm::vec4> & delta) {
    for (auto & i : a.attributes) {
        const std::string & k = i.first;
        a[k] += delta[k];
    }
}

void scanline(Container & cont, Vertex a, Vertex b) {
    assert(a["position"].y == b["position"].y);
    a["position"].x = int(a["position"].x);
    b["position"].x = int(b["position"].x);
    float dx = b["position"].x - a["position"].x;
    if (dx < 0) dx = -dx;
    std::map<std::string, glm::vec4> delta;
    float xi = dx != 0 ? dx : 1;
    for (auto & i : a.attributes) {
        const std::string & k = i.first;
        delta[k] = (b[k] - a[k]) / xi;
    }
    for (int i = 0; i <= dx; ++i) {
        cont.display->drawFragment(cont.program->getFragmentShader()->r_shade(a));
        incr(a, delta);
    }
}


void Mesh::draw(Container & cont) const {
    SDL_Renderer * renderer = cont.display->getRenderer();
    Mesh tmp(*this);
    Vertex & p1 = tmp.a;
    Vertex & p2 = tmp.b;
    Vertex & p3 = tmp.c;

    p1["position"].x = int(p1["position"].x);
    p1["position"].y = int(p1["position"].y);

    p2["position"].x = int(p2["position"].x);
    p2["position"].y = int(p2["position"].y);

    p3["position"].x = int(p3["position"].x);
    p3["position"].y = int(p3["position"].y);
    if (p1["position"].y > p2["position"].y) swap(p1, p2);
    if (p2["position"].y > p3["position"].y) swap(p2, p3);
    if (p1["position"].y > p2["position"].y) swap(p1, p2);

    float y1 = p1["position"].y;
    float y2 = p2["position"].y;
    float y3 = p3["position"].y;

    float rdy = y3 - y1;
    if (y1 == y3) return ;
    std::map<std::string, glm::vec4> rdelta;
    for (auto & i : p1.attributes) {
        const std::string & k = i.first;
        rdelta[k] = (p3[k] - p1[k]) / rdy;
    }

    Vertex rp(p1);
    //top half
    if (y1 != y2) {
        Vertex lp(p1);
        float ldy = y2 - y1;
        std::map<std::string, glm::vec4> ldelta;
        for (auto & i : tmp.a.attributes) {
            const std::string & k = i.first;
            ldelta[k] = (p2[k] - p1[k]) / ldy;
        }

        for (int y = y1; y <= y2; ++y) {
            scanline(cont, lp, rp);
            incr(lp, ldelta);
            incr(rp, rdelta);
        }
    }
    //bottom half
    if (y2 != y3) {
        Vertex lp(p2);

        float ldy = y3 - y2;
        std::map<std::string, glm::vec4> ldelta;
        for (auto & i : tmp.a.attributes) {
            const std::string & k = i.first;
            ldelta[k] = (p3[k] - p2[k]) / ldy;
        }

        if (y1 != y2) {//if top half was rasterized, move ahead one step
            incr(lp, ldelta);
        }

        for (int y = rp["position"].y; y <= y3; ++y) {
            scanline(cont, lp, rp);
            assert(lp["position"].y == rp["position"].y);
            incr(lp, ldelta);
            incr(rp, rdelta);
        }

    }
}
