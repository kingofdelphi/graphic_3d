#ifndef FRAGMENT_SHADER_H
#define FRAGMENT_SHADER_H

#include <vector>
#include "primitives.h"
#include "glmdecl.h"

#include "shaderprogram.h"

class FragmentShader {
    public:
        ShaderProgram * program;
        glm::vec4 snormal;

        Vertex r_shade(Vertex fragment) {
            float w = fragment["position"].w;
            for (auto & i : fragment.attributes) {
                if (i.first != "position") i.second /= w;
            }
            fragment["position"].w = 1.0;
            return shade(fragment);
        }

        virtual Vertex shade(const Vertex & fragment) = 0;
};

#endif
