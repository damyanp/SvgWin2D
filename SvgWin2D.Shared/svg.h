#pragma once

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
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
    virtual void draw(CanvasDrawingSession^ ds) = 0;
};

typedef std::vector<std::unique_ptr<element>> element_vector;

class container_element : public element
{
    element_vector elements_;

public:
    void add_child(std::unique_ptr<element>&& child);

    virtual void draw(CanvasDrawingSession^ ds) override;
};


class svg : public container_element
{
    std::unique_ptr<viewBox> viewBox_;
    length width_;
    length height_;

public:
    svg(std::unique_ptr<viewBox>&& viewBox, length width, length height)
        : viewBox_(std::move(viewBox))
        , width_(width)
        , height_(height)
    {}

    ICanvasImage^ create_image(ICanvasResourceCreator^ resourceCreator, Size destinationSize);
};


class group : public container_element
{
    element_vector elements_;

public:
};

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

class shape : public element
{
    paint fillPaint_;
    paint strokePaint_;

protected:
    shape(paint const& fillPaint, paint const& strokePaint)
        : fillPaint_(fillPaint)
        , strokePaint_(strokePaint)
    {}

    ICanvasBrush^ fillBrush(ICanvasResourceCreator^ resourceCreator);
    ICanvasBrush^ strokeBrush(ICanvasResourceCreator^ resourceCreator);
};


class circle : public shape
{
    length cx_;
    length cy_;
    length radius_;

public:
    circle(paint const& fillPaint, paint const& strokePaint, length cx, length cy, length radius)
        : shape(fillPaint, strokePaint)
        , cx_(cx)
        , cy_(cy)
        , radius_(radius)
    {}

    virtual void draw(CanvasDrawingSession^ ds) override;
};
