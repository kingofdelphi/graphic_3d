#ifndef CUBE_H
#define CUBE_H

#include "container.h"
#include "glmdecl.h"

#include <iostream>

struct Cube {
    //vertices are in clockwise order
    glm::vec4 verts[8];
    glm::vec3 colors[8];
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
        colors[0] = glm::vec3(1, 1, 1);
        colors[1] = glm::vec3(0, 1, 0);
        colors[2] = glm::vec3(1, 1, 1);
        colors[3] = glm::vec3(0, 1, 1);
        colors[4] = glm::vec3(0, 0, 0);
        colors[5] = glm::vec3(1, 0, 0);
        for (int i = 2; i < 4; ++i) colors[i + 4] = colors[i];
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

        cont.addMesh(Mesh(Vertex(v[0], colors[0]), Vertex(v[1], colors[1]), Vertex(v[2], colors[2])));
        cont.addMesh(Mesh(Vertex(v[2], colors[2]), Vertex(v[3], colors[3]), Vertex(v[0], colors[0])));
                                              
        cont.addMesh(Mesh(Vertex(v[4], colors[4]), Vertex(v[5], colors[5]), Vertex(v[6], colors[6])));
        cont.addMesh(Mesh(Vertex(v[6], colors[6]), Vertex(v[7], colors[7]), Vertex(v[4], colors[4])));
                                             
        cont.addMesh(Mesh(Vertex(v[3], colors[3]), Vertex(v[2], colors[2]), Vertex(v[6], colors[6])));
        cont.addMesh(Mesh(Vertex(v[6], colors[6]), Vertex(v[7], colors[7]), Vertex(v[3], colors[3])));
    }

    void update() {
        rot += .02;
    }

};


#endif
