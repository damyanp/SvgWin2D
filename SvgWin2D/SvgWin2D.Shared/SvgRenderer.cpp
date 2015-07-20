#include "pch.h"

#include "WinXmlPolicy.h"

namespace SvgWin2D
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Microsoft::Graphics::Canvas::Text;
    using namespace Windows::Data::Xml::Dom;
    using namespace svgpp;

    typedef 
    boost::mpl::set<
        // SVG Structural Elements
        tag::element::svg,
        tag::element::g,
        // SVG Shape Elements
        tag::element::circle,
        tag::element::ellipse,
        tag::element::line,
        tag::element::path,
        tag::element::polygon,
        tag::element::polyline,
        tag::element::rect
        >::type processed_elements_t;
    
                                           
    class Context
    {
        CanvasDrawingSession^ m_drawingSession;

    public:
        Context(CanvasDrawingSession^ drawingSession)
            : m_drawingSession(drawingSession)
        {
        }

        void on_enter_element(tag::element::any)
        {}

        void on_exit_element()
        {}


        void path_move_to(double x, double y, tag::coordinate::absolute)
        {}

        void path_line_to(double x, double y, tag::coordinate::absolute)
        {}

        void path_cubic_bezier_to(
            double x1, double y1,
            double x2, double y2,
            double x, double y,
            tag::coordinate::absolute)
        {}

        void path_quadratic_bezier_to(
            double x1, double y1,
            double x, double y,
            tag::coordinate::absolute)
        {}

        void path_elliptical_arc_to(
            double rx, double ry, double x_axis_rotation,
            bool large_arc_flag, bool sweep_flag,
            double x, double y,
            tag::coordinate::absolute)
        {}

        void path_close_subpath()
        {}

        void path_exit()
        {}
    };

    public ref class SvgRenderer sealed
    {
    public:
        static CanvasCommandList^ Render(ICanvasResourceCreator^ resourceCreator, IXmlNode^ svgElement)
        {
            auto commandList = ref new CanvasCommandList(resourceCreator);
            auto ds = commandList->CreateDrawingSession();

            Context context(ds);
            document_traversal<
                processed_elements<processed_elements_t>,
                processed_attributes<traits::shapes_attributes_by_element>
                >::load_document(svgElement, context);

            auto format = ref new CanvasTextFormat();
            format->HorizontalAlignment = CanvasHorizontalAlignment::Center;
            format->VerticalAlignment = CanvasVerticalAlignment::Center;

            ds->DrawText(L"Hello, world", 0, 0, Windows::UI::Colors::White, format);
            return commandList;
        }
    };
}