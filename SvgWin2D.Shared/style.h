#pragma once

#include "length.h"
#include "paint.h"

//
// Style gets inherited as we traverse the drawing.  We model this as a stack of
// 'style' values.
//
// I'm anticipating caching brushes in style, so we encapsulate the members.
//

class style
{
    paint color_;               // (used for paint_type::currentColor)
    paint fill_;
    paint stroke_;
    length strokeWidth_;

public:
    style();

    void set(
        std::unique_ptr<paint> const& color,
        std::unique_ptr<paint> const& fill,
        std::unique_ptr<paint> const& stroke,
        std::unique_ptr<length> const& strokeWidth);

    ICanvasBrush^ fillBrush(ICanvasResourceCreator^ resourceCreator);
    ICanvasBrush^ strokeBrush(ICanvasResourceCreator^ resourceCreator);
    float stroke_width();

private:
    template<typename T>
    void set(std::unique_ptr<T> const& value, T* field);
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