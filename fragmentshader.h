#ifndef FRAGMENT_SHADER_H
#define FRAGMENT_SHADER_H

#include <vector>
#include "primitives.h"
#include "glmdecl.h"

class FragmentShader {
    protected:
        std::vector<Light> lights;
    public:
        void addLight(const Light & light) {
            lights.push_back(light);
        }

        void clearLights() {
            lights.clear();
        }

        virtual Vertex shade(const Vertex & fragment) = 0;
};

#endif
