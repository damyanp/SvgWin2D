#pragma once

#include "font.h"
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
    font_family fontFamily_;
    font_size fontSize_;

public:
    style();

    void set(
        std::unique_ptr<paint> const& color,
        std::unique_ptr<paint> const& fill,
        std::unique_ptr<paint> const& stroke,
        std::unique_ptr<length> const& strokeWidth,
        std::unique_ptr<font_family> const& fontFamily,
        std::unique_ptr<font_size> const& fontSize);

    ICanvasBrush^ fillBrush(ICanvasResourceCreator^ resourceCreator);
    ICanvasBrush^ strokeBrush(ICanvasResourceCreator^ resourceCreator);
    float stroke_width();
    CanvasTextFormat^ text_format();

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