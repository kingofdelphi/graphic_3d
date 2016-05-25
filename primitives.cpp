#include "primitives.h"

using std::swap;

void Line::draw(Display & disp) const {
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


//b.y = c.y
void upperHalfDraw(Display & disp, Mesh mesh) {
    if (mesh.b.y != mesh.c.y) throw "b.y == c.y doesnot hold true";
    if (mesh.b.x > mesh.c.x) swap(mesh.b, mesh.c);
    SDL_Renderer * renderer = disp.getRenderer();
    float lx = mesh.a.x, rx = mesh.a.x;
    float lz = mesh.a.z, rz = mesh.a.z;
    float ldx = mesh.b.x - mesh.a.x;
    float ldy = mesh.b.y - mesh.a.y;
    float ldz = mesh.b.z - mesh.a.z;

    float rdx = mesh.c.x - mesh.a.x;
    float rdy = mesh.c.y - mesh.a.y;
    float rdz = mesh.c.z - mesh.a.z;

    float lxfactor = ldx / ldy;
    float rxfactor = rdx / rdy;

    float lzfactor = ldz / ldy;
    float rzfactor = rdz / rdy;

    for (int i = mesh.a.y; i <= mesh.b.y; ++i) {
        float z = lz;
        float f = (rz - lz) / (rx - lx);
        for (int j = lx; j <= rx; ++j) {
            disp.drawFragment(j, i, z, mesh.color);
            z += f;
        }
        lx += lxfactor;
        rx += rxfactor;
        
        lz += lzfactor;
        rz += rzfactor;
    }
}

//a.y = b.y
void lowerHalfDraw(Display & disp, Mesh mesh) {
    if (mesh.a.y != mesh.b.y) throw "a.y == b.y doesnot hold true";
    if (mesh.a.x > mesh.b.x) swap(mesh.a, mesh.b);
    SDL_Renderer * renderer = disp.getRenderer();
    float lx = mesh.a.x, rx = mesh.b.x;
    float lz = mesh.a.z, rz = mesh.b.z;
    float ldx = mesh.c.x - mesh.a.x;
    float ldy = mesh.c.y - mesh.a.y;
    float ldz = mesh.c.z - mesh.a.z;

    float rdx = mesh.c.x - mesh.b.x;
    float rdy = mesh.c.y - mesh.b.y;
    float rdz = mesh.c.z - mesh.b.z;

    float lxfactor = ldx / ldy;
    float rxfactor = rdx / rdy;

    float lzfactor = ldz / ldy;
    float rzfactor = rdz / rdy;
    for (int i = mesh.a.y; i <= mesh.c.y; ++i) {
        float z = lz;
        float f = (rz - lz) / (rx - lx);
        for (int j = lx; j <= rx; ++j) {
            disp.drawFragment(j, i, z, mesh.color);
            z += f;
        }
        lx += lxfactor;
        rx += rxfactor;

        lz += lzfactor;
        rz += rzfactor;
    }
}

void Mesh::draw(Display & disp) const {
    SDL_Renderer * renderer = disp.getRenderer();
    Mesh tmp(*this);
    tmp.a = glm::vec4((int)tmp.a.x, (int)tmp.a.y, tmp.a.z, 1.0);
    tmp.b = glm::vec4((int)tmp.b.x, (int)tmp.b.y, tmp.b.z, 1.0);
    tmp.c = glm::vec4((int)tmp.c.x, (int)tmp.c.y, tmp.c.z, 1.0);
    if (tmp.a.y > tmp.b.y) swap(tmp.a, tmp.b);
    if (tmp.b.y > tmp.c.y) swap(tmp.b, tmp.c);
    if (tmp.a.y > tmp.b.y) swap(tmp.a, tmp.b);
    
    if (tmp.a.y == tmp.c.y) { //a line
        if (tmp.a.x > tmp.b.x) swap(tmp.a, tmp.b);
        if (tmp.b.x > tmp.c.x) swap(tmp.b, tmp.c);
        if (tmp.a.x > tmp.b.x) swap(tmp.a, tmp.b);
        for (int i = tmp.a.x; i <= tmp.c.x; ++i) {
            disp.drawFragment(i, tmp.a.y, tmp.a.z, tmp.color);
        }
    }

    if (tmp.a.y == tmp.b.y) lowerHalfDraw(disp, tmp);
    else if (tmp.b.y == tmp.c.y) upperHalfDraw(disp, tmp);
    else {
        //now interpolate
        float dx = tmp.c.x - tmp.a.x;
        float dy = tmp.c.y - tmp.a.y;
        float dz = tmp.c.z - tmp.a.z;
        float factor = (tmp.b.y - tmp.a.y) / dy;
        float x = tmp.a.x + factor * dx;
        float y = tmp.b.y;
        float z = tmp.a.z + factor * dz;
        glm::vec4 P(x, y, z, 1.0);
        Mesh mup(tmp.a, tmp.b, P, tmp.color);
        Mesh mdown(tmp.b, P, tmp.c, tmp.color);
        upperHalfDraw(disp, mup);
        lowerHalfDraw(disp, mdown);
    }
}
