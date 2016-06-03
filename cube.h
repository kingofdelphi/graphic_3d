#ifndef CUBE_H
#define CUBE_H

#include "container.h"
#include "glmdecl.h"

#include <iostream>
#include <tuple>
#include <vector>

struct Cube {
    //vertices are in clockwise order
    std::vector<glm::vec4> verts;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> colors;
    float x, y, z, l, b, h;
    float rot;
    glm::mat4x4 model;
    std::vector<std::tuple<int, int, int>> meshes;
    Cube(float cx, float cy, float cz, float cl, float cb, float ch) :
        x(cx), y(cy), z(cz), l(cl), b(cb), h(ch), rot(0) {
            verts.resize(8);
            verts[0] = glm::vec4(-l / 2, -h / 2, b / 2, 1.0);
            verts[1] = glm::vec4(-l / 2, h / 2, b / 2, 1.0);
            verts[2] = glm::vec4(l / 2, h / 2, b / 2, 1.0);
            verts[3] = glm::vec4(l / 2, -h / 2, b / 2, 1.0);

            for (int i = 0; i < 4; ++i) {
                verts[i + 4] = verts[i];
                verts[i + 4].z = -b / 2;
            }

            meshes.push_back(std::make_tuple(0, 1, 2));
            meshes.push_back(std::make_tuple(2, 3, 0));

            meshes.push_back(std::make_tuple(6, 5, 4));
            meshes.push_back(std::make_tuple(4, 7, 6));

            meshes.push_back(std::make_tuple(1, 5, 6));
            meshes.push_back(std::make_tuple(6, 2, 1));
            
            meshes.push_back(std::make_tuple(4, 0, 3));
            meshes.push_back(std::make_tuple(3, 7, 4));

            meshes.push_back(std::make_tuple(2, 6, 7));
            meshes.push_back(std::make_tuple(7, 3, 2));

            meshes.push_back(std::make_tuple(5, 1, 0));
            meshes.push_back(std::make_tuple(0, 4, 5));

            normals.resize(8);
            for (int i = 0; i < 8; ++i) {
                glm::vec3 p(verts[i]);
                normals[i] = glm::vec4(glm::normalize(p), 0);
            }

            colors.resize(8);
            colors[0] = glm::vec4(1, 1, 1, 1);
            colors[1] = glm::vec4(1, 1, 1, 1);
            colors[2] = glm::vec4(1, 1, 1, 1);
            colors[3] = glm::vec4(1, 1, 1, 1);
            colors[4] = glm::vec4(1, 1, 1, 1);
            colors[5] = glm::vec4(1, 1, 1, 1);
            colors[6] = glm::vec4(1, 1, 1, 1);
            colors[7] = glm::vec4(1, 1, 1, 1);
        }

    void push(Container & cont) {
        glm::mat4x4 mrot = glm::rotate(glm::mat4(1.0), rot, glm::vec3(0, 1, 0));
        model = glm::transpose(glm::mat4x4(
                    1, 0, 0, x,
                    0, 1, 0, y,
                    0, 0, 1, z,
                    0, 0, 0, 1
                    ));

        glm::mat4x4 modelmat = model * mrot;
        glm::mat4x4 normalmat = mrot;
        auto & mp = cont.program->uniforms_mat4;

        mp["mmodel"] = modelmat;
        mp["mnormal"] = normalmat;

        cont.program->attribPointer("position", &verts);
        cont.program->attribPointer("color", &colors);
        cont.program->attribPointer("normal", &normals);

        for (auto & i : meshes) {
            cont.addMesh(i);
        }

        cont.flush();

    }

    void update() {
        rot += .05;
    }

};


#endif
