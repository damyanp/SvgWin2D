#include "pch.h"

#include "SvgDrawing.h"
#include "parse.h"

using namespace SvgWin2D;


/*static*/
IAsyncOperation<SvgDrawing^>^ SvgDrawing::LoadAsync(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument)
{
    return concurrency::create_async([=] { return Load(resourceCreator, svgDocument); });
}


SvgDrawing^ SvgDrawing::Load(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument)
{
    return ref new SvgDrawing(build_svg(svgDocument));
}


ICanvasImage^ SvgDrawing::Draw(Size destinationSize)
{
    return root_->create_image(destinationSize);
}