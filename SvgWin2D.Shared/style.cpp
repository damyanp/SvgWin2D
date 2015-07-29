#include "pch.h"

#include "style.h"

using namespace Windows::Foundation;

style::style()
    : color_(paint_type::color, Colors::Black)
    , fill_(paint_type::color, Colors::Black)
    , stroke_(paint_type::none, Colors::Black)
    , strokeWidth_(length{ 1, unit::unspecified })
{
}


template<typename T>
void style::set(std::unique_ptr<T> const& value, T* field)
{
    if (!value)
        return;

    *field = *value;
}

template<>
void style::set<paint>(std::unique_ptr<paint> const& value, paint* field)
{
    if (!value)
        return;

    switch (value->type())
    {
    case paint_type::inherit:
        break;

    case paint_type::currentColor:
        *field = color_;
        break;

    default:
        *field = *value;
        break;
    }
}


void style::set(
    std::unique_ptr<paint> const& color,
    std::unique_ptr<paint> const& fill,
    std::unique_ptr<paint> const& stroke,
    std::unique_ptr<length> const& strokeWidth)
{
    set(color, &color_);        // this must be set first so that paint_type::currentColor works correctly
    set(fill, &fill_);
    set(stroke, &stroke_);
    set(strokeWidth, &strokeWidth_);
}


ICanvasBrush^ style::fillBrush(ICanvasResourceCreator^ resourceCreator)
{
    return fill_.brush(resourceCreator);
}


ICanvasBrush^ style::strokeBrush(ICanvasResourceCreator^ resourceCreator)
{
    return stroke_.brush(resourceCreator);
}


float style::stroke_width()
{
    return strokeWidth_.Number; // TODO: respect the Unit!
}


inherited_style::inherited_style()
{
    style defaultStyle;
    stack_.push(defaultStyle);
}


void inherited_style::push()
{
    stack_.push(stack_.top());
}


void inherited_style::pop()
{
    stack_.pop();
    assert(!stack_.empty());
}
