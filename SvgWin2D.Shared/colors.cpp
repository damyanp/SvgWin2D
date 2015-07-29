#include "pch.h"

#include "colors.h"

// TODO: suport for 'system' colors
//
// See http://www.w3.org/TR/2008/REC-CSS2-20080411/ui.html#system-colors
// and http://www.w3.org/Graphics/SVG/Test/20110816/harness/htmlObjectApproved/color-prop-04-t.html


using Windows::UI::Color;

struct Entry
{
    wchar_t const* Name;
    Color Value;
};

bool operator<(Entry const& left, Entry const& right)
{
    return _wcsicmp(left.Name, right.Name) < 0;
}

bool operator<(Entry const& left, Platform::String^ right)
{
    return _wcsicmp(left.Name, right->Data()) < 0;
}

bool operator<(Platform::String^ left, Entry const& right)
{
    return _wcsicmp(left->Data(), right.Name) < 0;
}

static Entry gColors[] =
{
    { L"aliceblue"            , Color{ 255, 240, 248, 255 } },
    { L"antiquewhite"         , Color{ 255, 250, 235, 215 } },
    { L"aqua"                 , Color{ 255,  0, 255, 255 } },
    { L"aquamarine"           , Color{ 255, 127, 255, 212 } },
    { L"azure"                , Color{ 255, 240, 255, 255 } },
    { L"beige"                , Color{ 255, 245, 245, 220 } },
    { L"bisque"               , Color{ 255, 255, 228, 196 } },
    { L"black"                , Color{ 255,  0, 0, 0 } },
    { L"blanchedalmond"       , Color{ 255, 255, 235, 205 } },
    { L"blue"                 , Color{ 255,  0, 0, 255 } },
    { L"blueviolet"           , Color{ 255, 138, 43, 226 } },
    { L"brown"                , Color{ 255, 165, 42, 42 } },
    { L"burlywood"            , Color{ 255, 222, 184, 135 } },
    { L"cadetblue"            , Color{ 255,  95, 158, 160 } },
    { L"chartreuse"           , Color{ 255, 127, 255, 0 } },
    { L"chocolate"            , Color{ 255, 210, 105, 30 } },
    { L"coral"                , Color{ 255, 255, 127, 80 } },
    { L"cornflowerblue"       , Color{ 255, 100, 149, 237 } },
    { L"cornsilk"             , Color{ 255, 255, 248, 220 } },
    { L"crimson"              , Color{ 255, 220, 20, 60 } },
    { L"cyan"                 , Color{ 255,  0, 255, 255 } },
    { L"darkblue"             , Color{ 255,  0, 0, 139 } },
    { L"darkcyan"             , Color{ 255,  0, 139, 139 } },
    { L"darkgoldenrod"        , Color{ 255, 184, 134, 11 } },
    { L"darkgray"             , Color{ 255, 169, 169, 169 } },
    { L"darkgreen"            , Color{ 255,  0, 100, 0 } },
    { L"darkgrey"             , Color{ 255, 169, 169, 169 } },
    { L"darkkhaki"            , Color{ 255, 189, 183, 107 } },
    { L"darkmagenta"          , Color{ 255, 139, 0, 139 } },
    { L"darkolivegreen"       , Color{ 255,  85, 107, 47 } },
    { L"darkorange"           , Color{ 255, 255, 140, 0 } },
    { L"darkorchid"           , Color{ 255, 153, 50, 204 } },
    { L"darkred"              , Color{ 255, 139, 0, 0 } },
    { L"darksalmon"           , Color{ 255, 233, 150, 122 } },
    { L"darkseagreen"         , Color{ 255, 143, 188, 143 } },
    { L"darkslateblue"        , Color{ 255,  72, 61, 139 } },
    { L"darkslategray"        , Color{ 255,  47, 79, 79 } },
    { L"darkslategrey"        , Color{ 255,  47, 79, 79 } },
    { L"darkturquoise"        , Color{ 255,  0, 206, 209 } },
    { L"darkviolet"           , Color{ 255, 148, 0, 211 } },
    { L"deeppink"             , Color{ 255, 255, 20, 147 } },
    { L"deepskyblue"          , Color{ 255,  0, 191, 255 } },
    { L"dimgray"              , Color{ 255, 105, 105, 105 } },
    { L"dimgrey"              , Color{ 255, 105, 105, 105 } },
    { L"dodgerblue"           , Color{ 255,  30, 144, 255 } },
    { L"firebrick"            , Color{ 255, 178, 34, 34 } },
    { L"floralwhite"          , Color{ 255, 255, 250, 240 } },
    { L"forestgreen"          , Color{ 255,  34, 139, 34 } },
    { L"fuchsia"              , Color{ 255, 255, 0, 255 } },
    { L"gainsboro"            , Color{ 255, 220, 220, 220 } },
    { L"ghostwhite"           , Color{ 255, 248, 248, 255 } },
    { L"gold"                 , Color{ 255, 255, 215, 0 } },
    { L"goldenrod"            , Color{ 255, 218, 165, 32 } },
    { L"gray"                 , Color{ 255, 128, 128, 128 } },
    { L"green"                , Color{ 255,  0, 128, 0 } },
    { L"greenyellow"          , Color{ 255, 173, 255, 47 } },
    { L"grey"                 , Color{ 255, 128, 128, 128 } },
    { L"honeydew"             , Color{ 255, 240, 255, 240 } },
    { L"hotpink"              , Color{ 255, 255, 105, 180 } },
    { L"indianred"            , Color{ 255, 205, 92, 92 } },
    { L"indigo"               , Color{ 255,  75, 0, 130 } },
    { L"ivory"                , Color{ 255, 255, 255, 240 } },
    { L"khaki"                , Color{ 255, 240, 230, 140 } },
    { L"lavender"             , Color{ 255, 230, 230, 250 } },
    { L"lavenderblush"        , Color{ 255, 255, 240, 245 } },
    { L"lawngreen"            , Color{ 255, 124, 252, 0 } },
    { L"lemonchiffon"         , Color{ 255, 255, 250, 205 } },
    { L"lightblue"            , Color{ 255, 173, 216, 230 } },
    { L"lightcoral"           , Color{ 255, 240, 128, 128 } },
    { L"lightcyan"            , Color{ 255, 224, 255, 255 } },
    { L"lightgoldenrodyellow" , Color{ 255, 250, 250, 210 } },
    { L"lightgray"            , Color{ 255, 211, 211, 211 } },
    { L"lightgreen"           , Color{ 255, 144, 238, 144 } },
    { L"lightgrey"            , Color{ 255, 211, 211, 211 } },
    { L"lightpink"            , Color{ 255, 255, 182, 193 } },
    { L"lightsalmon"          , Color{ 255, 255, 160, 122 } },
    { L"lightseagreen"        , Color{ 255,  32, 178, 170 } },
    { L"lightskyblue"         , Color{ 255, 135, 206, 250 } },
    { L"lightslategray"       , Color{ 255, 119, 136, 153 } },
    { L"lightslategrey"       , Color{ 255, 119, 136, 153 } },
    { L"lightsteelblue"       , Color{ 255, 176, 196, 222 } },
    { L"lightyellow"          , Color{ 255, 255, 255, 224 } },
    { L"lime"                 , Color{ 255,  0, 255, 0 } },
    { L"limegreen"            , Color{ 255,  50, 205, 50 } },
    { L"linen"                , Color{ 255, 250, 240, 230 } },
    { L"magenta"              , Color{ 255, 255, 0, 255 } },
    { L"maroon"               , Color{ 255, 128, 0, 0 } },
    { L"mediumaquamarine"     , Color{ 255, 102, 205, 170 } },
    { L"mediumblue"           , Color{ 255,  0, 0, 205 } },
    { L"mediumorchid"         , Color{ 255, 186, 85, 211 } },
    { L"mediumpurple"         , Color{ 255, 147, 112, 219 } },
    { L"mediumseagreen"       , Color{ 255,  60, 179, 113 } },
    { L"mediumslateblue"      , Color{ 255, 123, 104, 238 } },
    { L"mediumspringgreen"    , Color{ 255,  0, 250, 154 } },
    { L"mediumturquoise"      , Color{ 255,  72, 209, 204 } },
    { L"mediumvioletred"      , Color{ 255, 199, 21, 133 } },
    { L"midnightblue"         , Color{ 255,  25, 25, 112 } },
    { L"mintcream"            , Color{ 255, 245, 255, 250 } },
    { L"mistyrose"            , Color{ 255, 255, 228, 225 } },
    { L"moccasin"             , Color{ 255, 255, 228, 181 } },
    { L"navajowhite"          , Color{ 255, 255, 222, 173 } },
    { L"navy"                 , Color{ 255,  0, 0, 128 } },
    { L"oldlace"              , Color{ 255, 253, 245, 230 } },
    { L"olive"                , Color{ 255, 128, 128, 0 } },
    { L"olivedrab"            , Color{ 255, 107, 142, 35 } },
    { L"orange"               , Color{ 255, 255, 165, 0 } },
    { L"orangered"            , Color{ 255, 255, 69, 0 } },
    { L"orchid"               , Color{ 255, 218, 112, 214 } },
    { L"palegoldenrod"        , Color{ 255, 238, 232, 170 } },
    { L"palegreen"            , Color{ 255, 152, 251, 152 } },
    { L"paleturquoise"        , Color{ 255, 175, 238, 238 } },
    { L"palevioletred"        , Color{ 255, 219, 112, 147 } },
    { L"papayawhip"           , Color{ 255, 255, 239, 213 } },
    { L"peachpuff"            , Color{ 255, 255, 218, 185 } },
    { L"peru"                 , Color{ 255, 205, 133, 63 } },
    { L"pink"                 , Color{ 255, 255, 192, 203 } },
    { L"plum"                 , Color{ 255, 221, 160, 221 } },
    { L"powderblue"           , Color{ 255, 176, 224, 230 } },
    { L"purple"               , Color{ 255, 128, 0, 128 } },
    { L"red"                  , Color{ 255, 255, 0, 0 } },
    { L"rosybrown"            , Color{ 255, 188, 143, 143 } },
    { L"royalblue"            , Color{ 255,  65, 105, 225 } },
    { L"saddlebrown"          , Color{ 255, 139, 69, 19 } },
    { L"salmon"               , Color{ 255, 250, 128, 114 } },
    { L"sandybrown"           , Color{ 255, 244, 164, 96 } },
    { L"seagreen"             , Color{ 255,  46, 139, 87 } },
    { L"seashell"             , Color{ 255, 255, 245, 238 } },
    { L"sienna"               , Color{ 255, 160, 82, 45 } },
    { L"silver"               , Color{ 255, 192, 192, 192 } },
    { L"skyblue"              , Color{ 255, 135, 206, 235 } },
    { L"slateblue"            , Color{ 255, 106, 90, 205 } },
    { L"slategray"            , Color{ 255, 112, 128, 144 } },
    { L"slategrey"            , Color{ 255, 112, 128, 144 } },
    { L"snow"                 , Color{ 255, 255, 250, 250 } },
    { L"springgreen"          , Color{ 255,  0, 255, 127 } },
    { L"steelblue"            , Color{ 255,  70, 130, 180 } },
    { L"tan"                  , Color{ 255, 210, 180, 140 } },
    { L"teal"                 , Color{ 255,  0, 128, 128 } },
    { L"thistle"              , Color{ 255, 216, 191, 216 } },
    { L"tomato"               , Color{ 255, 255, 99, 71 } },
    { L"turquoise"            , Color{ 255,  64, 224, 208 } },
    { L"violet"               , Color{ 255, 238, 130, 238 } },
    { L"wheat"                , Color{ 255, 245, 222, 179 } },
    { L"white"                , Color{ 255, 255, 255, 255 } },
    { L"whitesmoke"           , Color{ 255, 245, 245, 245 } },
    { L"yellow"               , Color{ 255, 255, 255, 0 } },
    { L"yellowgreen"          , Color{ 255, 154, 205, 50 } }
};


bool ensure_colors_sorted()
{
    std::sort(gColors, gColors + _countof(gColors));
    return true;
}


Color get_color_by_name(Platform::String^ name)
{
    static bool f = ensure_colors_sorted();

    auto begin = gColors;
    auto end = gColors + _countof(gColors);
        
    auto color = std::lower_bound(begin, end, name);

    if (color != end && !(name<*color))
        return color->Value;
    
    // not found
    return Color{ 255, 0, 0, 0 };
}