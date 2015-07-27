#pragma once

class viewBox
{
    float x_;
    float y_;
    float width_;
    float height_;

public:
    viewBox(float x, float y, float width, float height)
        : x_(x)
        , y_(y)
        , width_(width)
        , height_(height)
    {}

    float x() { return x_; }
    float y() { return y_; }
    float width() { return width_; }
    float height() { return height_; }
};


class svg
{
    std::unique_ptr<viewBox> viewBox_;

public:
    svg(std::unique_ptr<viewBox>&& viewBox)
        : viewBox_(std::move(viewBox))
    {}
};
