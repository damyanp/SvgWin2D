#pragma once

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::Foundation;

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
};

typedef std::vector<std::unique_ptr<element>> element_vector;

class container_element : public element
{
    element_vector elements_;

public:
    void add_child(std::unique_ptr<element>&& child);
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

    ICanvasImage^ create_image(Size destinationSize);
};

class group : public container_element
{
    element_vector elements_;

public:
};
