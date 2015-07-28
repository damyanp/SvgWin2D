#pragma once

#include "length.h"
#include "paint.h"
#include "style.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation;
using namespace Windows::UI;

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
    std::unique_ptr<paint> fillPaint_;
    std::unique_ptr<paint> strokePaint_;
    std::unique_ptr<length> strokeWidth_;

public:
    element(IXmlNode^ node);

    virtual void draw(CanvasDrawingSession^ ds, inherited_style* s) = 0;

protected:
    void apply_style(style* s);
};

typedef std::vector<std::unique_ptr<element>> element_vector;

class container_element : public element
{
    element_vector elements_;

public:
    container_element(IXmlNode^ node);

    virtual void draw(CanvasDrawingSession^ ds, inherited_style* s) override;
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


class circle : public element
{
    length cx_;
    length cy_;
    length radius_;

public:
    circle(IXmlNode^ node);

    virtual void draw(CanvasDrawingSession^ ds, inherited_style* s) override;
};


class rect : public element
{
    length x_;
    length y_;
    length width_;
    length height_;
    std::unique_ptr<length> rx_;
    std::unique_ptr<length> ry_;

public:
    rect(IXmlNode^ node);

    virtual void draw(CanvasDrawingSession^ ds, inherited_style* s) override;
};
