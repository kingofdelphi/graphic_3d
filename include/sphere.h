#ifndef SPHERE_H
#define SPHERE_H

#include "../lib/include/container.h"
#include "../lib/include/glmdecl.h"

#include <iostream>
#include <tuple>
#include <vector>

struct Sphere {
    //vertices are in clockwise order
    std::vector<glm::vec4> verts;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> line_colors;
    float x, y, z, radius;
    float rot;
    glm::mat4x4 model;
    std::vector<std::tuple<int, int, int>> meshes;
    std::vector<std::pair<int, int>> lines;
    Sphere(float cx, float cy, float cz, float cradius) : 
        x(cx), y(cy), z(cz), radius(cradius), rot(0) {
            std::vector<glm::vec3> vertices;
            int vert_steps = 20;
            int horz_steps = 20;
            int N = vert_steps + 1; //vertical vertices
            glm::vec4 sphereColor(0, 1, 0, 0);
            glm::vec4 lcolor(1, 1, 1, 0);

            glm::mat4x4 vrot = glm::rotate(glm::mat4(1.0), -(float)M_PI / vert_steps, glm::vec3(1, 0, 0));
            glm::mat4x4 hrot = glm::rotate(glm::mat4(1.0), 2 * (float)M_PI / horz_steps, glm::vec3(0, 1, 0));

            for (int i = 0, cur = 0; i <= horz_steps; i++) {
                glm::vec3 p(0, radius, 0);
                for (int j = 0; j < N; j++, cur++) {
                    if (i == 0) {
                        vertices.push_back(p);
                        p = glm::vec3(vrot * glm::vec4(p, 1));
                    } else {
                        vertices.push_back(glm::vec3(hrot * glm::vec4(vertices[cur - N], 1)));
                    }
                    verts.push_back(glm::vec4(vertices[cur], 1));
                    colors.push_back(sphereColor); 
                    line_colors.push_back(lcolor); 
                    normals.push_back(glm::vec4(glm::normalize(vertices[cur]), 0));
                }
            }

            std::vector<std::tuple<int, int, int>> mesh;

            for (int i = 0, cur = 0; i < horz_steps; i++) {
                for (int j = 0; j < vert_steps; j++, cur++) {
                    meshes.push_back(std::make_tuple(cur, cur + N, cur + 1));
                    meshes.push_back(std::make_tuple(cur + 1, cur + N, cur + 1 + N));
                }
                cur++;
            }

            //lines for the sphere, i want to overlay on top of the meshes
            for (int i = 0, cur = 0; i < horz_steps; i++) {
                for (int j = 0; j < vert_steps; j++, cur++) {
                    lines.push_back(std::make_pair(cur, cur + 1));
                }
                cur++;
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

        //cont.program->attribPointer("color", &line_colors);
        //for (auto & i : lines) {
        //    cont.addLine(i);
        //}

        //cont.flush();

    }

    void update() {
        rot += .05;
    }

};


#endif
