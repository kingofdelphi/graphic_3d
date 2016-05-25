#include "primitives.h"

using std::swap;

void Line::draw(Display & disp) const {
    SDL_Renderer * renderer = disp.getRenderer();
    int x1 = start.pos.x, x2 = finish.pos.x;
    int y1 = start.pos.y, y2 = finish.pos.y;
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

void scanline(Display & disp, const Vertex & a, const Vertex & b) {
    float z = a.pos.z, dx = b.pos.x - a.pos.x;
    float fz = (b.pos.z - a.pos.z) / dx;
    glm::vec3 fcolor = (b.color - a.color) / dx;
    glm::vec3 color = a.color;
    for (int j = a.pos.x; j <= b.pos.x; ++j) {
        disp.drawFragment(j, a.pos.y, z, color);
        z += fz;
        color += fcolor;
    }
}


//if upperhalf is true, b.y = c.y else a.y = b.y
void halfDraw(Display & disp, Mesh mesh, bool upperhalf) {
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
    SDL_Renderer * renderer = disp.getRenderer();

    float lx = mesh.a.pos.x, rx = upperhalf ? mesh.a.pos.x : mesh.b.pos.x;
    float lz = mesh.a.pos.z, rz = upperhalf ? mesh.a.pos.z : mesh.b.pos.z;
    glm::vec3 lcolor = mesh.a.color, rcolor = upperhalf ? mesh.a.color : mesh.b.color;
    //point that a will connect to, for left edge
    Vertex leftPoint = upperhalf ? mesh.b : mesh.c;
    //point that c will connect to, for right edge
    Vertex rightPoint = upperhalf ? mesh.a : mesh.b;
    float ldx = leftPoint.pos.x - mesh.a.pos.x;
    float ldy = leftPoint.pos.y - mesh.a.pos.y;
    float ldz = leftPoint.pos.z - mesh.a.pos.z;
    glm::vec3 ldcolor = leftPoint.color - mesh.a.color;

    float rdx = mesh.c.pos.x - rightPoint.pos.x;
    float rdy = mesh.c.pos.y - rightPoint.pos.y;
    float rdz = mesh.c.pos.z - rightPoint.pos.z;
    glm::vec3 rdcolor = mesh.c.color - rightPoint.color;

    float lxfactor = ldx / ldy;
    float rxfactor = rdx / rdy;

    float lzfactor = ldz / ldy;
    float rzfactor = rdz / rdy;

    glm::vec3 lcolfactor = ldcolor / ldy;
    glm::vec3 rcolfactor = rdcolor / rdy;
    for (int i = mesh.a.pos.y; i <= mesh.c.pos.y; ++i) {
        glm::vec4 lpos(lx, i, lz, 1.0);
        glm::vec4 rpos(rx, i, rz, 1.0);
        scanline(disp, Vertex(lpos, lcolor), Vertex(rpos, rcolor));
        lx += lxfactor;
        rx += rxfactor;

        lz += lzfactor;
        rz += rzfactor;

        lcolor += lcolfactor;
        rcolor += rcolfactor;
    }
}

//a.y = b.y

void Mesh::draw(Display & disp) const {
    SDL_Renderer * renderer = disp.getRenderer();
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
        for (int i = tmp.a.pos.x; i <= tmp.c.pos.x; ++i) {
            disp.drawFragment(i, tmp.a.pos.y, tmp.a.pos.z, glm::vec3(1, 0, 0));
        }
    }

    if (tmp.a.pos.y == tmp.b.pos.y) halfDraw(disp, tmp, false);
    else if (tmp.b.pos.y == tmp.c.pos.y) halfDraw(disp, tmp, true);
    else {
        //now interpolate
        float dx = tmp.c.pos.x - tmp.a.pos.x;
        float dy = tmp.c.pos.y - tmp.a.pos.y;
        float dz = tmp.c.pos.z - tmp.a.pos.z;
        glm::vec3 dcolor = tmp.c.color - tmp.a.color;
        float factor = (tmp.b.pos.y - tmp.a.pos.y) / dy;
        float x = tmp.a.pos.x + factor * dx;
        float y = tmp.b.pos.y;
        float z = tmp.a.pos.z + factor * dz;
        glm::vec3 color = tmp.a.color + factor * dcolor;
        Vertex P(glm::vec4(x, y, z, 1.0), color);
        Mesh mup(tmp.a, tmp.b, P);
        Mesh mdown(tmp.b, P, tmp.c);
        halfDraw(disp, mup, true);
        halfDraw(disp, mdown, false);
    }
}
