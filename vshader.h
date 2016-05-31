#ifndef VSHADER_H
#define VSHADER_H

#include "glmdecl.h"
#include "primitives.h"
#include <vector>
#include <iostream>

class VertexShader {
    public:
        glm::mat4x4 tmat;
        glm::mat4x4 invtmat;
        glm::mat4x4 mnormal;
        glm::mat4x4 view_normal;
        glm::mat4x4 mmodel;
        glm::mat4x4 mview;
        std::vector<Light> lights;
    public:
        VertexShader() { }

        void clearLights() {
            lights.clear();
        }

        void addLight(const Light & light) {
            lights.push_back(light);
        }

        std::vector<Light> getLights() const {
            return lights;
        }

        void setViewMatrix(const glm::mat4x4 & mat) {
            mview = mat;
        }

        void setTransformMatrix(const glm::mat4x4 & mat) {
            tmat = mat;
            //invtmat = glm::inverse(tmat);
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

        glm::mat4x4 & getViewMatrix() {
            return mview;
        }

        virtual Vertex transform(const Vertex & v) = 0;

};


#endif
