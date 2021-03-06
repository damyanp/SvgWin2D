#pragma once

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

    length operator*(float n) const
    {
        return length{ Number * n, Unit };
    }

    length operator/(float n) const
    {
        return length{ Number / n, Unit };
    }
};

