#include "shaderprogram.h"
#include "fragmentshader.h"
#include "vshader.h"

VertexShader * ShaderProgram::getVertexShader() {
    return vshader;
}

FragmentShader * ShaderProgram::getFragmentShader() {
    return fshader;
}

void ShaderProgram::init(VertexShader * vsh, FragmentShader * fsh) {
    fshader = fsh;
    fshader->program = this;
    vshader = vsh;
    vshader->program = this;
}
