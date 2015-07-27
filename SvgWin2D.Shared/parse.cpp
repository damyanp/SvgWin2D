#include "pch.h"

#include "parse.h"

using namespace Windows::Data::Xml::Dom;

Platform::String^ SVG_NS = L"http://w3.org/2000/svg";
Platform::String^ XMLNS_SVG = L"xmlns:svg='http://www.w3.org/2000/svg'";

std::wregex gTokenRegex(L"(-?[0-9.]+)\\s*,?\\s*");

std::unique_ptr<viewBox> parse_viewBox(Platform::String^ viewBoxString)
{
    auto viewBoxBegin = viewBoxString->Data();
    auto viewBoxEnd = viewBoxBegin + viewBoxString->Length();
    std::regex_token_iterator<const wchar_t*> it(viewBoxBegin, viewBoxEnd, gTokenRegex);
    std::regex_token_iterator<const wchar_t*> end;

    if (it == end)
        return nullptr;
    float x = static_cast<float>(_wtof(it->str().c_str()));
    ++it;

    if (it == end)
        return nullptr;
    float y = static_cast<float>(_wtof(it->str().c_str()));
    ++it;

    if (it == end)
        return nullptr;
    float w = static_cast<float>(_wtof(it->str().c_str()));
    ++it;

    if (it == end)
        return nullptr;
    float h = static_cast<float>(_wtof(it->str().c_str()));
    ++it;

    return std::make_unique<viewBox>(x, y, w, h);
}


std::unique_ptr<viewBox> parse_viewBox(IXmlNode^ svgElement)
{
    auto attribute = svgElement->Attributes->GetNamedItemNS(SVG_NS, L"viewBox");
    if (!attribute)
        return nullptr;

    auto viewBoxString = dynamic_cast<Platform::String^>(attribute->NodeValue);
    if (!viewBoxString)
        return nullptr;

    return parse_viewBox(viewBoxString);
}


std::unique_ptr<svg> build_svg(IXmlNode^ svgElement)
{
    return std::make_unique<svg>(parse_viewBox(svgElement));
}


std::unique_ptr<svg> build_svg(XmlDocument^ svgDocument)
{
    auto svgElement = svgDocument->SelectSingleNodeNS("svg:svg", XMLNS_SVG);
    return build_svg(svgElement);
}

