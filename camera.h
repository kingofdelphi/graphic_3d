#ifndef CAMERA_H
#define CAMERA_H

#include "glmdecl.h"

struct Camera {
    glm::vec3 pos;
    float yrot;
    Camera() : pos(glm::vec3(0, 0, 0)) { }
    glm::mat4x4 getViewMatrix() {
        glm::mat4x4 mrot = glm::rotate(glm::mat4(1.0), -yrot, glm::vec3(0, 1, 0));
        glm::mat4x4 model = glm::transpose(glm::mat4x4(
                    1, 0, 0, -pos.x,
                    0, 1, 0, -pos.y,
                    0, 0, 1, -pos.z,
                    0, 0, 0, 1
                    ));
        return mrot * model;
    }
    glm::mat4x4 getRotationMatrix() {
        glm::mat4x4 mrot = glm::rotate(glm::mat4(1.0), yrot, glm::vec3(0, 1, 0));
        return mrot;
    }
};
#endif
