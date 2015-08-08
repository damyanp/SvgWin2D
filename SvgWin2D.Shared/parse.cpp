#include "pch.h"

#include "colors.h"
#include "parse.h"
#include "path_parser.h"
#include "transform_parser.h"

using namespace Windows::Data::Xml::Dom;

Platform::String^ SVG_NS = L"http://www.w3.org/2000/svg";
Platform::String^ XMLNS_SVG = L"xmlns:svg='http://www.w3.org/2000/svg'";

std::wregex gTokenRegex(L"(-?[0-9.]+)\\s*,?\\s*");

class list_parser
{
    Platform::String^ toParse_;
    std::regex_token_iterator<const wchar_t*> it_;
    
public:
    list_parser(Platform::String^ toParse)
        : toParse_(toParse)
        , it_(toParse->Begin(), toParse->Begin() + toParse->Length(), gTokenRegex)
    {
    }

    bool try_get_next(float* value)
    {
        if (it_ == std::regex_token_iterator<const wchar_t*>())
            return false;

        *value = static_cast<float>(_wtof(it_->str().c_str()));
        ++it_;

        return true;
    }
};


std::unique_ptr<viewBox> parse_viewBox(Platform::String^ viewBoxString)
{
    float x, y, w, h;

    list_parser parser(viewBoxString);

    if (!(parser.try_get_next(&x) 
        && parser.try_get_next(&y) 
        && parser.try_get_next(&w) 
        && parser.try_get_next(&h)))
    {
        return nullptr;
    }

    return std::make_unique<viewBox>(x, y, w, h);
}


std::unique_ptr<viewBox> parse_viewBox(IXmlNode^ element)
{
    auto attribute = element->Attributes->GetNamedItem(L"viewBox");
    if (!attribute)
        return nullptr;

    auto viewBoxString = dynamic_cast<Platform::String^>(attribute->NodeValue);
    if (!viewBoxString)
        return nullptr;

    return parse_viewBox(viewBoxString);
}


preserveAspectRatio::preserveAspectRatio(Platform::String^ attributeString)
    : Defer(false)
    , Align(align::xMidYMid)
    , Slice(false)
{
    if (!attributeString)
        return;
}


Platform::String^ get_attribute(IXmlNode^ element, Platform::String^ name)
{
    auto attribute = element->Attributes->GetNamedItem(name);
    if (!attribute)
        return nullptr;

    auto attributeString = dynamic_cast<Platform::String^>(attribute->NodeValue);
    if (!attributeString)
        return nullptr;

    return attributeString;
}


std::wregex gLengthRegex(L"(-?[0-9.]+)(em|ex|px|in|cm|mm|pt|pc|%)?");

length parse_length(Platform::String^ lengthString, length defaultLength)
{
    std::wcmatch match;

    if (!std::regex_match(lengthString->Data(), lengthString->Data() + lengthString->Length(), match, gLengthRegex))
        return defaultLength;

    float number = static_cast<float>(_wtof(match[1].str().c_str()));

    if (!match[2].matched)
        return length{ number, unit::unspecified };
   
    auto unitString = match[2].str();

    static std::pair<std::wstring, unit> unitNames[] =
    { 
        { L"em", unit::em },
        { L"ex", unit::ex },
        { L"px", unit::px },
        { L"in", unit::in },
        { L"cm", unit::cm },
        { L"mm", unit::mm },
        { L"pt", unit::pt },
        { L"pc", unit::pc },
        { L"%",  unit::percent }
    };
    
    for (auto const& unitName : unitNames)
    {
        if (unitName.first == unitString)
            return length{ number, unitName.second };
    }
    
    return defaultLength;
}


length parse_length(IXmlNode^ element, Platform::String^ name, length defaultLength)
{
    auto attributeString = get_attribute(element, name);
    if (!attributeString)
        return defaultLength;

    return parse_length(attributeString, defaultLength);
}


length parse_width_or_height(IXmlNode^ element, Platform::String^ name)
{
    return parse_length(element, name, length{ 100, unit::percent });
}


length parse_coordinate(IXmlNode^ element, Platform::String^ name)
{
    return parse_length(element, name, length{ 0, unit::unspecified });
}

std::unique_ptr<length> parse_optional_coordinate(IXmlNode^ element, Platform::String^ name)
{
    auto lengthString = get_attribute(element, name);

    if (!lengthString)
        return nullptr;

    return std::make_unique<length>(parse_length(lengthString, length{ 0, unit::unspecified }));
}


std::unique_ptr<length> parse_stroke_width(IXmlNode^ element)
{
    auto lengthString = get_attribute(element, L"stroke-width");

    if (!lengthString)
        return nullptr;

    if (lengthString == L"inherit")
        return nullptr;

    return std::make_unique<length>(parse_length(lengthString, length{ 0, unit::unspecified }));
}


// hex colors are either #XXX or #XXXXXX where 'X' is a hex digit
std::wregex gHexColorRegex(L"#([[:xdigit:]]{3}|[[:xdigit:]]{6})", std::wregex::icase);
std::wregex gIntColorRegex(L"rgb\\([[:space:]]*([0-9]+)[,[:space:]]*([0-9]+)[,[:space:]]*([0-9]+)[[:space:]]*\\)", std::wregex::icase);
std::wregex gPercentColorRegex(L"rgb\\([[:space:]]*([0-9.]+)%[,[:space:]]*([0-9.]+)%[,[:space:]]*([0-9.]+)%[[:space:]]*\\)", std::wregex::icase);

std::unique_ptr<paint> parse_paint(IXmlNode^ element, Platform::String^ name)
{
    auto attributeString = get_attribute(element, name);

    if (!attributeString)
        return nullptr;

    if (attributeString == L"none")
        return std::make_unique<paint>(paint_type::none, Colors::HotPink);

    if (attributeString == L"currentColor")
        return std::make_unique<paint>(paint_type::currentColor, Colors::HotPink);

    if (attributeString == L"inherit")
        return std::make_unique<paint>(paint_type::inherit, Colors::HotPink);

    auto stringBegin = attributeString->Data();
    auto stringEnd = stringBegin + attributeString->Length();

    std::wcmatch match;

    // Hex colors (#FFF or #FFFFFF)
    if (std::regex_match(stringBegin, stringEnd, match, gHexColorRegex))
    {
        auto hexColor = match[1].str();
        std::wstring rs, gs, bs;
        if (hexColor.length() == 3)
        {
            rs.assign(2, hexColor[0]);
            gs.assign(2, hexColor[1]);
            bs.assign(2, hexColor[2]);
        }
        else
        {
            assert(hexColor.length() == 6);
            rs.assign(hexColor.begin(), hexColor.begin()+2);
            gs.assign(hexColor.begin()+2, hexColor.begin()+4);
            bs.assign(hexColor.begin()+4, hexColor.begin()+6);
        }
        
        auto r = static_cast<uint8_t>(std::stoul(rs, nullptr, 16));
        auto g = static_cast<uint8_t>(std::stoul(gs, nullptr, 16));
        auto b = static_cast<uint8_t>(std::stoul(bs, nullptr, 16));

        return std::make_unique<paint>(paint_type::color, Color{ 255, r, g, b });
    }
    
    // Int rgb (rgb(1,2,3))
    if (std::regex_match(stringBegin, stringEnd, match, gIntColorRegex))
    {
        auto r = static_cast<uint8_t>(std::stoul(match[1].str(), nullptr, 10));
        auto g = static_cast<uint8_t>(std::stoul(match[2].str(), nullptr, 10));
        auto b = static_cast<uint8_t>(std::stoul(match[3].str(), nullptr, 10));

        return std::make_unique<paint>(paint_type::color, Color{ 255, r, g, b });
    }

    // Percentage rgb (rgb(15.1, 12.3, 99.9))
    if (std::regex_match(stringBegin, stringEnd, match, gPercentColorRegex))
    {
        auto rPercent = _wtof(match[1].str().c_str());
        auto gPercent = _wtof(match[2].str().c_str());
        auto bPercent = _wtof(match[3].str().c_str());

        auto r = static_cast<uint8_t>(255.0 * std::min(100.0, std::max(0.0, rPercent)) / 100.0);
        auto g = static_cast<uint8_t>(255.0 * std::min(100.0, std::max(0.0, gPercent)) / 100.0);
        auto b = static_cast<uint8_t>(255.0 * std::min(100.0, std::max(0.0, bPercent)) / 100.0);

        return std::make_unique<paint>(paint_type::color, Color{ 255, r, g, b });
    }
    
    return std::make_unique<paint>(paint_type::color, get_color_by_name(attributeString));
}


std::vector<point> parse_points(IXmlNode^ node)
{
    std::vector<point> points;

    auto str = get_attribute(node, L"points");

    list_parser parser(str);

    float x, y;

    while (parser.try_get_next(&x))
    {
        if (!parser.try_get_next(&y))
            break;

        points.push_back(std::make_pair(x,y));
    }

    return points;
}


std::unique_ptr<float3x2> parse_transform(IXmlNode^ node)
{
    auto str = get_attribute(node, L"transform");

    if (!str)
        return nullptr;

    return std::make_unique<float3x2>(transform_parser::parse(str));
}


CanvasGeometry^ parse_path_data(ICanvasResourceCreator^ resourceCreator, IXmlNode^ node)
{
    auto str = get_attribute(node, L"d");

    if (!str)
        return nullptr;

    return path_parser::parse(resourceCreator, str);
}


std::vector<std::wstring> parse_comma_separated_list(Platform::String^ str)
{
    static std::wregex entryRegex(
        L"[[:space:]]*"         // ignore whitespace prefix
        L",?"                   // ignore comma prefix
        L"[[:space:]]*"         // 
        L"(?:"                  // 
        L"(?:\"([^\"]*)\")|"    // a quoted string
        L"([^,]*[^,[:space:]])" // a normal string
        L"[[:space:]]*"         // 
        L")"
        );

    std::vector<std::wstring> entries;

    typedef std::regex_iterator<wchar_t const*> regex_iterator;

    for (regex_iterator it(str->Begin(), str->End(), entryRegex); it != regex_iterator(); ++it)
    {
        auto match = *it;
        if (match[1].matched)
            entries.push_back(match[1].str());
        else if (match[2].matched)
            entries.push_back(match[2].str());
    }

    return entries;
}


std::unique_ptr<font_family> parse_font_family(IXmlNode^ node)
{
    auto str = get_attribute(node, L"font-family");

    if (!str)
        return nullptr;

    if (str == L"inherit")
        return nullptr;

    auto names = parse_comma_separated_list(str);

    return std::make_unique<font_family>(std::move(names));
}


std::unique_ptr<font_size> parse_font_size(IXmlNode^ node)
{
    auto str = get_attribute(node, L"font-size");

    if (!str)
        return nullptr;

    if (str == L"inherit")
        return nullptr;

    return std::make_unique<font_size>(str);
}


std::unique_ptr<element> parse_any_element(ICanvasResourceCreator^ resourceCreator, IXmlNode^ node)
{
    auto fullName = node->NodeName;
    auto localName = node->LocalName;
    auto nsuri = node->NamespaceUri;

    auto nsuri_uri = dynamic_cast<Windows::Foundation::Uri^>(nsuri);
    auto nsuri_str = dynamic_cast<Platform::String^>(nsuri);
    

    if (dynamic_cast<Platform::String^>(node->NamespaceUri) != SVG_NS)
        return nullptr;

    auto name = dynamic_cast<Platform::String^>(node->LocalName);

    if (name == L"svg")
        return std::make_unique<svg>(resourceCreator, node);
    else if (name == L"g")
        return std::make_unique<group>(resourceCreator, node);
    else if (name == L"circle")
        return std::make_unique<circle>(node);
    else if (name == L"rect")
        return std::make_unique<rect>(node);
    else if (name == L"ellipse")
        return std::make_unique<ellipse>(node);
    else if (name == L"line")
        return std::make_unique<line>(node);
    else if (name == L"polyline")
        return std::make_unique<polyline>(node);
    else if (name == L"polygon")
        return std::make_unique<polygon>(node);
    else if (name == L"path")
        return std::make_unique<path>(resourceCreator, node);
    else if (name == L"text")
        return std::make_unique<text>(node);
    else
        return nullptr;
}



std::unique_ptr<svg> parse_svg(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument)
{
    auto svgElement = svgDocument->SelectSingleNodeNS("svg:svg", XMLNS_SVG);
    return std::make_unique<svg>(resourceCreator, svgElement);
}


//
// The various elements construct construct themselves from the XML nodes.
//


element::element(IXmlNode^ node)
    : color_(parse_paint(node, L"color"))
    , fillPaint_(parse_paint(node, L"fill"))
    , strokePaint_(parse_paint(node, L"stroke"))
    , strokeWidth_(parse_stroke_width(node))
    , transform_(parse_transform(node))
    , fontFamily_(parse_font_family(node))
    , fontSize_(parse_font_size(node))
{
}


container_element::container_element(ICanvasResourceCreator^ resourceCreator, IXmlNode^ node)
    : element(node)
{
    for (auto const& childNode : node->ChildNodes)
    {
        if (childNode->NodeType != NodeType::ElementNode)
            continue;

        auto childElement = parse_any_element(resourceCreator, childNode);

        if (childElement)
            elements_.push_back(std::move(childElement));
    }
}


svg::svg(ICanvasResourceCreator^ resourceCreator, IXmlNode^ node)
    : container_element(resourceCreator, node)
    , viewBox_(parse_viewBox(node))
    , preserveAspectRatio_(get_attribute(node, L"preserveAspectRatio"))
    , width_(parse_width_or_height(node, L"width"))
    , height_(parse_width_or_height(node, L"height"))
{
}


group::group(ICanvasResourceCreator^ resourceCreator, IXmlNode^ node)
    : container_element(resourceCreator, node)
{
}


circle::circle(IXmlNode^ node)
    : element(node)
    , cx_(parse_coordinate(node, L"cx"))
    , cy_(parse_coordinate(node, L"cy"))
    , radius_(parse_coordinate(node, L"r"))
{
}


ellipse::ellipse(IXmlNode^ node)
    : element(node)
    , cx_(parse_coordinate(node, L"cx"))
    , cy_(parse_coordinate(node, L"cy"))
    , rx_(parse_coordinate(node, L"rx"))
    , ry_(parse_coordinate(node, L"ry"))
{
}


rect::rect(IXmlNode^ node)
    : element(node)
    , x_(parse_coordinate(node, L"x"))
    , y_(parse_coordinate(node, L"y"))
    , width_(parse_coordinate(node, L"width"))
    , height_(parse_coordinate(node, L"height"))
    , rx_(parse_optional_coordinate(node, L"rx"))
    , ry_(parse_optional_coordinate(node, L"ry"))
{
}


line::line(IXmlNode^ node)
    : element(node)
    , x1_(parse_coordinate(node, L"x1"))
    , y1_(parse_coordinate(node, L"y1"))
    , x2_(parse_coordinate(node, L"x2"))
    , y2_(parse_coordinate(node, L"y2"))
{
}


polything::polything(IXmlNode^ node)
    : element(node)
    , points_(parse_points(node))
{
}


polyline::polyline(IXmlNode^ node)
    : polything(node)
{
}


polygon::polygon(IXmlNode^ node)
    : polything(node)
{
}


path::path(ICanvasResourceCreator^ resourceCreator, IXmlNode^ node)
    : element(node)
    , geometry_(parse_path_data(resourceCreator, node))
{
}


text::text(IXmlNode^ node)
    : element(node)
    , x_(parse_coordinate(node, L"x"))
    , y_(parse_coordinate(node, L"y"))
    , text_(node->InnerText)
{
}
