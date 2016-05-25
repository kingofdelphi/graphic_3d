#include "container.h"
#include <iostream>

Container::Container() {
}

void Container::addLine(const Line & line) {
    lines.push_back(line);
}

void Container::render(Display & disp) {
    using namespace glm;
    const float l = -1, r = 1, t = 1, b = -1;
    const float n = 1.0, f = 100.0;
    mat4x4 pers = transpose(mat4x4(
                2 * n / (r - l), 0, (r + l) / (r - l), 0,
                0, 2 * n / (t - b), (t + b) / (t - b), 0,
                0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
                0, 0, -1, 0
                ));
    //projection
    for (auto & i : lines) {
        i.start = pers * i.start;
        i.finish = pers * i.finish;
        i.start /= i.start.w;
        i.finish /= i.finish.w;
    }
    //toscreen
    float w = disp.getWidth();
    float h = disp.getHeight();
    mat4x4 screen = transpose(mat4x4(
                w / 2, 0, 0, w / 2,
                0, -h / 2, 0, h / 2,
                0, 0, 1, 0,
                0, 0, 0, 1
                ));

    for (auto & i : lines) {
        i.start = screen * i.start;
        i.finish = screen * i.finish;
        i.draw(disp);
    }

}
