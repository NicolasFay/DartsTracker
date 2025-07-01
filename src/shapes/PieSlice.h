#ifndef PIESLICE_H
#define PIESLICE_H

#include "shape.h"
#include "rect.h"
#include "circle.h"
#include "triangle.h"
#include "../shader/shader.h"
#include <memory>

using std::make_unique, std::unique_ptr;

class PieSlice {
private:
    // Each Pie Slice contains 1 triangle and 1 circle to make the end rounded
    // Store the shapes in a vector and use polymorphism
    // to draw
    vector<unique_ptr<Shape>> shapes;
public:
    // Constructors
    PieSlice();
    PieSlice(Shader& shader, vec2 pos);

    // Draw the cloud
    void setUniformsAndDraw() const;
/*
    // This will allow us to move the clouds left and right
    void moveXWithinBounds(int delta, const unsigned int width);
    */

    // Return true if r overlaps with the rectangle in the cloud
    // and false otherwise
    bool isOverlapping(const Rect& r) const;
};


#endif //PIESLICE_H
