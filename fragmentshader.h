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
            float w = fragment.pos.w;
            fragment.color /= w;
            fragment.normal /= w;
            fragment.old_pos /= w;
            fragment.pos.w = 1.0;
            return shade(fragment);
        }

        virtual Vertex shade(const Vertex & fragment) = 0;
};

#endif
