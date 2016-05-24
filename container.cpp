#include "container.h"

Container::Container() {
}

void Container::addLine(const Line & line) {
    lines.push_back(line);
}

void Container::render(Display & disp) {
    for (auto & i : lines) {
        i.draw(disp);
    }
}
