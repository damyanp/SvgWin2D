#include "pch.h"

#include "paint.h"

ICanvasBrush^ paint::brush(ICanvasResourceCreator^ resourceCreator) const
{
    switch (type_)
    {
    case paint_type::none:
        return nullptr;

    case paint_type::color:
        return ref new CanvasSolidColorBrush(resourceCreator, color_);

    default:
        return ref new CanvasSolidColorBrush(resourceCreator, Colors::HotPink); // TODO: the others!
    }
}
