#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <map>
#include <vector>
#include <iostream>
#include "primitives.h"
#include "glmdecl.h"

class FragmentShader;
class VertexShader;

struct ShaderProgram {
    FragmentShader * fshader;
    VertexShader * vshader;
    std::map<std::string, glm::mat4x4> uniforms_mat4;
    std::map<std::string, glm::vec4> uniforms_vec4;
    std::map<std::string, std::vector<glm::vec4>*> attribute_map;
    std::vector<glm::vec3>* np;
    std::vector<Light> lights;

    void init(VertexShader * vsh, FragmentShader * fsh);

    void normalAttribPointer(std::vector<glm::vec3> * alist) {
        np = alist;
    }

    void attribPointer(std::string attr, std::vector<glm::vec4> * alist) {
        attribute_map[attr] = alist;
    }

    void clearLights() {
        lights.clear();
    }

    void addLight(const Light & light) {
        lights.push_back(light);
    }

    std::vector<Light> getLights() const {
        return lights;
    }

    VertexShader * getVertexShader();
    FragmentShader * getFragmentShader();

};

#endif
