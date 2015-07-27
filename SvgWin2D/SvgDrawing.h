#pragma once

namespace SvgWin2D
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Windows::Foundation;
    using namespace Windows::Data::Xml::Dom;

    public ref class SvgDrawing sealed
    {
    public:
        IAsyncOperation<SvgDrawing^>^ Load(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument);
    };
}

