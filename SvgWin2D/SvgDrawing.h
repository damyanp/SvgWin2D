#pragma once

#include "svg.h"

namespace SvgWin2D
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Windows::Foundation;
    using namespace Windows::Data::Xml::Dom;

    public ref class SvgDrawing sealed
    {
        std::unique_ptr<svg> root_;

    public:
        //
        // Builds up the SvgDrawing instance from an SvgDocument.  This is async because the SVG file may reference other
        // resources that need to be downloaded.
        //
        static IAsyncOperation<SvgDrawing^>^ LoadAsync(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument);

        ICanvasImage^ Draw(Size destinationSize);

    private:
        SvgDrawing(std::unique_ptr<svg>&& root)
            : root_(std::move(root))
        {}

        static SvgDrawing^ Load(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument);
    };
}

