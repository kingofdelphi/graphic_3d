#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "glmdecl.h"
#include "container.h"

struct Helper {
    static void drawObj(Container & cont, const std::string & file) {
        std::ifstream fp(file);
        std::string s;
        std::vector<glm::vec4> verts;
        std::vector<int> faces;
        while (getline(fp, s)) {
            if (s.empty()) continue;
            std::istringstream f(s);
            std::string ch;
            f >> ch;
            if (ch[0] == 'v') {
                float a, b, c;
                f >> a >> b >> c;
                verts.push_back(glm::vec4(a, b, c, 1.0));
            } else if (ch[0] == 'f' ) {
                int a, b, c;
                f >> a >> b >> c;
                a--; b--; c--;
                faces.push_back(a);
                faces.push_back(b);
                faces.push_back(c);
            }
        }
        std::vector<glm::vec3> vertnorms(verts.size(), glm::vec3(0, 0, 0));
        for (int i = 0; i < faces.size(); i += 3) {
            glm::vec3 pa(verts[faces[i]]);
            glm::vec3 pb(verts[faces[i + 1]]);
            glm::vec3 pc(verts[faces[i + 2]]);
            glm::vec3 norm = glm::cross(pc - pb, pa - pb);
            vertnorms[faces[i]] += norm;
            vertnorms[faces[i + 1]] += norm;
            vertnorms[faces[i + 2]] += norm;
        }
        for (auto & i : vertnorms) i = glm::normalize(i);
        static float rot = 0;
        rot += .01;
        for (int i = 0; i < faces.size(); i += 3) {
            glm::vec4 va = verts[faces[i]];
            glm::vec4 vb = verts[faces[i + 1]];
            glm::vec4 vc = verts[faces[i + 2]];
            glm::mat4x4 mrot = glm::rotate(glm::mat4(1.0), rot, glm::vec3(0, 1, 0));
            va = mrot * va;
            vb = mrot * vb;
            vc = mrot * vc;
            va.z -= 2.5;
            vb.z -= 2.5;
            vc.z -= 2.5;
            glm::vec3 na(vertnorms[faces[i]]);
            glm::vec3 nb(vertnorms[faces[i + 1]]);
            glm::vec3 nc(vertnorms[faces[i + 2]]);
            na = glm::vec3(mrot * glm::vec4(na, 0));
            nb = glm::vec3(mrot * glm::vec4(nb, 0));
            nc = glm::vec3(mrot * glm::vec4(nc, 0));
            Vertex x(va, glm::vec4(1, 1, 1, 1), na);
            Vertex y(vb, glm::vec4(1, 1, 1, 1), nb);
            Vertex z(vc, glm::vec4(1, 1, 1, 1), nc);
            //cont.addLine(Line(x, y));
            //cont.addLine(Line(y, z));
            //cont.addLine(Line(z, x));
            cont.addMesh(Mesh(x, y, z));
            cont.flush();
        }
    }
};

#endif
