#ifndef COLOR_H
#define COLOR_H

struct Color {
    float r, g, b;
    Color(float mr = 0, float mg = 0, float mb = 0) :
        r(mr), g(mg), b(mb) { }
};
#endif
