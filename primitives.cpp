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
        v = cont.fshader->shade(v);
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

void scanline(Container & cont, const Vertex & a, const Vertex & b) {
    float z = a.pos.z, dx = b.pos.x - a.pos.x;
    float fz = (b.pos.z - a.pos.z) / dx;
    glm::vec4 fcolor = (b.color - a.color) / dx;
    glm::vec3 fnormal = (b.normal - a.normal) / dx;
    glm::vec4 color = a.color;
    glm::vec3 normal = a.normal;
    for (int j = a.pos.x; j <= b.pos.x; ++j) {
        Vertex v(glm::vec4(j, a.pos.y, z, 1.0), color, normal);
        //apply fragment shading
        v = cont.fshader->shade(v);
        //screen test
        cont.display->drawFragment(v);
        z += fz;
        color += fcolor;
        normal += fnormal;
    }
}


//if upperhalf is true, b.y = c.y else a.y = b.y
void halfDraw(Container & cont, Mesh mesh, bool upperhalf) {
    try {
        if (upperhalf) {
            if (mesh.b.pos.y != mesh.c.pos.y) throw "wrong triangle, not upperhalf";
        } else {
            if (mesh.a.pos.y != mesh.b.pos.y) throw "wrong triangle, not lowerhalf";
        }
    } catch (const char * s) {
        std::cout << "Error: " << s << "\n";
        throw ;
    }
    if (upperhalf) {
        if (mesh.b.pos.x > mesh.c.pos.x) swap(mesh.b, mesh.c);
    } else {
        if (mesh.a.pos.x > mesh.b.pos.x) swap(mesh.a, mesh.b);
    }
    SDL_Renderer * renderer = cont.display->getRenderer();

    float lx = mesh.a.pos.x, rx = upperhalf ? mesh.a.pos.x : mesh.b.pos.x;
    float lz = mesh.a.pos.z, rz = upperhalf ? mesh.a.pos.z : mesh.b.pos.z;
    glm::vec4 lcolor = mesh.a.color, rcolor = upperhalf ? mesh.a.color : mesh.b.color;
    glm::vec3 lnormal = mesh.a.normal, rnormal = upperhalf ? mesh.a.normal : mesh.b.normal;
    //point that a will connect to, for left edge
    Vertex leftPoint = upperhalf ? mesh.b : mesh.c;
    //point that c will connect to, for right edge
    Vertex rightPoint = upperhalf ? mesh.a : mesh.b;
    glm::vec4 ldpos = leftPoint.pos - mesh.a.pos;
    glm::vec4 ldcolor = leftPoint.color - mesh.a.color;
    glm::vec3 ldnormal = leftPoint.normal - mesh.a.normal;

    glm::vec4 rdpos = mesh.c.pos - rightPoint.pos;
    glm::vec4 rdcolor = mesh.c.color - rightPoint.color;
    glm::vec3 rdnormal = mesh.c.normal - rightPoint.normal;

    float lxfactor = ldpos.x / ldpos.y;
    float rxfactor = rdpos.x / rdpos.y;

    float lzfactor = ldpos.z / ldpos.y;
    float rzfactor = rdpos.z / rdpos.y;

    glm::vec4 lcolfactor = ldcolor / ldpos.y;
    glm::vec4 rcolfactor = rdcolor / rdpos.y;

    glm::vec3 lnormfactor = ldnormal / ldpos.y;
    glm::vec3 rnormfactor = rdnormal / rdpos.y;

    for (int i = mesh.a.pos.y; i <= mesh.c.pos.y; ++i) {
        glm::vec4 lpos(lx, i, lz, 1.0);
        glm::vec4 rpos(rx, i, rz, 1.0);
        scanline(cont, Vertex(lpos, lcolor, lnormal), Vertex(rpos, rcolor, rnormal));
        lx += lxfactor;
        rx += rxfactor;

        lz += lzfactor;
        rz += rzfactor;

        lcolor += lcolfactor;
        rcolor += rcolfactor;

        lnormal += lnormfactor;
        rnormal += rnormfactor;
    }
}

//a.y = b.y

void Mesh::draw(Container & cont) const {
    SDL_Renderer * renderer = cont.display->getRenderer();
    Mesh tmp(*this);
    tmp.a.pos = glm::vec4((int)tmp.a.pos.x, (int)tmp.a.pos.y, tmp.a.pos.z, 1.0);
    tmp.b.pos = glm::vec4((int)tmp.b.pos.x, (int)tmp.b.pos.y, tmp.b.pos.z, 1.0);
    tmp.c.pos = glm::vec4((int)tmp.c.pos.x, (int)tmp.c.pos.y, tmp.c.pos.z, 1.0);
    if (tmp.a.pos.y > tmp.b.pos.y) swap(tmp.a, tmp.b);
    if (tmp.b.pos.y > tmp.c.pos.y) swap(tmp.b, tmp.c);
    if (tmp.a.pos.y > tmp.b.pos.y) swap(tmp.a, tmp.b);

    if (tmp.a.pos.y == tmp.c.pos.y) { //a line
        if (tmp.a.pos.x > tmp.b.pos.x) swap(tmp.a, tmp.b);
        if (tmp.b.pos.x > tmp.c.pos.x) swap(tmp.b, tmp.c);
        if (tmp.a.pos.x > tmp.b.pos.x) swap(tmp.a, tmp.b);
        scanline(cont, tmp.a, tmp.c);
    }

    if (tmp.a.pos.y == tmp.b.pos.y) halfDraw(cont, tmp, false);
    else if (tmp.b.pos.y == tmp.c.pos.y) halfDraw(cont, tmp, true);
    else {
        //now interpolate
        float dx = tmp.c.pos.x - tmp.a.pos.x;
        float dy = tmp.c.pos.y - tmp.a.pos.y;
        float dz = tmp.c.pos.z - tmp.a.pos.z;
        glm::vec4 dcolor = tmp.c.color - tmp.a.color;
        glm::vec3 dnormal = tmp.c.normal - tmp.a.normal;
        float factor = (tmp.b.pos.y - tmp.a.pos.y) / dy;
        float x = tmp.a.pos.x + factor * dx;
        float y = tmp.b.pos.y;
        float z = tmp.a.pos.z + factor * dz;
        glm::vec4 color = tmp.a.color + factor * dcolor;
        glm::vec3 normal = tmp.a.normal + factor * dnormal;
        Vertex P(glm::vec4(x, y, z, 1.0), color, normal);
        Mesh mup(tmp.a, tmp.b, P);
        Mesh mdown(tmp.b, P, tmp.c);
        halfDraw(cont, mup, true);
        halfDraw(cont, mdown, false);
    }
}
