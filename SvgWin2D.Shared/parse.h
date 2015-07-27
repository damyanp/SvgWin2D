#pragma once

#include "svg.h"

std::unique_ptr<svg> parse_svg(Windows::Data::Xml::Dom::XmlDocument^ document);

std::unique_ptr<viewBox> parse_viewBox(Platform::String^ viewBoxString);
length parse_length(Platform::String^ lengthString, length defaultLength);


