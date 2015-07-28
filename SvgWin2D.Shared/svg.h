#pragma once

#include "paint.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation;
using namespace Windows::UI;

enum class unit
{
    em,
    ex,
    px,
    in,
    cm,
    mm,
    pt,
    pc,
    percent,
    unspecified
};

struct length
{
    float Number;
    unit Unit;

    bool operator==(length const& other) const
    {
        return Unit == other.Unit && Number == other.Number;
    }
};

struct viewBox
{
    float X;
    float Y;
    float Width;
    float Height;

    viewBox(float x, float y, float w, float h)
        : X(x), Y(y), Width(w), Height(h)
    {}
};

class element
{
public:
    element(IXmlNode^ node);

    virtual void draw(CanvasDrawingSession^ ds) = 0;
};

typedef std::vector<std::unique_ptr<element>> element_vector;

class container_element : public element
{
    element_vector elements_;

public:
    container_element(IXmlNode^ node);

    virtual void draw(CanvasDrawingSession^ ds) override;
};


class svg : public container_element
{
    std::unique_ptr<viewBox> viewBox_;
    length width_;
    length height_;

public:
    svg(IXmlNode^ node);

    ICanvasImage^ create_image(ICanvasResourceCreator^ resourceCreator, Size destinationSize);
};


class group : public container_element
{
    element_vector elements_;

public:
    group(IXmlNode^ node);
};


class shape : public element
{
    paint fillPaint_;
    paint strokePaint_;

protected:
    shape(IXmlNode^ node);

    ICanvasBrush^ fillBrush(ICanvasResourceCreator^ resourceCreator);
    ICanvasBrush^ strokeBrush(ICanvasResourceCreator^ resourceCreator);
};


class circle : public shape
{
    length cx_;
    length cy_;
    length radius_;

public:
    circle(IXmlNode^ node);

    virtual void draw(CanvasDrawingSession^ ds) override;
};
