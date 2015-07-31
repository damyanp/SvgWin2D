#pragma once

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Geometry;
using Windows::Foundation::Numerics::float2;

class path_parser
{
    std::regex_token_iterator<const wchar_t*> it_;
    static std::regex_token_iterator<const wchar_t*> end;

    CanvasPathBuilder^ path_;
    bool inFigure_;
    float2 figureStartPos_;
    float2 currentPos_;

    enum class cp_type { none, cubic, quadratic };
    cp_type lastCommandCPType_;
    float2 lastCommandCP_;

public:
    static CanvasGeometry^ parse(ICanvasResourceCreator^ resourceCreator, Platform::String^ string);

    static std::wregex sPathTokenRegex;

    path_parser(ICanvasResourceCreator^ resourceCreator, Platform::String^ string);

private:
    CanvasGeometry^ geometry();

    class Backtracker backtracker();

    wchar_t peek_next_command();
    bool try_take_number(float* n);

    bool parse_moveto_drawto_command_group();
    bool parse_moveto();
    bool parse_drawto_command();
    bool parse_closepath();
    bool parse_lineto(bool absoluteCoords);
    void parse_lineto_argument_sequence(bool absoluteCoords);
    bool parse_horizontal_lineto(bool absoluteCoords);
    bool parse_vertical_lineto(bool absoluteCoords);
    bool parse_curveto(bool absoluteCoords);
    bool parse_curveto_argument(bool absoluteCoords);
    bool parse_smooth_curveto(bool absoluteCoords);
    bool parse_smooth_curveto_argument(bool absoluteCoords);
    bool parse_quadratic_bezier_curveto(bool absoluteCoords);
    bool parse_quadratic_bezier_curveto_argument(bool absoluteCoords);
    bool parse_smooth_quadratic_bezier_curveto(bool absoluteCoords);
    bool parse_smooth_quadratic_bezier_curveto_argument(bool absoluteCoords);
    bool parse_elliptical_arc(bool absoluteCoords);
    bool parse_elliptical_arc_argument(bool absoluteCoords);
    
    bool parse_coordinate_pair(float2* pos);
    bool parse_flag(bool* flag);

    void set_current_pos(float2 pos);
    void set_current_pos(float2 pos, cp_type type, float2 cp);
    float2 last_cp(cp_type type);
};
