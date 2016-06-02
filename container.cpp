#include "container.h"
#include <iostream>

Container::Container() 
    : display(nullptr) {
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
    if (program == nullptr) throw "Error: Shader program not specified";

    using namespace glm;

    //toscreen
    float width = display->getWidth();
    float height = display->getHeight();

    //for (auto & i : lines) {
    //    i.start = vshader->transform(i.start);
    //    i.finish = vshader->transform(i.finish);
    //    i.a.pos /= i.a.pos.w;
    //    i.b.pos /= i.b.pos.w;
    //    i.c.pos /= i.c.pos.w;

    //}
    
    //fragment shader
    VertexShader * vshader = program->getVertexShader();
    FragmentShader * fshader = program->getFragmentShader();
    auto & unfms = program->uniforms_mat4;
    for (auto & i : meshes) {
        //move this code later**************************************
        glm::vec3 va(unfms["mview"] * unfms["mmodel"] * i.a.pos);
        glm::vec3 vb(unfms["mview"] * unfms["mmodel"] * i.b.pos);
        glm::vec3 vc(unfms["mview"] * unfms["mmodel"] * i.c.pos);

        glm::vec4 la(unfms["pers"] * unfms["world_to_light"] * unfms["mmodel"] * i.a.pos);
        glm::vec4 lb(unfms["pers"] * unfms["world_to_light"] * unfms["mmodel"] * i.b.pos);
        glm::vec4 lc(unfms["pers"] * unfms["world_to_light"] * unfms["mmodel"] * i.c.pos);

        i.a = vshader->transform(i.a);
        i.b = vshader->transform(i.b);
        i.c = vshader->transform(i.c);
        i.a.old_pos = la;
        i.b.old_pos = lb;
        i.c.old_pos = lc;
        glm::vec3 n = glm::normalize(glm::cross(vb - va, vc - va));
        fshader->snormal = glm::vec4(n, -glm::dot(n, va));

        float aw = i.a.pos.w;
        float bw = i.b.pos.w;
        float cw = i.c.pos.w;

        //move everything from eye space to screen space 
        i.a.pos /= aw;
        i.b.pos /= bw;
        i.c.pos /= cw;
        
        i.a.old_pos /= aw;
        i.b.old_pos /= bw;
        i.c.old_pos /= cw;

        i.a.color /= aw;
        i.b.color /= bw;
        i.c.color /= cw;

        i.a.normal /= aw;
        i.b.normal /= bw;
        i.c.normal /= cw;

        i.a.pos.w = 1 / aw;
        i.b.pos.w = 1 / bw;
        i.c.pos.w = 1 / cw;

        auto toscreen = [width, height](vec4 pos) {
            pos.x = (1 + pos.x) * width * .5;
            pos.y = (1 - pos.y) * height * .5;
            return pos;
        };

        i.a.pos = toscreen(i.a.pos);
        i.b.pos = toscreen(i.b.pos);
        i.c.pos = toscreen(i.c.pos);

        i.draw(*this);
    }

    clearRequests();

}
