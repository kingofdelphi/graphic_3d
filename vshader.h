#ifndef VSHADER_H
#define VSHADER_H

#include "glmdecl.h"
#include "primitives.h"
#include <vector>
#include <iostream>

class VertexShader {
    protected:
        glm::mat4x4 tmat;
        glm::mat4x4 mnormal;
        glm::mat4x4 mmodel;
        std::vector<Light> lights;
    public:
        VertexShader() : mnormal(glm::mat4x4(1)), mmodel(glm::mat4x4(1)) { }

        void clearLights() {
            lights.clear();
        }

        void addLight(const Light & light) {
            lights.push_back(light);
        }

        std::vector<Light> getLights() const {
            return lights;
        }

        void setTransformMatrix(const glm::mat4x4 & mat) {
            tmat = mat;
        }

        void setNormalMatrix(const glm::mat4x4 & mat) {
            mnormal = mat;
        }

        void setModelMatrix(const glm::mat4x4 & mat) {
            mmodel = mat;
        }

        glm::mat4x4 & getTransformMatrix() {
            return tmat;
        }

        virtual Vertex transform(const Vertex & v) = 0;

};


#endif
