#pragma once

#include "paint.h"

//
// Style gets inherited as we traverse the drawing.  We model this as a stack of
// 'style' values.
//
// I'm anticipating caching brushes in style, so we encapsulate the members.
//

class style
{
    paint fill_;
    paint stroke_;

public:
    style();

    void set_fill(paint value) { fill_ = value; }
    void set_stroke(paint value) { stroke_ = value; }

    ICanvasBrush^ fillBrush(ICanvasResourceCreator^ resourceCreator);
    ICanvasBrush^ strokeBrush(ICanvasResourceCreator^ resourceCreator);
};


class inherited_style
{
    std::stack<style> stack_;

public:
    inherited_style();

    void push();
    void pop();
    style* current() { return &stack_.top(); }
};