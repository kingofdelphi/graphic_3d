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
//    if (line.start.pos.w > line.finish.pos.w) 
//        std::swap(line.start, line.finish);
//    static const float z_clip = -1;
//    //both endpoints are behind the clip plane 
//    if (line.finish.pos.z < z_clip * line.finish.pos.w) return true;
//    //both endpoints are at front the clip plane 
//    if (line.start.pos.z >= z_clip * line.start.pos.w) return false;
//    //need to clip
//    //now interpolate
//    glm::vec4 delta = line.finish.pos - line.start.pos;
//    glm::vec4 dcolor = line.finish.color - line.start.color;
//    glm::vec3 dnormal = line.finish.normal - line.start.normal;
//    float K = delta.z / delta.w;
//    assert(z_clip != K);
//    float w = (line.start.pos.z - K * line.start.pos.w) / (z_clip - K);
//    float factor = (w - line.start.pos.w) / delta.w;
//    line.start.pos += factor * delta;
//    line.start.color += factor * dcolor;
//    line.start.normal += factor * dnormal;
//    return false;
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

        Vertex a, b, c;
        for (auto & i : program->attribute_map) {
            a.addAttribute(i.first, amap[i.first]->at(p));
            b.addAttribute(i.first, amap[i.first]->at(q));
            c.addAttribute(i.first, amap[i.first]->at(r));
        }

        a = vshader->transform(a);
        b = vshader->transform(b);
        c = vshader->transform(c);

        float aw = a["position"].w;
        float bw = b["position"].w;
        float cw = c["position"].w;

        //move everything from eye space to screen space 
        for (auto & j : a.attributes) {
            std::string k = j.first;
            a[k] /= aw;
            b[k] /= bw;
            c[k] /= cw;
        }
        
        a["position"].w = 1 / aw;
        b["position"].w = 1 / bw;
        c["position"].w = 1 / cw;

        auto toscreen = [width, height](vec4 pos) {
            pos.x = (1 + pos.x) * width * .5;
            pos.y = (1 - pos.y) * height * .5;
            return pos;
        };

        a["position"] = toscreen(a["position"]);
        b["position"] = toscreen(b["position"]);
        c["position"] = toscreen(c["position"]);

        Mesh m(a, b, c);
        m.draw(*this);
    }

    clearRequests();

}
