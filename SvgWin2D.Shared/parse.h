#pragma once

#include "svg.h"

std::unique_ptr<svg> build_svg(Windows::Data::Xml::Dom::XmlDocument^ document);

std::unique_ptr<viewBox> parse_viewBox(Platform::String^ viewBoxString);

