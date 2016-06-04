#ifndef VSHADER_H
#define VSHADER_H

#include "glmdecl.h"
#include "shaderprogram.h"

class VertexShader {
    public:
        ShaderProgram * program;
    public:
        VertexShader() { }

        //transforms indexed vertex, vertices are specified in shader program
        virtual Vertex transform(size_t index) = 0;

};


#endif
