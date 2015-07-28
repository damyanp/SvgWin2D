#include "pch.h"

#include "parse.h"

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
    auto attribute = element->Attributes->GetNamedItemNS(SVG_NS, L"viewBox");
    if (!attribute)
        return nullptr;

    auto viewBoxString = dynamic_cast<Platform::String^>(attribute->NodeValue);
    if (!viewBoxString)
        return nullptr;

    return parse_viewBox(viewBoxString);
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
std::wregex gHexColorRegex(L"#([[:xdigit:]]{3}|[[:xdigit:]]{6})");

std::unique_ptr<paint> parse_paint(IXmlNode^ element, Platform::String^ name)
{
    auto attributeString = get_attribute(element, name);

    if (!attributeString)
        return nullptr;

    if (attributeString == "none")
        return std::make_unique<paint>(paint_type::none, Colors::HotPink);

    std::wcmatch match;
    if (std::regex_match(attributeString->Data(), attributeString->Data() + attributeString->Length(), match, gHexColorRegex))
    {
        auto hexColor = match[1].str();
        std::wstring rs, gs, bs;
        if (hexColor.length() == 3)
        {
            rs.assign(hexColor.begin(), hexColor.begin()+1);
            gs.assign(hexColor.begin()+1, hexColor.begin()+2);
            bs.assign(hexColor.begin()+2, hexColor.begin()+3);
        }
        else
        {
            assert(hexColor.length() == 6);
            rs.assign(hexColor.begin(), hexColor.begin()+2);
            gs.assign(hexColor.begin()+2, hexColor.begin()+4);
            bs.assign(hexColor.begin()+4, hexColor.begin()+6);
        }
        
        uint8_t r = static_cast<uint8_t>(std::stoul(rs, nullptr, 16));
        uint8_t g = static_cast<uint8_t>(std::stoul(gs, nullptr, 16));
        uint8_t b = static_cast<uint8_t>(std::stoul(bs, nullptr, 16));

        return std::make_unique<paint>(paint_type::color, Color{255, r, g, b});
    }

    // TODO: proper color parsing!

    if (attributeString == L"black")
        return std::make_unique<paint>(paint_type::color, Colors::Black);

    if (attributeString == L"green")
        return std::make_unique<paint>(paint_type::color, Colors::Green);

    if (attributeString == L"lime")
        return std::make_unique<paint>(paint_type::color, Colors::Lime);

    if (attributeString == L"yellow")
        return std::make_unique<paint>(paint_type::color, Colors::Yellow);

    if (attributeString == L"blue")
        return std::make_unique<paint>(paint_type::color, Colors::Blue);

    return std::make_unique<paint>(paint_type::color, Colors::HotPink);
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


std::unique_ptr<element> parse_any_element(IXmlNode^ node)
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
        return std::make_unique<svg>(node);
    else if (name == L"g")
        return std::make_unique<group>(node);
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
    else
        return nullptr;
}



std::unique_ptr<svg> parse_svg(XmlDocument^ svgDocument)
{
    auto svgElement = svgDocument->SelectSingleNodeNS("svg:svg", XMLNS_SVG);
    return std::make_unique<svg>(svgElement);
}


//
// The various elements construct construct themselves from the XML nodes.
//


element::element(IXmlNode^ node)
    : fillPaint_(parse_paint(node, L"fill"))
    , strokePaint_(parse_paint(node, L"stroke"))
    , strokeWidth_(parse_stroke_width(node))
{
}


container_element::container_element(IXmlNode^ node)
    : element(node)
{
    for (auto const& childNode : node->ChildNodes)
    {
        if (childNode->NodeType != NodeType::ElementNode)
            continue;

        auto childElement = parse_any_element(childNode);

        if (childElement)
            elements_.push_back(std::move(childElement));
    }
}


svg::svg(IXmlNode^ node)
    : container_element(node)
    , viewBox_(parse_viewBox(node))
    , width_(parse_width_or_height(node, L"width"))
    , height_(parse_width_or_height(node, L"height"))
{
}


group::group(IXmlNode^ node)
    : container_element(node)
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

