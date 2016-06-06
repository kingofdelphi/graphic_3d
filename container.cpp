#include "container.h"
#include <iostream>

Container::Container() 
    : display(nullptr), cull(NONE) {
}

void Container::addMesh(const std::tuple<int, int, int> & mesh) {
    meshes.push_back(mesh);
}

void Container::addLine(const std::pair<int, int> & line) {
    lines.push_back(line);
}

Vertex clip(Vertex a, const Vertex & b) {

    float w1 = a.attrs[0].w;
    float w2 = b.attrs[0].w;

    float z1 = a.attrs[0].z;
    float z2 = b.attrs[0].z;

    //the denominator can never be zero if n != f and z1 != z2 is true
    float w = (w1 * z2 - z1 * w2) / (w2 - w1 + z2 - z1);
    //w1 != w2 is true here
    float f = (w - w1) / (w2 - w1);
    glm::vec4 delta = b.attrs[0] - a.attrs[0];
    for (size_t i = 0; i < a.attrs.size(); ++i) {
        a.attrs[i] += f * (b.attrs[i] - a.attrs[i]);
    }
    return a;
}

std::vector<Mesh> Container::clipMesh(Mesh mesh) {
    Vertex * v[3] = {&mesh.a, &mesh.b, &mesh.c};
    std::vector<Mesh> res;
    int a = 0, b = 1, c = 2;
    bool swp = 0;
    if (v[a]->attrs[0].w > v[b]->attrs[0].w) {
        std::swap(a, b);
        swp ^= 1;
    }
    
    if (v[b]->attrs[0].w > v[c]->attrs[0].w) {
        std::swap(b, c);
        swp ^= 1;
    }

    if (v[a]->attrs[0].w > v[b]->attrs[0].w) {
        std::swap(a, b);
        swp ^= 1;
    }
    
    //both all points are behind the clip plane 
    if (v[c]->attrs[0].z <= -v[c]->attrs[0].w) {
        return res; //empty
    }

    //both all points are at the front of the clip plane 
    if (v[a]->attrs[0].z >= -v[a]->attrs[0].w) {
        res.push_back(mesh);
    } else
    //if two points are behind the near plane
    if (v[b]->attrs[0].z <= -v[b]->attrs[0].w) {
        *v[a] = clip(*v[a], *v[c]);
        *v[b] = clip(*v[b], *v[c]);
        res.push_back(mesh);
    } else { //if only one point is behind near plane
        Vertex v0 = clip(*v[a], *v[b]);
        Vertex v1 = clip(*v[a], *v[c]);
        *v[a] = v0;
        res.push_back(mesh);
        Mesh m(v0, *v[c], v1);
        if (swp) std::swap(m.a, m.b); //to preserve the order of vertices(normals are used for culling)
        res.push_back(m);
    }

    return res;
}

bool Container::clipLine(Line & line) {
    if (line.start.attrs[0].w > line.finish.attrs[0].w) 
        std::swap(line.start, line.finish);
    //both endpoints are behind the clip plane 
    if (line.finish.attrs[0].z <= -line.finish.attrs[0].w) return true;
    //both endpoints are at front the clip plane 
    if (line.start.attrs[0].z >= -line.start.attrs[0].w) return false;
    line.start = clip(line.start, line.finish);
    return false;
}

void Container::flush() {
    if (program == nullptr) throw "Error: Shader program not specified";

    using namespace glm;

    //toscreen
    float width = display->getZBuffer()->width;
    float height = display->getZBuffer()->height;

    auto toscreen = [width, height](vec4 pos) {
        pos.x = (1 + pos.x) * width * .5;
        pos.y = (1 - pos.y) * height * .5;
        return pos;
    };

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

        //fov clipping => z clipping
        auto res = clipMesh(Mesh(a, b, c));

        for (auto & mesh : res) {

            float aw = mesh.a.attrs[0].w;
            float bw = mesh.b.attrs[0].w;
            float cw = mesh.c.attrs[0].w;

            //move everything from eye space to screen space 
            for (size_t i = 0; i < mesh.a.attrs.size(); ++i) {
                mesh.a.attrs[i] /= aw;
                mesh.b.attrs[i] /= bw;
                mesh.c.attrs[i] /= cw;
            }

            //view port clipping begin
            float maxx = max(mesh.a.attrs[0].x, max(mesh.b.attrs[0].x,
                    mesh.c.attrs[0].x));

            float minx = min(mesh.a.attrs[0].x, min(mesh.b.attrs[0].x,
                    mesh.c.attrs[0].x));

            float maxy = max(mesh.a.attrs[0].y, max(mesh.b.attrs[0].y,
                    mesh.c.attrs[0].y));

            float miny = min(mesh.a.attrs[0].y, min(mesh.b.attrs[0].y,
                    mesh.c.attrs[0].y));

            if (maxx < -1 || minx > 1) continue;
            if (maxy < -1 || miny > 1) continue;

            //view port clipping end

            bool culled = false;

            if (cull != NONE) {//if culling is enabled
                //assumes vertices are specified in clockwise order

                float x1 = mesh.a.attrs[0].x;
                float y1 = mesh.a.attrs[0].y;

                float x2 = mesh.b.attrs[0].x;
                float y2 = mesh.b.attrs[0].y;

                float x3 = mesh.c.attrs[0].x;
                float y3 = mesh.c.attrs[0].y;

                float z = (x1 - x2) * (y3 - y2) - (y1 - y2) * (x3 - x2);

                if (cull == FRONTFACE) {
                    culled = z >= 0;
                } else if (cull == BACKFACE) {
                    culled = z <= 0;
                }

            }

            if (!culled) {

                //w = z, reverse z for perspective correction
                mesh.a.attrs[0].w = 1 / aw;
                mesh.b.attrs[0].w = 1 / bw;
                mesh.c.attrs[0].w = 1 / cw;

                mesh.a.attrs[0] = toscreen(mesh.a.attrs[0]);
                mesh.b.attrs[0] = toscreen(mesh.b.attrs[0]);
                mesh.c.attrs[0] = toscreen(mesh.c.attrs[0]);

                mesh.draw(*this);
            }

        }

    }

    for (auto & i : lines) {
        int p = i.first;
        int q = i.second;

        Vertex a = vshader->transform(p);
        Vertex b = vshader->transform(q);

        Line line(a, b);

        if (!clipLine(line)) {

            float aw = line.start.attrs[0].w;
            float bw = line.finish.attrs[0].w;

            //move everything from eye space to screen space 
            for (size_t i = 0; i < a.attrs.size(); ++i) {
                line.start.attrs[i] /= aw;
                line.finish.attrs[i] /= bw;
            }

            //viewport clipping
            float maxx = max(line.start.attrs[0].x, line.finish.attrs[0].x);

            float minx = min(line.start.attrs[0].x, line.finish.attrs[0].x);

            float maxy = max(line.start.attrs[0].y, line.finish.attrs[0].y);

            float miny = min(line.start.attrs[0].y, line.finish.attrs[0].y);

            if (maxx < -1 || minx > 1) continue;
            if (maxy < -1 || miny > 1) continue;

            //w = z, reverse z for perspective correction
            line.start.attrs[0].w = 1 / aw;
            line.finish.attrs[0].w = 1 / bw;

            line.start.attrs[0] = toscreen(line.start.attrs[0]);
            line.finish.attrs[0] = toscreen(line.finish.attrs[0]);

            line.draw(*this);
        }

    }

    clearRequests();

}
