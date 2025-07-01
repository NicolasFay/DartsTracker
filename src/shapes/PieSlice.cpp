#include "PieSlice.h"

PieSlice::PieSlice() {};

PieSlice::PieSlice(Shader& shader, vec2 pos) {
    shapes.push_back(make_unique<Circle>(shader, vec2(pos.x + 3, pos.y + 15), vec2(5, 5), vec2(-1, 0), color(1, 1, 1, 1)));
    shapes.push_back(make_unique<Circle>(shader, vec2(pos.x - 10, pos.y + 5), vec2(5, 5), vec2(-1, 0), color(1, 1, 1, 1)));
    shapes.push_back(make_unique<Circle>(shader, vec2(pos.x + 10, pos.y + 5), vec2(5, 5), vec2(-1, 0), color(1, 1, 1, 1)));
    shapes.push_back(make_unique<Rect>(shader, pos, vec2(15, 15), color(1, 1, 1, 1)));
}

void PieSlice::setUniformsAndDraw() const {
    for (const unique_ptr<Shape>& s : shapes) {
        s->setUniforms();
        s->draw();
    }
}
/*
void PieSlice::moveXWithinBounds(int delta, const unsigned int width) {
    for (const unique_ptr<Shape> &s: shapes) {
        s->moveX(delta);
        if (s->getPosX() < -20) {
            s->setPosX(width + 20);
        }
    }
}
*/
bool PieSlice::isOverlapping(const Rect& r) const {
    return r.isOverlapping(*shapes[3]);
}