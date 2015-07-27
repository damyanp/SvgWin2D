#include "pch.h"

#include "svg.h"

float calculate_width_or_height(length svgLength, float destinationLength)
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


float svg::calculate_width(float destinationWidth)
{
    return calculate_width_or_height(width_, destinationWidth);
}


float svg::calculate_height(float destinationHeight)
{
    return calculate_width_or_height(height_, destinationHeight);
}