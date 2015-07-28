#include "pch.h"

#include "svg.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::Foundation;
using namespace Windows::UI;


void element::apply_style(style* s)
{
    if (fillPaint_)
        s->set_fill(*fillPaint_);
    if (strokePaint_)
        s->set_stroke(*strokePaint_);
}


void container_element::draw(CanvasDrawingSession^ ds, inherited_style* s)
{
    s->push();

    apply_style(s->current());

    for (auto const& child : elements_)
    {
        child->draw(ds, s);
    }

    s->pop();
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

    draw(ds, std::make_unique<inherited_style>().get());

    delete ds;

    auto crop = ref new Effects::CropEffect();

    auto width = calculate_width_or_height(width_, destinationSize.Width);
    auto height = calculate_width_or_height(height_, destinationSize.Height);

    crop->SourceRectangle = Rect{ 0, 0, width, height };
    crop->Source = content;

    return crop;
}


void circle::draw(CanvasDrawingSession^ ds, inherited_style* s)
{
    s->push();
    apply_style(s->current());

    auto fb = s->current()->fillBrush(ds);
    if (fb)
        ds->FillCircle(cx_.Number, cy_.Number, radius_.Number, fb);

    auto sb = s->current()->strokeBrush(ds);
    if (sb)
        ds->DrawCircle(cx_.Number, cy_.Number, radius_.Number, sb);

    s->pop();
}