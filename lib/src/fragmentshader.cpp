#include "../include/fragmentshader.h"
#include "../include/container.h"

void FragmentShader::r_shade(Vertex fragment, Container & cont) {

    float w = fragment.attrs[0].w;

    for (size_t i = 1; i < fragment.attrs.size(); ++i) {
        fragment.attrs[i] /= w;
    }

    fragment.attrs[0].w = 1.0;

    //perform depth test before doing the actual fragment shading
    float x = fragment.attrs[0].x, y = fragment.attrs[0].y, z = fragment.attrs[0].z;

    int X = (int)(round(x)), Y = (int)(round(y));

    if (cont.zbuffer->inbounds(X, Y) && 
            z + .00001 < cont.zbuffer->buffer[Y][X]) {
        cont.zbuffer->buffer[Y][X] = z;
        cont.display->drawFragment(shade(fragment));
    }

}
