#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include "glmdecl.h"
#include "display.h"
#include "primitives.h"
#include "vshader.h"

class Container {
    public:
        Container();

        void clear() {
            lines.clear();
            meshes.clear();
        }

        void setVertexShader(const VertexShader & vsh) {
            vshader = vsh;
        }

        void addLine(const Line & line);
        void addMesh(const Mesh & mesh);
        void render(Display & disp);
    private:
        std::vector<Line> lines;
        std::vector<Mesh> meshes;
        VertexShader vshader;
};

#endif
