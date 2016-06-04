#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H

#include "glmdecl.h"

struct DBuffer {

    size_t width, height;
    float ** buffer;

    DBuffer(size_t w, size_t h) : buffer(nullptr) {
        resize(w, h);
    }
    
    glm::vec2 todevice(float x, float y) {
        x = (1 + x) * width * .5;
        y = (1 - y) * height * .5;
        return glm::vec2(x, y);
    }

    void free() {
        if (buffer) {
            for (int i = 0; i < height; ++i) delete [] buffer[i];
            delete [] buffer;
            buffer = nullptr;
        }
    }

    void resize(size_t w, size_t h) {
        free();
        width = w;
        height = h;
        buffer = new float * [height];
        for (int i = 0; i < height; ++i) {
            buffer[i] = new float[width];
        }
    }
    
    bool inbounds(int x, int y) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return false;
        }
        return true;
    }

    void clear() {
        if (buffer == nullptr) {
            std::cout << "buffer not defined\n";
            throw ;
        }
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                buffer[i][j] = 1e100;
            }
        }
    }

    float get(int x, int y) {
        if (!inbounds(x, y)) {
            std::cout << "error not in bounds\n";
            throw ;
        }
        return buffer[y][x];
    }

    ~DBuffer() {
        free();
    }

};
#endif
