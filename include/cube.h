#ifndef CUBE_H
#define CUBE_H

#include "../lib/include/container.h"
#include "../lib/include/glmdecl.h"

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
            std::vector<glm::vec3> vertices(8);
            vertices[0] = glm::vec3(-l / 2, -h / 2, b / 2);
            vertices[1] = glm::vec3(-l / 2, h / 2, b / 2);
            vertices[2] = glm::vec3(l / 2, h / 2, b / 2);
            vertices[3] = glm::vec3(l / 2, -h / 2, b / 2);

            for (int i = 0; i < 4; ++i) {
                vertices[i + 4] = vertices[i];
                vertices[i + 4].z = -b / 2;
            }

            std::vector<std::tuple<int, int, int>> mesh;
            mesh.push_back(std::make_tuple(0, 1, 2));
            mesh.push_back(std::make_tuple(2, 3, 0));

            mesh.push_back(std::make_tuple(6, 5, 4));
            mesh.push_back(std::make_tuple(4, 7, 6));

            mesh.push_back(std::make_tuple(1, 5, 6));
            mesh.push_back(std::make_tuple(6, 2, 1));
            
            mesh.push_back(std::make_tuple(4, 0, 3));
            mesh.push_back(std::make_tuple(3, 7, 4));

            mesh.push_back(std::make_tuple(2, 6, 7));
            mesh.push_back(std::make_tuple(7, 3, 2));

            mesh.push_back(std::make_tuple(5, 1, 0));
            mesh.push_back(std::make_tuple(0, 4, 5));

            std::vector<glm::vec4> color(8);
            color[0] = glm::vec4(1, 1, 1, 1);
            color[1] = glm::vec4(1, 1, 1, 1);
            color[2] = glm::vec4(1, 1, 1, 1);
            color[3] = glm::vec4(1, 1, 1, 1);
            color[4] = glm::vec4(1, 1, 1, 1);
            color[5] = glm::vec4(1, 1, 1, 1);
            color[6] = glm::vec4(1, 1, 1, 1);
            color[7] = glm::vec4(1, 1, 1, 1);

            for (auto & i : mesh) {
                int a = std::get<0>(i);
                int b = std::get<1>(i);
                int c = std::get<2>(i);
                int p = verts.size();
                verts.push_back(glm::vec4(vertices[a], 1));
                verts.push_back(glm::vec4(vertices[b], 1));
                verts.push_back(glm::vec4(vertices[c], 1));

                colors.push_back(color[a]);
                colors.push_back(color[b]);
                colors.push_back(color[c]);
                
                glm::vec4 normal(glm::cross(vertices[a] - vertices[b], vertices[c] - vertices[b]), 0.f);
                normals.push_back(normal);
                normals.push_back(normal);
                normals.push_back(normal);

                meshes.push_back(std::make_tuple(p, p + 1, p + 2));
            }

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
        
        mp["PVM"] = mp["PV"] * modelmat;
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
