#include "pch.h"

#include "font.h"
#include "parse.h"


//
// font_size
//

font_size::font_size(type t)
    : type_(t)
{
}


font_size::font_size(length size)
    : type_(type::length)
    , size_(size)
{
}


font_size::font_size(Platform::String^ str)
    : type_(parse_type(str))
    , size_(parse_length(str, length{}))
{
}


/*static*/
font_size::type font_size::parse_type(Platform::String^ str)
{
    if (str == L"xx-small") return type::xx_small;
    if (str == L"x-small") return type::x_small;
    if (str == L"small") return type::small;
    if (str == L"medium") return type::medium;
    if (str == L"large") return type::large;
    if (str == L"x-large") return type::x_large;
    if (str == L"xx-large") return type::xx_large;
    if (str == L"larger") return type::larger;
    if (str == L"smaller") return type::smaller;
    return type::length;
}


/*static*/
float font_size::absolute_size_power(type t)
{
    switch (t)
    {
    case type::xx_small: return -3;
    case type::x_small:  return -2;
    case type::small:    return -1;
    case type::medium:   return  0;
    case type::large:    return  1;
    case type::x_large:  return  2;
    case type::xx_large: return  3;

    case type::larger:
    case type::smaller:
    case type::length:
    default:
        assert(false);
        return 0;
    }
}


void font_size::apply_to(CanvasTextFormat^ format) const
{
    float calculatedSize;

    switch (type_)
    {
    case type::larger:
    case type::smaller:
        // these values should never appear in a computed style since 
        // the inheritance rules should have resolved it when the
        // style was set
        assert(false);

    case type::length:
        calculatedSize = size_.Number;
        break;

    default:
        calculatedSize = 16.0f * std::pow(1.2f, absolute_size_power(type_));
        break;
    }

    format->FontSize = calculatedSize;
}


font_size font_size::get_inherited(font_size const& parentValue)
{
    switch (type_)
    {
    case type::larger:
        return font_size(size_ * 1.2f);

    case type::smaller:
        return font_size(size_ / 1.2f);

    default:
        return font_size(*this);
    }
}
