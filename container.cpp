#include "container.h"
#include <iostream>

Container::Container() 
    : display(nullptr) {
}

void Container::addMesh(const std::tuple<int, int, int> & mesh) {
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

    //fragment shader
    VertexShader * vshader = program->getVertexShader();
    FragmentShader * fshader = program->getFragmentShader();
    auto & unfms = program->uniforms_mat4;
    if (program->attribute_map.find("position") == program->attribute_map.end()) {
        std::cout << "no position specified";
        throw ;
    }
    for (auto & i : meshes) {
        int p = std::get<0>(i);
        int q = std::get<1>(i);
        int r = std::get<2>(i);
        auto & amap = program->attribute_map;
        Vertex a(amap["position"]->at(p));
        Vertex b(amap["position"]->at(q));
        Vertex c(amap["position"]->at(r));
        a.color = amap["color"]->at(p);
        b.color = amap["color"]->at(q);
        c.color = amap["color"]->at(r);

        a.normal = program->np->at(p);
        b.normal = program->np->at(q);
        c.normal = program->np->at(r);

        a = vshader->transform(a);
        b = vshader->transform(b);
        c = vshader->transform(c);

        float aw = a.pos.w;
        float bw = b.pos.w;
        float cw = c.pos.w;

        //move everything from eye space to screen space 
        a.pos /= aw;
        b.pos /= bw;
        c.pos /= cw;
        
        a.old_pos /= aw;
        b.old_pos /= bw;
        c.old_pos /= cw;

        a.color /= aw;
        b.color /= bw;
        c.color /= cw;

        a.normal /= aw;
        b.normal /= bw;
        c.normal /= cw;

        a.pos.w = 1 / aw;
        b.pos.w = 1 / bw;
        c.pos.w = 1 / cw;

        auto toscreen = [width, height](vec4 pos) {
            pos.x = (1 + pos.x) * width * .5;
            pos.y = (1 - pos.y) * height * .5;
            return pos;
        };

        a.pos = toscreen(a.pos);
        b.pos = toscreen(b.pos);
        c.pos = toscreen(c.pos);
        Mesh m(a, b, c);
        m.draw(*this);
    }

    clearRequests();

}
