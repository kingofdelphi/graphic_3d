#include "../lib/include/glib.h"

class ShadowVertexShader : public VertexShader {
    public:
        Vertex transform(size_t index) {
            Vertex r;
            auto & amap = program->attribute_map;
            auto & unfms = program->uniforms_mat4;

            //add attributes to the vertex
            //currently: position must go first, color must go second, rest can be any
            r.attrs.push_back(amap["position"]->at(index));

            //process attributes
            r.attrs[0] = unfms["PVM"] * r.attrs[0];
            return r;
        }
};

class GouraudVertexShader : public VertexShader {
    public:
        Vertex transform(size_t index) {
            Vertex r;
            auto & amap = program->attribute_map;
            auto & unfms = program->uniforms_mat4;
            //add attributes to the vertex
            //currently: position must go first, color must go second, rest can be any
            r.attrs.push_back(amap["position"]->at(index));
            r.attrs.push_back(amap["color"]->at(index));
            r.attrs.push_back(amap["normal"]->at(index));

            //process attributes
            glm::vec4 world = unfms["mmodel"] * r.attrs[0];
            r.attrs.push_back(unfms["world_to_light"] * world);//world space coords to light space homogeneous coords

            r.attrs.push_back(world); //vertex world space position
            
            r.attrs[0] = unfms["PVM"] * r.attrs[0];
            r.attrs[2] = unfms["mnormal"] * r.attrs[2];
            
            return r;
        }
};

class PassThroughFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            return fragment;
        }
};

class PhongFragmentShader : public FragmentShader {
    public:
        Vertex shade(const Vertex & fragment) {
            Vertex r(fragment);
            glm::vec4 pos = r.attrs[3]; //oldpos
            pos /= pos.w;
            auto & dbuffer = program->dbuffer;
            glm::vec2 dev = dbuffer->todevice(pos.x, pos.y);
            pos.x = dev.x;
            pos.y = dev.y;
            int X = int(pos.x);
            int Y = int(pos.y);
            float bias = 0.0004;
            float s = -1, e = 1;
            float notshadow = 0;
            for (int i = s; i <= e; ++i) {
                for (int j = s; j <= e; ++j) {
                    int AX = X + i, AY = Y + j;
                    if (dbuffer->inbounds(AX, AY)) {
                        if (pos.z - bias < dbuffer->get(AX, AY)) notshadow += 1;
                    }
                }
            }
            notshadow /= (e - s + 1) * (e - s  + 1); 
            glm::vec4 color(0.5, 0.5, 0.5, 1.0);
            glm::vec3 N = normalize(glm::vec3(r.attrs[2]));
            glm::vec3 ppos(r.attrs[4]);
            //lighting calculations
            for (auto & i : program->lights) {
                glm::vec3 norm(glm::normalize(glm::vec3(i.pos) - ppos));
                float f = dot(norm, N);
                if (f < 0) f = 0;
                float Cd = 0.7;
                glm::vec4 scale = i.color * Cd * f;
                color += notshadow * scale;
                //color += glm::vec4(0.5, 0.5, 0.5, 0) * notshadow;
            }
            r.attrs[1] *= color;
            r.attrs[1].x = std::min(std::max(r.attrs[1].x, 0.f), 1.f);
            r.attrs[1].y = std::min(std::max(r.attrs[1].y, 0.f), 1.f);
            r.attrs[1].z = std::min(std::max(r.attrs[1].z, 0.f), 1.f);
            return r;
        }
};

