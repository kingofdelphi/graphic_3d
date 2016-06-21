#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "../lib/include/glmdecl.h"
#include "../lib/include/container.h"
#include <tuple>
#include <vector>

struct Object {
    std::vector<glm::vec4> verts;
    std::vector<glm::vec4> colors;
    std::vector<std::tuple<int, int, int>> meshes;
    std::vector<glm::vec4> normals;
    float rot;
    Object() : rot(0) { }

    void update() {
        rot += .01;
    }

    void draw(Container & cont) {
        auto & mp = cont.program->uniforms_mat4;
        glm::mat4x4 model = glm::transpose(glm::mat4x4(
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, -1.5,
                    0, 0, 0, 1
                    ));
        glm::mat4x4 mrot = glm::rotate(glm::mat4(1.0), rot, glm::vec3(0, 1, 0));
        mp["mmodel"] = model * mrot;
        mp["mnormal"] = mrot * glm::mat4x4(1.0);
        mp["PVM"] = mp["PV"] * mp["mmodel"];

        cont.program->attribPointer("position", &verts);
        cont.program->attribPointer("color", &colors);
        cont.program->attribPointer("normal", &normals);

        for (auto & i : meshes) cont.addMesh(i);
        cont.flush();
    }

    void load(const std::string & file) {
        std::ifstream fp(file);
        std::string s;
        std::vector<int> faces;
        while (getline(fp, s)) {
            if (s.empty()) continue;
            std::istringstream f(s);
            std::string ch;
            f >> ch;
            if (ch[0] == 'v') {
                float a, b, c;
                f >> a >> b >> c;
                a *= .5;
                b *= .5;
                c *= .5;
                verts.push_back(glm::vec4(c, b, a, 1.0));
            } else if (ch[0] == 'f' ) {
                int a, b, c;
                f >> a >> b >> c;
                a--; b--; c--;
                faces.push_back(a);
                faces.push_back(b);
                faces.push_back(c);
                meshes.push_back(std::make_tuple(a, b, c));
            }
        }
        normals.resize(verts.size(), glm::vec4(0, 0, 0, 0));
        for (int i = 0; i < faces.size(); i += 3) {
            glm::vec3 pa(verts[faces[i]]);
            glm::vec3 pb(verts[faces[i + 1]]);
            glm::vec3 pc(verts[faces[i + 2]]);
            glm::vec4 norm(glm::cross(pa - pb, pc - pb), 0);
            normals[faces[i]] += norm;
            normals[faces[i + 1]] += norm;
            normals[faces[i + 2]] += norm;
        }
        for (auto & i : normals) i = glm::normalize(i);

        //colors
        colors.resize(verts.size());
        for (int i = 0; i < verts.size(); ++i) {
            float r = rand() * 1.0 / RAND_MAX;
            float g = rand() * 1.0 / RAND_MAX;
            float b = rand() * 1.0 / RAND_MAX;
            r = 1; g = 1; b = 1;
            colors[i] = glm::vec4(r, g, b, 1);
        }
    }
};

#endif
