#pragma once

#include "length.h"

#if defined small
#undef small
#endif

using namespace Microsoft::Graphics::Canvas::Text;

class font_family
{
    std::vector<std::wstring> families_;
    
public:
    font_family(std::vector<std::wstring>&& families)
        : families_(std::move(families))
    {
    }
    
    void apply_to(CanvasTextFormat^ format) const
    {
        if (families_.empty())
            return;
        
        format->FontFamily = ref new Platform::String(families_[0].c_str());
    }
};

class font_size
{
public:
    // medium = 16px
    enum class type { xx_small, x_small, small, medium, large, x_large, xx_large, larger, smaller, length };
    
private:
    type type_;
    length size_;

public:
    font_size(type t);
    font_size(length size);
    font_size(Platform::String^ str);

    void apply_to(CanvasTextFormat^ format) const;
    font_size get_inherited(font_size const& parentValue);

private:
    static float absolute_size_power(type t);
    static type parse_type(Platform::String^ string);
};