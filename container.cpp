#include "container.h"
#include <iostream>

Container::Container() 
    : fshader(nullptr), display(nullptr), vshader(nullptr) {
}

void Container::addMesh(const Mesh & mesh) {
    meshes.push_back(mesh);
}

void Container::addLine(const Line & line) {
    lines.push_back(line);
}

bool Container::clipLine(Line & line) {
    if (line.start.pos.w > line.finish.pos.w) 
        std::swap(line.start, line.finish);
    static const float z_clip = -1;
    //both endpoints are behind the clip plane 
    if (line.finish.pos.z < z_clip * line.finish.pos.w) return true;
    //both endpoints are at front the clip plane 
    if (line.start.pos.z >= z_clip * line.start.pos.w) return false;
    //need to clip
    //now interpolate
    glm::vec4 delta = line.finish.pos - line.start.pos;
    glm::vec4 dcolor = line.finish.color - line.start.color;
    glm::vec3 dnormal = line.finish.normal - line.start.normal;
    float K = delta.z / delta.w;
    assert(z_clip != K);
    float w = (line.start.pos.z - K * line.start.pos.w) / (z_clip - K);
    float factor = (w - line.start.pos.w) / delta.w;
    line.start.pos += factor * delta;
    line.start.color += factor * dcolor;
    line.start.normal += factor * dnormal;
    return false;
}

void Container::flush() {

    if (vshader == nullptr) throw "Error: Vertex Shader not specified";
    if (fshader == nullptr) throw "Error: Fragment Shader not specified";
    using namespace glm;

    //toscreen
    float w = display->getWidth();
    float h = display->getHeight();

    mat4x4 screen = transpose(mat4x4(
                w / 2, 0, 0, w / 2,
                0, -h / 2, 0, h / 2,
                0, 0, 1, 0,
                0, 0, 0, 1
                ));
    //for (auto & i : lines) {
    //    i.start = vshader->transform(i.start);
    //    i.finish = vshader->transform(i.finish);
    //    i.a.pos /= i.a.pos.w;
    //    i.b.pos /= i.b.pos.w;
    //    i.c.pos /= i.c.pos.w;

    //}

    //fragment shader
    fshader->clearLights();
    for (auto & i : vshader->getLights()) {
        fshader->addLight(i);
    }
    glm::mat4x4 invpers = glm::inverse(vshader->tmat);
    glm::mat4x4 invview = glm::inverse(vshader->mview);
    glm::mat4x4 invmodel = glm::inverse(vshader->mmodel);
    for (auto & i : meshes) {
        glm::vec3 va(vshader->mview * vshader->mmodel * i.a.pos);
        glm::vec3 vb(vshader->mview * vshader->mmodel * i.b.pos);
        glm::vec3 vc(vshader->mview * vshader->mmodel * i.c.pos);
        i.a = vshader->transform(i.a);
        i.b = vshader->transform(i.b);
        i.c = vshader->transform(i.c);
        glm::vec3 n = glm::normalize(glm::cross(vb - va, vc - va));
        fshader->snormal = glm::vec4(n, -glm::dot(n, va));
        i.a.pos.x /= -i.a.pos.z;
        i.b.pos.x /= -i.b.pos.z;
        i.c.pos.x /= -i.c.pos.z;

        i.a.pos.y /= -i.a.pos.z;
        i.b.pos.y /= -i.b.pos.z;
        i.c.pos.y /= -i.c.pos.z;
        
        i.a.color /= i.a.pos.z;
        i.b.color /= i.b.pos.z;
        i.c.color /= i.c.pos.z;

        i.a.normal /= i.a.pos.z;
        i.b.normal /= i.b.pos.z;
        i.c.normal /= i.c.pos.z;

        i.a.pos.z = 1 / i.a.pos.z;
        i.b.pos.z = 1 / i.b.pos.z;
        i.c.pos.z = 1 / i.c.pos.z;

        //i.a.pos.w = i.b.pos.w = i.c.pos.w = 1;
        //i.a.pos /= i.a.pos.w;
        //i.b.pos /= i.b.pos.w;
        //i.c.pos /= i.c.pos.w;
        i.a.pos = screen * i.a.pos;
        i.b.pos = screen * i.b.pos;
        i.c.pos = screen * i.c.pos;

        i.draw(*this);
    }

    //clipping
    //std::vector<Line> tmp = lines;
    //lines.clear();
    //for (auto & i : tmp) {
    //    bool discard = clipLine(i);
    //    if (!discard) {
    //        lines.push_back(i);
    //    }
    //}

    //perspective divide
    //for (auto & i : lines) {
    //    i.start.pos /= i.start.pos.w;
    //    i.finish.pos /= i.finish.pos.w;
    //}

    //for (auto & i : lines) {
    //    i.start.pos = screen * i.start.pos;
    //    i.finish.pos = screen * i.finish.pos;
    //    i.draw(*this);
    //}

    clearRequests();

}
