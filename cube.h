#ifndef CUBE_H
#define CUBE_H

#include "container.h"
#include "glmdecl.h"

#include <iostream>
#include <tuple>

struct Cube {
    //vertices are in clockwise order
    glm::vec4 verts[8];
    glm::vec3 normals[8];
    glm::vec3 colors[8];
    float x, y, z, l, b, h;
    float rot;
    glm::mat4x4 model;
    std::vector<std::tuple<int, int, int, glm::vec3>> meshes;
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

            meshes.push_back(std::make_tuple(0, 1, 2, glm::vec3(0, 0, 1)));
            meshes.push_back(std::make_tuple(2, 3, 0, glm::vec3(0, 0, 1)));

            meshes.push_back(std::make_tuple(6, 5, 4, glm::vec3(0, 0, -1)));
            meshes.push_back(std::make_tuple(4, 7, 6, glm::vec3(0, 0, -1)));

            meshes.push_back(std::make_tuple(1, 5, 6, glm::vec3(0, -1, 0)));
            meshes.push_back(std::make_tuple(6, 2, 1, glm::vec3(0, -1, 0)));

            meshes.push_back(std::make_tuple(2, 6, 7, glm::vec3(1, 0, 0)));
            meshes.push_back(std::make_tuple(7, 3, 2, glm::vec3(1, 0, 0)));

            meshes.push_back(std::make_tuple(5, 1, 0, glm::vec3(-1, 0, 0)));
            meshes.push_back(std::make_tuple(0, 4, 5, glm::vec3(-1, 0, 0)));


            for (int i = 0; i < 8; ++i) normals[i] = glm::vec3(0, 0, 0);

            for (auto & i : meshes) {
                int a = std::get<0>(i);
                int b = std::get<1>(i);
                int c = std::get<2>(i);
                glm::vec3 n = std::get<3>(i);
                normals[a] += n;
                normals[b] += n;
                normals[c] += n;
            }

            for (int i = 0; i < 8; ++i) {
                normals[i] = glm::normalize(normals[i]);
            }

            colors[0] = glm::vec3(0, 1, 0);
            colors[1] = glm::vec3(0, 1, 0);
            colors[2] = glm::vec3(0, 1, 0);
            colors[3] = glm::vec3(0, 1, 0);
            colors[4] = glm::vec3(0, 1, 0);
            colors[5] = glm::vec3(0, 1, 0);
            colors[6] = glm::vec3(0, 1, 0);
            colors[7] = glm::vec3(0, 1, 0);

        }

    void push(Container & cont) {
        glm::vec4 v[8];
        glm::vec3 norms[8];
        glm::mat4x4 mat = glm::rotate(glm::mat4(1.0), rot, glm::vec3(0, 1, 0));
        model = glm::transpose(glm::mat4x4(
                    1, 0, 0, x,
                    0, 1, 0, y,
                    0, 0, 1, z,
                    0, 0, 0, 1
                    ));

        for (int i = 0; i < 8; ++i) v[i] = model * mat * verts[i];
        for (int i = 0; i < 8; ++i) {
            norms[i] = (mat * glm::vec4(normals[i], 1)).xyz();
        }

        for (int i = 0; i < 4; ++i) {
            glm::vec4 a = v[i];
            glm::vec4 b = v[(i + 1) & 3];
            //cont.addLine(Line(a, b));
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
        for (auto & i : meshes) {
            int a = std::get<0>(i);
            int b = std::get<1>(i);
            int c = std::get<2>(i);
            cont.addMesh(Mesh(Vertex(v[a], colors[a], norms[a]), Vertex(v[b], colors[b], norms[b]), Vertex(v[c], colors[c], norms[c])));
        }

    }

    void update() {
        rot += .02;
    }

};


#endif
