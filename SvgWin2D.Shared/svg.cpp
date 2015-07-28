#include "pch.h"

#include "svg.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::Foundation;
using namespace Windows::UI;


void element::draw(CanvasDrawingSession^ ds, inherited_style* s)
{
    s->push();
    apply_style(s->current());
    draw_element(ds, s);
    s->pop();
}


void element::apply_style(style* s)
{
    if (fillPaint_)
        s->set_fill(*fillPaint_);
    if (strokePaint_)
        s->set_stroke(*strokePaint_);
    if (strokeWidth_)
        s->set_stroke_width(*strokeWidth_);
}


void container_element::draw_element(CanvasDrawingSession^ ds, inherited_style* s)
{
    for (auto const& child : elements_)
    {
        child->draw(ds, s);
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

    draw_element(ds, std::make_unique<inherited_style>().get());

    delete ds;

    auto crop = ref new Effects::CropEffect();

    auto width = calculate_width_or_height(width_, destinationSize.Width);
    auto height = calculate_width_or_height(height_, destinationSize.Height);

    crop->SourceRectangle = Rect{ 0, 0, width, height };
    crop->Source = content;

    return crop;
}


void circle::draw_element(CanvasDrawingSession^ ds, inherited_style* s)
{
    auto fb = s->current()->fillBrush(ds);
    if (fb)
        ds->FillCircle(cx_.Number, cy_.Number, radius_.Number, fb);

    auto sb = s->current()->strokeBrush(ds);
    if (sb)
        ds->DrawCircle(cx_.Number, cy_.Number, radius_.Number, sb, s->current()->stroke_width());
}


void ellipse::draw_element(CanvasDrawingSession^ ds, inherited_style* s)
{
    auto fb = s->current()->fillBrush(ds);
    if (fb)
        ds->FillEllipse(cx_.Number, cy_.Number, rx_.Number, ry_.Number, fb);

    auto sb = s->current()->strokeBrush(ds);
    if (sb)
        ds->DrawEllipse(cx_.Number, cy_.Number, rx_.Number, ry_.Number, sb, s->current()->stroke_width());
}


void rect::draw_element(CanvasDrawingSession^ ds, inherited_style* s)
{
    Rect rect{ x_.Number, y_.Number, width_.Number, height_.Number };

    auto fb = s->current()->fillBrush(ds);
    if (fb)
    {
        if (rx_ && ry_)
            ds->FillRoundedRectangle(rect, rx_->Number, ry_->Number, fb);
        else if (rx_)
            ds->FillRoundedRectangle(rect, rx_->Number, rx_->Number, fb);
        else if (ry_)
            ds->FillRoundedRectangle(rect, ry_->Number, ry_->Number, fb);
        else
            ds->FillRectangle(rect, fb);
    }

    auto sb = s->current()->strokeBrush(ds);
    if (sb)
    {
        auto strokeWidth = s->current()->stroke_width();

        if (rx_ && ry_)
            ds->DrawRoundedRectangle(rect, rx_->Number, ry_->Number, sb, strokeWidth);
        else if (rx_)
            ds->DrawRoundedRectangle(rect, rx_->Number, rx_->Number, sb, strokeWidth);
        else if (ry_)
            ds->DrawRoundedRectangle(rect, ry_->Number, ry_->Number, sb, strokeWidth);
        else
            ds->DrawRectangle(rect, sb, strokeWidth);
    }
}


void line::draw_element(CanvasDrawingSession^ ds, inherited_style* s)
{
    auto sb = s->current()->strokeBrush(ds);
    if (sb)
    {
        auto strokeWidth = s->current()->stroke_width();

        ds->DrawLine(x1_.Number, y1_.Number, x2_.Number, y2_.Number, sb, strokeWidth);
    }
}