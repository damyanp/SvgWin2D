#include "pch.h"

#include "svg.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::Foundation;
using namespace Windows::UI;

void container_element::draw(CanvasDrawingSession^ ds)
{
    for (auto const& child : elements_)
    {
        child->draw(ds);
    }
}


static float calculate_width_or_height(length svgLength, float destinationLength)
{
    switch (svgLength.Unit)
    {
    case unit::percent:
        return destinationLength * std::max(0.0f, std::min(100.0f, svgLength.Number)) / 100.0f;

    case unit::px:
        return svgLength.Number;

    default:
        return svgLength.Number; // TODO: all the other units!
    }
}


ICanvasImage^ svg::create_image(ICanvasResourceCreator^ resourceCreator, Size destinationSize)
{
    auto content = ref new CanvasCommandList(resourceCreator);
    auto ds = content->CreateDrawingSession();
    ds->Clear(Colors::Transparent);
    draw(ds);
    delete ds;

    auto crop = ref new Effects::CropEffect();

    auto width = calculate_width_or_height(width_, destinationSize.Width);
    auto height = calculate_width_or_height(height_, destinationSize.Height);

    crop->SourceRectangle = Rect{ 0, 0, width, height };
    crop->Source = content;

    return crop;
}


ICanvasBrush^ shape::fillBrush(ICanvasResourceCreator^ resourceCreator)
{
    return fillPaint_.brush(resourceCreator);
}


ICanvasBrush^ shape::strokeBrush(ICanvasResourceCreator^ resourceCreator)
{
    return strokePaint_.brush(resourceCreator);
}


void circle::draw(CanvasDrawingSession^ ds)
{
    auto fb = fillBrush(ds);
    if (fb)
        ds->FillCircle(cx_.Number, cy_.Number, radius_.Number, fb);

    auto sb = strokeBrush(ds);
    if (sb)
        ds->DrawCircle(cx_.Number, cy_.Number, radius_.Number, sb);
}