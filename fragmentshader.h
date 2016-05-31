#ifndef FRAGMENT_SHADER_H
#define FRAGMENT_SHADER_H

#include <vector>
#include "primitives.h"
#include "glmdecl.h"

class FragmentShader {
    protected:
        std::vector<Light> lights;
    public:
        glm::vec4 snormal;
        void addLight(const Light & light) {
            lights.push_back(light);
        }

        void clearLights() {
            lights.clear();
        }

        Vertex r_shade(Vertex fragment) {
            fragment.color /= fragment.pos.z;
            fragment.normal /= fragment.pos.z;
            fragment.pos.z = 1 / fragment.pos.z;
            return shade(fragment);
        }

        virtual Vertex shade(const Vertex & fragment) = 0;
};

#endif
