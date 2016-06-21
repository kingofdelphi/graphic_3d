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

        std::vector<Mesh> clipMesh(Mesh mesh);

        void addLine(const std::pair<int, int> & line);

        void addMesh(const std::tuple<int, int, int> & mesh);

        void flush();

        void clearRequests() {
            lines.clear();
            meshes.clear();
        }

        void enableCull(CULL type = BACKFACE) {
            cull = type;
        }

        void setZBuffer(DBuffer * dbuf) {
            zbuffer = dbuf;
        }

        DBuffer * getZBuffer() {
            return zbuffer;
        }

        //private:
        Display * display;
        DBuffer * zbuffer;
        std::vector<std::pair<int, int>> lines;
        std::vector<std::tuple<int, int, int>> meshes;
        ShaderProgram * program;
        CULL cull;
};

#endif
