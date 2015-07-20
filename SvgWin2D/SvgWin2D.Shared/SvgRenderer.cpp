#include "pch.h"

#include "WinXmlPolicy.h"

namespace SvgWin2D
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Microsoft::Graphics::Canvas::Geometry;
    using namespace Microsoft::Graphics::Canvas::Text;
    using namespace Windows::Data::Xml::Dom;
    using namespace Windows::Foundation::Numerics;
    using namespace svgpp;

    typedef boost::mpl::set<
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

    typedef boost::mpl::insert<
        traits::shapes_attributes_by_element,
        tag::attribute::transform
        >::type processed_attributes_t;

    class Context
    {
        CanvasDrawingSession^ m_drawingSession;

        CanvasPathBuilder^ m_currentPathBuilder;
        bool m_inFigure;

    public:
        Context(CanvasDrawingSession^ drawingSession)
            : m_drawingSession(drawingSession)
        {
        }

        void on_enter_element(tag::element::any)
        {}

        void on_exit_element()
        {}

        void transform_matrix(boost::array<double, 6> const& matrix)
        {
            m_drawingSession->Transform = float3x2(
                static_cast<float>(matrix[0]), 
                static_cast<float>(matrix[1]),
                static_cast<float>(matrix[2]),
                static_cast<float>(matrix[3]),
                static_cast<float>(matrix[4]),
                static_cast<float>(matrix[5])); // TODO: this is a guess
       }

        void path_move_to(double x, double y, tag::coordinate::absolute)
        {
            if (!m_currentPathBuilder)
            {
                m_currentPathBuilder = ref new CanvasPathBuilder(m_drawingSession);
                m_currentPathBuilder->SetFilledRegionDetermination(CanvasFilledRegionDetermination::Winding);
                m_inFigure = false;
            }

            if (m_inFigure)
            {
                m_currentPathBuilder->EndFigure(CanvasFigureLoop::Open);
                m_inFigure = false;
            }

            m_currentPathBuilder->BeginFigure(static_cast<float>(x), static_cast<float>(y));
        }

        void path_line_to(double x, double y, tag::coordinate::absolute)
        {
            m_currentPathBuilder->AddLine(static_cast<float>(x), static_cast<float>(y));
        }

        void path_cubic_bezier_to(
            double x1, double y1,
            double x2, double y2,
            double x, double y,
            tag::coordinate::absolute)
        {
            m_currentPathBuilder->AddCubicBezier(
                float2(static_cast<float>(x1), static_cast<float>(y1)),
                float2(static_cast<float>(x2), static_cast<float>(y2)),
                float2(static_cast<float>(x), static_cast<float>(y)));
        }

        void path_quadratic_bezier_to(
            double x1, double y1,
            double x, double y,
            tag::coordinate::absolute)
        {
            m_currentPathBuilder->AddQuadraticBezier(
                float2(static_cast<float>(x1), static_cast<float>(y1)),
                float2(static_cast<float>(x), static_cast<float>(y)));
        }

        void path_elliptical_arc_to(
            double rx, double ry, double x_axis_rotation,
            bool large_arc_flag, bool sweep_flag,
            double x, double y,
            tag::coordinate::absolute)
        {
            m_currentPathBuilder->AddArc(
                float2(static_cast<float>(x), static_cast<float>(y)),
                static_cast<float>(rx), static_cast<float>(ry),
                static_cast<float>(x_axis_rotation),
                sweep_flag ? CanvasSweepDirection::CounterClockwise : CanvasSweepDirection::Clockwise, // TODO: this is a guess
                large_arc_flag ? CanvasArcSize::Small : CanvasArcSize::Large); // TODO: this is a guess
        }

        void path_close_subpath()
        {
            m_currentPathBuilder->EndFigure(CanvasFigureLoop::Closed);
            m_inFigure = false;
        }

        void path_exit()
        {
            auto geometry = CanvasGeometry::CreatePath(m_currentPathBuilder);
            m_currentPathBuilder = nullptr;

            m_drawingSession->FillGeometry(geometry, Windows::UI::Colors::White);
        }
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
                processed_attributes<processed_attributes_t>
                >::load_document(svgElement, context);

            auto format = ref new CanvasTextFormat();
            format->HorizontalAlignment = CanvasHorizontalAlignment::Center;
            format->VerticalAlignment = CanvasVerticalAlignment::Center;

            ds->DrawText(L"Hello, world", 0, 0, Windows::UI::Colors::White, format);
            return commandList;
        }
    };
}