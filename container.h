#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include <tuple>
#include "glmdecl.h"
#include "display.h"
#include "primitives.h"
#include "vshader.h"
#include "fragmentshader.h"
#include "shaderprogram.h"

enum CULL {BACKFACE, FRONTFACE, NONE};

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
        void addMesh(const std::tuple<int, int, int> & mesh);
        void flush();
        void clearRequests() {
            lines.clear();
            meshes.clear();
        }

        void enableCull(CULL type = BACKFACE) {
            cull = type;
        }

    //private:
        Display * display;
        std::vector<Line> lines;
        std::vector<std::tuple<int, int, int>> meshes;
        ShaderProgram * program;
        CULL cull;
};

#endif
