#pragma once

#include "svg.h"

std::unique_ptr<svg> parse_svg(Microsoft::Graphics::Canvas::ICanvasResourceCreator^ resourceCreator, Windows::Data::Xml::Dom::XmlDocument^ document);

std::unique_ptr<viewBox> parse_viewBox(Platform::String^ viewBoxString);
length parse_length(Platform::String^ lengthString, length defaultLength);
std::vector<std::wstring> parse_comma_separated_list(Platform::String^ str);

