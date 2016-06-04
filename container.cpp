#include "container.h"
#include <iostream>

Container::Container() 
    : display(nullptr), cull(NONE) {
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
    float width = display->getZBuffer()->width;
    float height = display->getZBuffer()->height;

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

        Vertex a = vshader->transform(p);
        Vertex b = vshader->transform(q);
        Vertex c = vshader->transform(r);

        float aw = a.attrs[0].w;
        float bw = b.attrs[0].w;
        float cw = c.attrs[0].w;

        //move everything from eye space to screen space 
        for (size_t i = 0; i < a.attrs.size(); ++i) {
            a.attrs[i] /= aw;
            b.attrs[i] /= bw;
            c.attrs[i] /= cw;
        }

        bool culled = false;

        if (cull != NONE) {//if culling is enabled
            //assumes vertices are specified in clockwise order
           
            float x1 = a.attrs[0].x;
            float y1 = a.attrs[0].y;

            float x2 = b.attrs[0].x;
            float y2 = b.attrs[0].y;

            float x3 = c.attrs[0].x;
            float y3 = c.attrs[0].y;

            float z = (x1 - x2) * (y3 - y2) - (y1 - y2) * (x3 - x2);

            if (cull == FRONTFACE) {
                culled = z >= 0;
            } else if (cull == BACKFACE) {
                culled = z <= 0;
            }

        }

        if (!culled) {
            //w = z, reverse z for perspective correction
            a.attrs[0].w = 1 / aw;
            b.attrs[0].w = 1 / bw;
            c.attrs[0].w = 1 / cw;

            auto toscreen = [width, height](vec4 pos) {
                pos.x = (1 + pos.x) * width * .5;
                pos.y = (1 - pos.y) * height * .5;
                return pos;
            };

            a.attrs[0] = toscreen(a.attrs[0]);
            b.attrs[0] = toscreen(b.attrs[0]);
            c.attrs[0] = toscreen(c.attrs[0]);

            Mesh(a, b, c).draw(*this);
        }
    }

    clearRequests();

}
