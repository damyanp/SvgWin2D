#include "pch.h"

#include "SvgDrawing.h"
#include "parse.h"

using namespace Windows::UI;

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
    auto background = ref new Effects::ColorSourceEffect();
    background->Color = Colors::Blue;

    auto crop = ref new Effects::CropEffect();

    auto width = root_->calculate_width(destinationSize.Width);
    auto height = root_->calculate_height(destinationSize.Height);

    crop->SourceRectangle = Rect{ 0, 0, width, height };
    crop->Source = background;

    return crop;
}