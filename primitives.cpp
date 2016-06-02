#include "primitives.h"
#include "container.h"
#include <bits/stdc++.h>

using std::swap;

void Line::draw(Container & cont) const {
    int x1 = start.pos.x, x2 = finish.pos.x;
    int y1 = start.pos.y, y2 = finish.pos.y;
    int dx = x2 - x1, dy = y2 - y1;
    int xinc = sign(dx), yinc = sign(dy);
    int delta = 0, x = x1, y = y1;
    int inc = 0, param = 0;
    bool xmotion;
    int xmove = xinc, ymove = yinc;
    float factor = 0;
    if (abs(dx) > abs(dy)) {
        factor = xinc;
        ymove = 0;
        inc = xinc * dy;
        param = dx;
        xmotion = true;
    } else {
        factor = yinc;
        xmove = 0;
        inc = yinc * dx;
        param = dy;
        xmotion = false;
    }
    int c = abs(param) + 1;
    if (param == 0) { //a single point
        cont.display->drawFragment(start);
        return ;
    }
    glm::vec4 fcolor = factor * (finish.color - start.color) / (float)param;
    glm::vec3 fnormal = factor * (finish.normal - start.normal) / (float)param;
    glm::vec4 color = start.color;
    glm::vec3 normal = start.normal;
    float z = start.pos.z;
    float fz = factor * (finish.pos.z - start.pos.z) / (float)param;
    int Aparam = param * sign(param);
    //for a line to not have holes, this invariant must hold: x(n+1) - x(n) <= 1, y(n+1) - y(n) <= 1
    for (int i = 0; i < c; ++i) {
        Vertex v(glm::vec4(x, y, z, 1.0), color, normal);
        //apply fragment shading
        v = cont.program->getFragmentShader()->shade(v);
        //screen test
        cont.display->drawFragment(v);
        delta += inc;
        if (abs(delta) >= Aparam) {
            //int q = delta / param;
            //if (abs(q) > 1) throw "how come?";
            if (xmotion) y += yinc; else x += xinc;
            delta -= (xmotion ? yinc : xinc) * param;
        }
        x += xmove;
        y += ymove;
        z += fz;
        normal += fnormal;
        color += fcolor;
    }
}

void scanline(Container & cont, int y, 
        int x1, float z1, float w1, glm::vec4 ca, glm::vec3 na, glm::vec4 pa,
        int x2, float z2, float w2, glm::vec4 cb, glm::vec3 nb, glm::vec4 pb
        ) {
    int c = abs(x2 - x1) + 1;
    int x = x1;
    float z = z1;
    float w = w1;
    glm::vec4 color = ca;
    glm::vec4 pos = pa;
    glm::vec3 normal = na;
    float dz = z2 - z1;
    float dx = x2 - x1;
    float dw = w2 - w1;
    int xinc = x1 < x2 ? 1 : -1;
    glm::vec4 dcolor = cb - ca;
    glm::vec4 dpos = pb - pa;
    glm::vec3 dnormal = nb - na;
    float ZF = xinc * dz / dx; 
    float WF = xinc * dw / dx; 
    glm::vec4 CF = float(xinc) * dcolor / dx; 
    glm::vec4 PF = float(xinc) * dpos / dx; 
    glm::vec3 NF = float(xinc) * dnormal / dx; 
    for (int i = 0; i < c; ++i) {
        //setpixel(screen, x, y, 0xff0000);
        Vertex v(glm::vec4(x, y, z, w), color, normal);
        v.old_pos = pos;
        //apply fragment shading
        v = cont.program->getFragmentShader()->r_shade(v);
        //screen test
        cont.display->drawFragment(v);
        x += xinc;
        z += ZF;
        w += WF;
        color += CF;
        normal += NF;
        pos += PF;
    }
}

void Mesh::draw(Container & cont) const {
    SDL_Renderer * renderer = cont.display->getRenderer();
    Mesh tmp(*this);

    tmp.a.pos.x = int(tmp.a.pos.x);
    tmp.a.pos.y = int(tmp.a.pos.y);
    tmp.b.pos.x = int(tmp.b.pos.x);
    tmp.b.pos.y = int(tmp.b.pos.y);
    tmp.c.pos.x = int(tmp.c.pos.x);
    tmp.c.pos.y = int(tmp.c.pos.y);

    if (tmp.a.pos.y > tmp.b.pos.y) swap(tmp.a, tmp.b);
    if (tmp.b.pos.y > tmp.c.pos.y) swap(tmp.b, tmp.c);
    if (tmp.a.pos.y > tmp.b.pos.y) swap(tmp.a, tmp.b);
    float x1 = tmp.a.pos.x, y1 = tmp.a.pos.y, z1 = tmp.a.pos.z;
    float w1 = tmp.a.pos.w;
    float x2 = tmp.b.pos.x, y2 = tmp.b.pos.y, z2 = tmp.b.pos.z;
    float w2 = tmp.b.pos.w;
    float x3 = tmp.c.pos.x, y3 = tmp.c.pos.y, z3 = tmp.c.pos.z;
    float w3 = tmp.c.pos.w;
    glm::vec3 n1 = tmp.a.normal;
    glm::vec3 n2 = tmp.b.normal;
    glm::vec3 n3 = tmp.c.normal;
    glm::vec4 c1 = tmp.a.color;
    glm::vec4 c2 = tmp.b.color;
    glm::vec4 c3 = tmp.c.color;

    glm::vec4 p1 = tmp.a.old_pos;
    glm::vec4 p2 = tmp.b.old_pos;
    glm::vec4 p3 = tmp.c.old_pos;
    float rdy = y3 - y1, rdx = x3 - x1, rdz = z3 - z1;
    float rdw = w3 - w1;

    glm::vec4 rdcolor = c3 - c1;
    glm::vec3 rdnormal = n3 - n1;
    glm::vec4 rdpos = p3 - p1;

    float RXF = rdx / rdy; 
    float RZF = rdz / rdy;
    float RWF = rdw / rdy;

    glm::vec4 RCF = rdcolor / rdy;
    glm::vec3 RNF = rdnormal / rdy;
    glm::vec4 RPF = rdpos / rdy;

    float rx = x1, rz = z1;
    float rw = w1;
    glm::vec4 rcol = c1;
    glm::vec4 rpos = p1;
    glm::vec3 rnorm = n1;
    if (y1 == y3) return ;
    //top half
    if (y1 != y2) {
        float lx = x1;
        float lz = z1;
        float lw = w1;
        glm::vec4 lcol = c1;
        glm::vec4 lpos = p1;
        glm::vec3 lnorm = n1;
        float ldx = x2 - x1, ldy = y2 - y1, ldz = z2 - z1;
        float ldw = w2 - w1;
        glm::vec4 ldcolor = c2 - c1;
        glm::vec4 ldpos = p2 - p1;
        glm::vec3 ldnormal = n2 - n1;
        float LXF = ldx / ldy;
        float LZF = ldz / ldy;
        float LWF = ldw / ldy;

        glm::vec4 LCF = ldcolor / ldy;
        glm::vec4 LPF = ldpos / ldy;
        glm::vec3 LNF = ldnormal / ldy;

        for (int y = y1; y <= y2; ++y) {
            scanline(cont, y, lx, lz, lw, lcol, lnorm, lpos, rx, rz, rw, rcol, rnorm, rpos);

            rx += RXF;
            rz += RZF;
            rw += RWF;
            rcol += RCF;
            rnorm += RNF;
            rpos += RPF;

            lx += LXF;
            lz += LZF;
            lw += LWF;
            lcol += LCF;
            lnorm += LNF;
            lpos += LPF;
        }
    }
    //bottom half
    if (y2 != y3) {
        float lx = x2;
        float lz = z2;
        float lw = w2;
        glm::vec4 lcol = c2;
        glm::vec4 lpos = p2;
        glm::vec3 lnorm = n2;

        float ldx = x3 - x2, ldy = y3 - y2, ldz = z3 - z2;
        float ldw = w3 - w2;
        glm::vec4 ldcolor = c3 - c2;
        glm::vec3 ldnormal = n3 - n2;
        glm::vec4 ldpos = p3 - p2;

        float LXF = ldx / ldy;
        float LZF = ldz / ldy;
        float LWF = ldw / ldy;

        glm::vec4 LCF = ldcolor / ldy;
        glm::vec3 LNF = ldnormal / ldy;
        glm::vec4 LPF = ldpos / ldy;

        int y = y2;

        if (y1 != y2) {//if top half was rasterized, move ahead one step
            ++y;
            lx += LXF;
            lz += LZF;
            lcol += LCF;
            lnorm += LNF;
            lw += LWF;
            lpos += LPF;
        }

        for (; y <= y3; ++y) {
            scanline(cont, y, lx, lz, lw, lcol, lnorm, lpos, rx, rz, rw, rcol, rnorm, rpos);

            rx += RXF;
            rz += RZF;
            rw += RWF;
            rcol += RCF;
            rnorm += RNF;
            rpos += RPF;

            lx += LXF;
            lz += LZF;
            lw += LWF;
            lpos += LPF;
            lcol += LCF;
            lnorm += LNF;
        }

    }
}
