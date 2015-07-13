#include "pch.h"

namespace SvgWin2D
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Windows::Data::Xml::Dom;

    public ref class SvgRenderer sealed
    {
    public:
        CanvasCommandList^ Render(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument)
        {
            auto commandList = ref new CanvasCommandList(resourceCreator);
            auto ds = commandList->CreateDrawingSession();
            ds->DrawText(L"Hello, world", 0, 0, Windows::UI::Colors::White);
            return commandList;
        }
    };
}