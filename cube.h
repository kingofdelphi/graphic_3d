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
    glm::vec4 colors[8];
    float x, y, z, l, b, h;
    float rot;
    glm::mat4x4 model;
    std::vector<std::tuple<int, int, int>> meshes;
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

            for (int i = 0; i < 8; ++i) {
                glm::vec3 p(verts[i]);
                normals[i] = glm::normalize(p);
            }

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
        //glm::mat4x4 normalmat = glm::transpose(glm::inverse(modelmat));
        glm::mat4x4 normalmat = mrot;
        auto & mp = cont.program->uniforms_mat4;
        mp["mmodel"] = modelmat;
        mp["mnormal"] = normalmat;

        for (int i = 0; i < 4; ++i) {
            int pa = i, pb = (i + 1) & 3;
            Vertex a(verts[pa], colors[pa], normals[pa]);
            Vertex b(verts[pb], colors[pb], normals[pb]);
            cont.addLine(Line(a, b));
        }

        for (int i = 0; i < 4; ++i) {
            int pa = i, pb = (i + 1) & 3;
            pa += 4; pb += 4;
            Vertex a(verts[pa], colors[pa], normals[pa]);
            Vertex b(verts[pb], colors[pb], normals[pb]);
            cont.addLine(Line(a, b));
        }

        for (int i = 0; i < 4; ++i) {
            int pa = i, pb = i + 4;
            Vertex a(verts[pa], colors[pa], normals[pa]);
            Vertex b(verts[pb], colors[pb], normals[pb]);
            cont.addLine(Line(a, b));
        }

        for (auto & i : meshes) {
            int a = std::get<0>(i);
            int b = std::get<1>(i);
            int c = std::get<2>(i);
            cont.addMesh(Mesh(Vertex(verts[a], colors[a], normals[a]), Vertex(verts[b], colors[b], normals[b]), Vertex(verts[c], colors[c], normals[c])));
        }

        cont.flush();

    }

    void update() {
        rot += .05;
    }

};


#endif
