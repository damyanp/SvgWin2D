#include "pch.h"

namespace SvgWin2D
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Microsoft::Graphics::Canvas::Text;
    using namespace Windows::Data::Xml::Dom;

    public ref class SvgRenderer sealed
    {
    public:
        static CanvasCommandList^ Render(ICanvasResourceCreator^ resourceCreator, XmlDocument^ svgDocument)
        {
            auto commandList = ref new CanvasCommandList(resourceCreator);
            auto ds = commandList->CreateDrawingSession();

            auto format = ref new CanvasTextFormat();
            format->HorizontalAlignment = CanvasHorizontalAlignment::Center;
            format->VerticalAlignment = CanvasVerticalAlignment::Center;

            ds->DrawText(L"Hello, world", 0, 0, Windows::UI::Colors::White, format);
            return commandList;
        }
    };
}