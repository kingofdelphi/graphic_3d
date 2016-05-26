#ifndef VSHADER_H
#define VSHADER_H

#include "glmdecl.h"
#include "primitives.h"
#include <vector>
#include <iostream>

class VertexShader {
    private:
        glm::mat4x4 tmat;
        std::vector<Light> lights;
    public:

        void addLight(const Light & light) {
            lights.push_back(light);
        }

        void setTransformMatrix(const glm::mat4x4 & mat) {
            tmat = mat;
        }

        Vertex transform(const Vertex & v) {
            Vertex r(v);
            //lighting calculations
            glm::vec3 color(0, 0, 0);
            for (auto & i : lights) {
                float f = glm::dot(-i.normal, r.normal);
                if (f < 0) f = 0;
                float Cd = 0.6;
                glm::vec3 scale = i.color * Cd *f;
                color += scale;
            }
            r.color *= color;
            r.pos = tmat * r.pos;
            r.pos /= r.pos.w;
            return r;
        }
};

#endif
