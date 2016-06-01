#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include "glmdecl.h"
#include "display.h"
#include "primitives.h"
#include "vshader.h"
#include "fragmentshader.h"
#include "shaderprogram.h"

class Container {
    public:
        Container();

        void setDisplay(Display * disp) {
            display = disp;
        }

        void clear() {
            lines.clear();
            meshes.clear();
        }

        void setProgram(ShaderProgram * prog) {
            program = prog;
        }

        bool clipLine(Line & line);
        void addLine(const Line & line);
        void addMesh(const Mesh & mesh);
        void flush();
        void clearRequests() {
            lines.clear();
            meshes.clear();
        }
    //private:
        Display * display;
        std::vector<Line> lines;
        std::vector<Mesh> meshes;
        ShaderProgram * program;
};

#endif
