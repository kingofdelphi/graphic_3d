#include "../include/primitives.h"
#include "../include/container.h"
#include <vector>

using std::swap;

void incr(Vertex & a, std::vector<glm::vec4> & delta) {
    for (size_t i = 0; i < delta.size(); ++i) {
        a.attrs[i] += delta[i];
    }
}

void Line::draw(Container & cont) const {
    SDL_Renderer * r = cont.display->getRenderer();
    int x1 = start.attrs[0].x, x2 = finish.attrs[0].x;
    int y1 = start.attrs[0].y, y2 = finish.attrs[0].y;
    int dx = x2 - x1, dy = y2 - y1;

    float param = 0;

    if (abs(dx) > abs(dy)) {
        param = abs(dx);
    } else {
        param = abs(dy);
    }

    if (param == 0) { //a single point
        cont.program->getFragmentShader()->r_shade(start, cont);
        return ;
    }

    size_t N = start.attrs.size();
    std::vector<glm::vec4> delta(N);

    for (size_t i = 0; i < N; ++i) {
        delta[i] = (finish.attrs[i] - start.attrs[i]) / param;
    }

    Vertex a(start);
    for (int i = 0; i <= param; ++i) {
        //apply fragment shading
        cont.program->getFragmentShader()->r_shade(a, cont);
        incr(a, delta);
    }
}

void scanline(Container & cont, Vertex a, Vertex b) {
    a.attrs[0].x = int(a.attrs[0].x);
    b.attrs[0].x = int(b.attrs[0].x);

    float dx = b.attrs[0].x - a.attrs[0].x;
    if (dx < 0) dx = -dx;
    if (dx == 0) {
        cont.program->getFragmentShader()->r_shade(a, cont);
        return ;
    }

    size_t N = a.attrs.size();
    std::vector<glm::vec4> delta(N);

    for (size_t i = 0; i < N; ++i) {
        delta[i] = (b.attrs[i] - a.attrs[i]) / dx;
    }

    for (int i = 0; i <= dx; ++i) {
        cont.program->getFragmentShader()->r_shade(a, cont);
        incr(a, delta);
    }
}


void Mesh::draw(Container & cont) const {
    Mesh tmp(*this);

    Vertex & p1 = tmp.a;
    Vertex & p2 = tmp.b;
    Vertex & p3 = tmp.c;

    p1.attrs[0].x = int(p1.attrs[0].x);
    p1.attrs[0].y = int(p1.attrs[0].y);

    p2.attrs[0].x = int(p2.attrs[0].x);
    p2.attrs[0].y = int(p2.attrs[0].y);

    p3.attrs[0].x = int(p3.attrs[0].x);
    p3.attrs[0].y = int(p3.attrs[0].y);
    if (p1.attrs[0].y > p2.attrs[0].y) swap(p1, p2);
    if (p2.attrs[0].y > p3.attrs[0].y) swap(p2, p3);
    if (p1.attrs[0].y > p2.attrs[0].y) swap(p1, p2);

    float y1 = p1.attrs[0].y;
    float y2 = p2.attrs[0].y;
    float y3 = p3.attrs[0].y;

    float rdy = y3 - y1;
    if (y1 == y3) {
        if (p1.attrs[0].x > p2.attrs[0].x) swap(p1, p2);
        if (p2.attrs[0].x > p3.attrs[0].x) swap(p2, p3);
        if (p1.attrs[0].x > p2.attrs[0].x) swap(p1, p2);
        scanline(cont, p1, p2);
        return ;
    }

    size_t N = p1.attrs.size();
    std::vector<glm::vec4> rdelta(N);

    for (size_t i = 0; i < N; ++i) {
        rdelta[i] = (p3.attrs[i] - p1.attrs[i]) / rdy;
    }

    Vertex rp(p1);
    //top half
    if (y1 != y2) {
        Vertex lp(p1);
        float ldy = y2 - y1;
        std::vector<glm::vec4> ldelta(N);

        for (size_t i = 0; i < N; ++i) {
            ldelta[i] = (p2.attrs[i] - p1.attrs[i]) / ldy;
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

        std::vector<glm::vec4> ldelta(N);

        for (size_t i = 0; i < N; ++i) {
            ldelta[i] = (p3.attrs[i] - p2.attrs[i]) / ldy;
        }

        if (y1 != y2) {//if top half was rasterized, move ahead one step
            incr(lp, ldelta);
        }

        for (int y = rp.attrs[0].y; y <= y3; ++y) {
            scanline(cont, lp, rp);
            //assert(lp.attrs[0].y == rp.attrs[0].y);
            incr(lp, ldelta);
            incr(rp, rdelta);
        }

    }
}
