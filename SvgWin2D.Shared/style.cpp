#include "pch.h"

#include "style.h"

using namespace Windows::Foundation;

style::style()
    : fill_(paint_type::color, Colors::Black)
    , stroke_(paint_type::none, Colors::Black)
{
}


ICanvasBrush^ style::fillBrush(ICanvasResourceCreator^ resourceCreator)
{
    return fill_.brush(resourceCreator);
}


ICanvasBrush^ style::strokeBrush(ICanvasResourceCreator^ resourceCreator)
{
    return stroke_.brush(resourceCreator);
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
