#ifndef FRAGMENT_SHADER_H
#define FRAGMENT_SHADER_H

#include <vector>
#include "primitives.h"
#include "glmdecl.h"
#include "shaderprogram.h"

class Container;

class FragmentShader {
    public:
        ShaderProgram * program;

        void r_shade(Vertex fragment, Container & cont);

        virtual Vertex shade(const Vertex & fragment) = 0;
};

#endif
