#ifndef VSHADER_H
#define VSHADER_H

#include "glmdecl.h"
#include "shaderprogram.h"

class VertexShader {
    public:
        ShaderProgram * program;
    public:
        VertexShader() { }

        virtual Vertex transform(const Vertex & v) = 0;

};


#endif
