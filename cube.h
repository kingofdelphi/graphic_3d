#ifndef CUBE_H
#define CUBE_H

#include "container.h"
#include "glmdecl.h"

#include <iostream>

struct Cube {
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

        model = glm::transpose(glm::mat4x4(
                1, 0, 0, x,
                0, 1, 0, y,
                0, 0, 1, z,
                0, 0, 0, 1
                ));


        for (int i = 0; i < 4; ++i) {
            verts[i + 4] = verts[i];
            verts[i].z = -b / 2;
        }

    }

    void push(Container & cont) {
        glm::vec4 v[8];
        glm::mat4x4 mat = glm::rotate(glm::mat4(1.0), rot, glm::vec3(0, 1, 0));

        for (int i = 0; i < 8; ++i) v[i] = model * mat * verts[i];

        for (int i = 0; i < 4; ++i) {
            glm::vec4 a = v[i];
            glm::vec4 b = v[(i + 1) & 3];
            cont.addLine(Line(a, b));
        }

        for (int i = 0; i < 4; ++i) {
            glm::vec4 a = v[i + 4];
            glm::vec4 b = v[4 + ((i + 1) & 3)];
            cont.addLine(Line(a, b));
        }

        for (int i = 0; i < 4; ++i) {
            glm::vec4 a = v[i];
            glm::vec4 b = v[i + 4];
            cont.addLine(Line(a, b));
        }

    }

    void update() {
        rot += .01;
    }

};


#endif
