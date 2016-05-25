#ifndef CUBE_H
#define CUBE_H

#include "container.h"
#include "glmdecl.h"

#include <iostream>

struct Cube {
    //vertices are in clockwise order
    glm::vec4 verts[8];
    float x, y, z, l, b, h;
    float rot;
    glm::mat4x4 model;
    Cube(float cx, float cy, float cz, float cl, float cb, float ch) :
        x(cx), y(cy), z(cz), l(cl), b(cb), h(ch), rot(0) {

        verts[0] = glm::vec4(-l / 2, -h / 2, b / 2, 1.0);
        verts[1] = glm::vec4(-l / 2, h / 2, b / 2, 1.0);
        verts[2] = glm::vec4(l / 2, h / 2, b / 2, 1.0);
        verts[3] = glm::vec4(l / 2, -h / 2, b / 2, 1.0);

        for (int i = 0; i < 4; ++i) {
            verts[i + 4] = verts[i];
            verts[i + 4].z = -b / 2;
        }

    }

    void push(Container & cont) {
        glm::vec4 v[8];
        glm::mat4x4 mat = glm::rotate(glm::mat4(1.0), rot, glm::vec3(0, 1, 0));
        model = glm::transpose(glm::mat4x4(
                1, 0, 0, x,
                0, 1, 0, y,
                0, 0, 1, z,
                0, 0, 0, 1
                ));

        for (int i = 0; i < 8; ++i) v[i] = model * mat * verts[i];

        for (int i = 0; i < 4; ++i) {
            glm::vec4 a = v[i];
            glm::vec4 b = v[(i + 1) & 3];
            cont.addLine(Line(a, b));
        }

        //for (int i = 0; i < 4; ++i) {
        //    glm::vec4 a = v[i + 4];
        //    glm::vec4 b = v[4 + ((i + 1) & 3)];
        //    cont.addLine(Line(a, b));
        //}

        //for (int i = 0; i < 4; ++i) {
        //    glm::vec4 a = v[i];
        //    glm::vec4 b = v[i + 4];
        //    cont.addLine(Line(a, b));
        //}

        cont.addMesh(Mesh(v[0], v[1], v[2], Color(255, 255, 255)));
        cont.addMesh(Mesh(v[2], v[3], v[0], Color(255, 255, 255)));

        cont.addMesh(Mesh(v[4], v[5], v[6], Color(0, 255, 0)));
        cont.addMesh(Mesh(v[6], v[7], v[4], Color(0, 255, 0)));

        cont.addMesh(Mesh(v[3], v[2], v[6], Color(0, 0, 0)));
        cont.addMesh(Mesh(v[6], v[7], v[3], Color(0, 0, 0)));
    }

    void update() {
        rot += .02;
    }

};


#endif
