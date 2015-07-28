#pragma once

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Windows::UI;

enum class paint_type
{
    none,
    currentColor,
    color,
    iri,
    inherit
};

class paint
{
    paint_type type_;
    Color color_;
    Platform::String^ iri_;
    paint_type fallbackType_;

public:
    paint(paint_type type, Color color, Platform::String^ iri = nullptr, paint_type fallbackType = paint_type::none)
        : type_(type)
        , color_(color)
        , iri_(iri)
        , fallbackType_(fallbackType)
    {}

    ICanvasBrush^ brush(ICanvasResourceCreator^ resourceCreator);
};
