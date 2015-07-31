#include "pch.h"

#include "style.h"

using namespace Windows::Foundation;

style::style()
    : color_(paint_type::color, Colors::Black)
    , fill_(paint_type::color, Colors::Black)
    , stroke_(paint_type::none, Colors::Black)
    , strokeWidth_(length{ 1, unit::unspecified })
    , fontFamily_(std::vector<std::wstring> { L"Arial" })
    , fontSize_(font_size::type::medium)
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

template<>
void style::set<font_size>(std::unique_ptr<font_size> const& value, font_size* field)
{
    if (!value)
        return;

    *field = value->get_inherited(*field);
}


void style::set(
    std::unique_ptr<paint> const& color,
    std::unique_ptr<paint> const& fill,
    std::unique_ptr<paint> const& stroke,
    std::unique_ptr<length> const& strokeWidth,
    std::unique_ptr<font_family> const& fontFamily,
    std::unique_ptr<font_size> const& fontSize)
{
    set(color, &color_);        // this must be set first so that paint_type::currentColor works correctly
    set(fill, &fill_);
    set(stroke, &stroke_);
    set(strokeWidth, &strokeWidth_);
    set(fontFamily, &fontFamily_);
    set(fontSize, &fontSize_);
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


CanvasTextFormat^ style::text_format()
{
    auto format = ref new CanvasTextFormat();
    format->VerticalAlignment = CanvasVerticalAlignment::Bottom;
    fontFamily_.apply_to(format);
    fontSize_.apply_to(format);
    return format;
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
