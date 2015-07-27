#include "pch.h"

#include "parse.h"

using namespace Windows::Data::Xml::Dom;

Platform::String^ SVG_NS = L"http://w3.org/2000/svg";
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


length parse_width_or_height(IXmlNode^ element, Platform::String^ name)
{
    auto defaultLength = length{ 100, unit::percent };

    auto attribute = element->Attributes->GetNamedItemNS(SVG_NS, name);
    if (!attribute)
        return defaultLength;

    auto attributeString = dynamic_cast<Platform::String^>(attribute->NodeValue);
    if (!attributeString)
        return defaultLength;

    return parse_length(attributeString, defaultLength);
}


std::unique_ptr<svg> build_svg(IXmlNode^ svgElement)
{
    return std::make_unique<svg>(
        parse_viewBox(svgElement),
        parse_width_or_height(svgElement, L"width"),
        parse_width_or_height(svgElement, L"height"));
}


std::unique_ptr<svg> build_svg(XmlDocument^ svgDocument)
{
    auto svgElement = svgDocument->SelectSingleNodeNS("svg:svg", XMLNS_SVG);
    return build_svg(svgElement);
}

