#include "pch.h"

#include "path_parser.h"

/*static*/
std::wregex path_parser::sPathTokenRegex(
    L"[,[:space:]]*"                           // ignored whitespaces & commas
    L"("                                       // a token could be:
    L"[MmZzLlHhVvCcSsQqTtAa]|"                 // a command character
    L"[+-]?[0-9]*\\.[0-9]*([eE][+-]?[0-9]+)?|" // a number
    L"[+-]?[0-9]+"                             // an integer
    L")"                                       // 
    L"[,[:space:]]*"                           // ignored whitespaces & commas
    );


/*static*/
std::regex_token_iterator<const wchar_t*> path_parser::end;


static bool use_absolute_coords(wchar_t cmd)
{
    return std::iswupper(cmd) != 0;
}


/*static*/
CanvasGeometry^ path_parser::parse(ICanvasResourceCreator^ resourceCreator, Platform::String^ string)
{
    path_parser parser(resourceCreator, string);
    return parser.geometry();
}


path_parser::path_parser(ICanvasResourceCreator^ resourceCreator, Platform::String^ string)
    : it_(string->Begin(), string->End(), sPathTokenRegex, 1, std::regex_constants::match_continuous)
    , path_(ref new CanvasPathBuilder(resourceCreator))
    , inFigure_(false)
    , currentPos_{ 0, 0 }
    , lastCommandCPType_(cp_type::none)
    , lastCommandCP_{ 0, 0 }
{
    while (parse_moveto_drawto_command_group())
        ;
}


CanvasGeometry^ path_parser::geometry()
{
    return CanvasGeometry::CreatePath(path_);
}


//
// See http://www.w3.org/TR/SVG11/paths.html#PathDataBnf
//

class Backtracker
{
    std::regex_token_iterator<const wchar_t*>* dest_;
    std::regex_token_iterator<const wchar_t*> storedIt_;

public:
    Backtracker(std::regex_token_iterator<const wchar_t*>* dest)
        : dest_(dest)
        , storedIt_(*dest_)
    {
    }

    ~Backtracker()
    {
        if (dest_)
            *dest_ = storedIt_;
    }

    void accept()
    {
        dest_ = nullptr;
    }
};


Backtracker path_parser::backtracker()
{
    return Backtracker(&it_);
}


wchar_t path_parser::peek_next_command()
{
    if (it_ == end)
        return L'\0';

    auto cmd = it_->str();

    if (cmd.length() > 1 || cmd.empty())
        return L'\0';

    return cmd[0];
}


bool path_parser::try_take_number(float* n)
{
    if (it_ == end)
        return false;

    auto str = it_->str();
    wchar_t* numberEnd = nullptr;
    float parsedNumber = static_cast<float>(wcstod(str.c_str(), &numberEnd));

    // it can't have been a number if the end of the number is the same as the
    // beginning of the number
    if (numberEnd == str.c_str())
        return false;

    *n = parsedNumber;

    ++it_;
    return true;
}
    


bool path_parser::parse_moveto_drawto_command_group()
{
    if (!parse_moveto())
        return false;

    // followed by zero or more other drawto_commands
    while (parse_drawto_command())
        ;

    if (inFigure_)
    {
        path_->EndFigure(CanvasFigureLoop::Open);
        inFigure_ = false;
    }

    return true;
}


bool path_parser::parse_moveto()
{
    auto b = backtracker();

    auto cmd = peek_next_command();

    if (cmd != L'M' && cmd != 'm')
        return false;

    ++it_;

    float2 pos;
    if (!parse_coordinate_pair(&pos))
        return false;

    bool absoluteCoords = use_absolute_coords(cmd);
    if (!absoluteCoords)
        pos = currentPos_ + pos;

    set_current_pos(pos);

    if (inFigure_)
    {
        path_->EndFigure(CanvasFigureLoop::Open);
        inFigure_ = false;
    }

    path_->BeginFigure(pos);
    inFigure_ = true;
    figureStartPos_ = pos;

    parse_lineto_argument_sequence(absoluteCoords);

    b.accept();
    return true;
}


bool path_parser::parse_drawto_command()
{
    auto cmd = peek_next_command();
    if (cmd == L'\0')
        return false;

    auto oldIt = it_;
    ++it_;

    bool absoluteCoords = use_absolute_coords(cmd);

    switch (cmd)
    {
    case 'Z':
    case 'z':
        return parse_closepath();

    case 'L':
    case 'l':
        return parse_lineto(absoluteCoords);

    case 'H':
    case 'h':
        return parse_horizontal_lineto(absoluteCoords);

    case 'V':
    case 'v':
        return parse_vertical_lineto(absoluteCoords);

    case 'C':
    case 'c':
        return parse_curveto(absoluteCoords);

    case 'S':
    case 's':
        return parse_smooth_curveto(absoluteCoords);

    case 'Q':
    case 'q':
        return parse_quadratic_bezier_curveto(absoluteCoords);

    case 'T':
    case 't':
        return parse_smooth_quadratic_bezier_curveto(absoluteCoords);

    case 'A':
    case 'a':
        return parse_elliptical_arc(absoluteCoords);

    default:
        it_ = oldIt;
        return false;
    }
}


bool path_parser::parse_closepath()
{
    if (inFigure_)
    {
        path_->EndFigure(CanvasFigureLoop::Closed);
        inFigure_ = false;
    }

    set_current_pos(figureStartPos_);

    return true;
}


bool path_parser::parse_lineto(bool absoluteCoords)
{
    parse_lineto_argument_sequence(absoluteCoords);
    return true;
}


void path_parser::parse_lineto_argument_sequence(bool absoluteCoords)
{
    float2 pos;
    while (parse_coordinate_pair(&pos))
    {
        if (!absoluteCoords)
            pos = currentPos_ + pos;

        path_->AddLine(pos);
        set_current_pos(pos);
    }
}


bool path_parser::parse_horizontal_lineto(bool absoluteCoords)
{
    float x;

    if (!try_take_number(&x))
        return false;

    do
    {
        float2 p = currentPos_;

        if (absoluteCoords)
            p.x = x;
        else
            p.x += x;

        path_->AddLine(p);
        set_current_pos(p);

    } while (try_take_number(&x));

    return true;
}


bool path_parser::parse_vertical_lineto(bool absoluteCoords)
{
    float y;

    if (!try_take_number(&y))
        return false;

    do
    {
        float2 p = currentPos_;
        if (absoluteCoords)
            p.y = y;
        else
            p.y += y;

        path_->AddLine(p);
        set_current_pos(p);
    } while (try_take_number(&y));

    return true;
}


bool path_parser::parse_curveto(bool absoluteCoords)
{
    if (!parse_curveto_argument(absoluteCoords))
        return false;
    
    while (parse_curveto_argument(absoluteCoords))
        ;

    return true;
}


bool path_parser::parse_curveto_argument(bool absoluteCoords)
{
    auto b = backtracker();

    float2 cp1, cp2, ep;
    
    if (!parse_coordinate_pair(&cp1))
        return false;

    if (!parse_coordinate_pair(&cp2))
        return false;

    if (!parse_coordinate_pair(&ep))
        return false;

    if (!absoluteCoords)
    {
        cp1 += currentPos_;
        cp2 += currentPos_;
        ep += currentPos_;
    }

    path_->AddCubicBezier(cp1, cp2, ep);

    set_current_pos(ep, cp_type::cubic, cp2);

    b.accept();
    return true;
}


bool path_parser::parse_smooth_curveto(bool absoluteCoords)
{
    if (!parse_smooth_curveto_argument(absoluteCoords))
        return false;

    while (parse_smooth_curveto_argument(absoluteCoords))
        ;

    return true;
}


bool path_parser::parse_smooth_curveto_argument(bool absoluteCoords)
{
    using namespace Windows::Foundation::Numerics;

    auto b = backtracker();

    float2 cp2, ep;

    if (!parse_coordinate_pair(&cp2))
        return false;

    if (!parse_coordinate_pair(&ep))
        return false;

    if (!absoluteCoords)
    {
        cp2 += currentPos_;
        ep += currentPos_;
    }

    float2 cp1;

    cp1 = currentPos_ + (currentPos_ - last_cp(cp_type::cubic));

    path_->AddCubicBezier(cp1, cp2, ep);

    set_current_pos(ep, cp_type::cubic, cp2);

    b.accept();
    return true;
}


bool path_parser::parse_quadratic_bezier_curveto(bool absoluteCoords)
{
    if (!parse_quadratic_bezier_curveto_argument(absoluteCoords))
        return false;
    
    while (parse_quadratic_bezier_curveto_argument(absoluteCoords))
        ;
    
    return true;
}


bool path_parser::parse_quadratic_bezier_curveto_argument(bool absoluteCoords)
{
    auto b = backtracker();

    float2 cp, ep;

    if (!parse_coordinate_pair(&cp))
        return false;

    if (!parse_coordinate_pair(&ep))
        return false;

    if (!absoluteCoords)
    {
        cp += currentPos_;
        ep += currentPos_;
    }

    path_->AddQuadraticBezier(cp, ep);

    set_current_pos(ep, cp_type::quadratic, cp);

    b.accept();
    return true;
}


bool path_parser::parse_smooth_quadratic_bezier_curveto(bool absoluteCoords)
{
    if (!parse_smooth_quadratic_bezier_curveto_argument(absoluteCoords))
        return false;

    while (parse_smooth_quadratic_bezier_curveto_argument(absoluteCoords))
        ;

    return true;
}


bool path_parser::parse_smooth_quadratic_bezier_curveto_argument(bool absoluteCoords)
{
    auto b = backtracker();

    float2 ep;

    if (!parse_coordinate_pair(&ep))
        return false;

    if (!absoluteCoords)
    {
        ep += currentPos_;
    }

    float2 cp;
    cp = currentPos_ + (currentPos_ - last_cp(cp_type::quadratic));

    path_->AddQuadraticBezier(cp, ep);

    set_current_pos(ep, cp_type::quadratic, cp);

    b.accept();
    return true;
}


bool path_parser::parse_elliptical_arc(bool absoluteCoords)
{
    if (!parse_elliptical_arc_argument(absoluteCoords))
        return false;

    while (parse_elliptical_arc_argument(absoluteCoords))
        ;

    return true;
}


bool path_parser::parse_elliptical_arc_argument(bool absoluteCoords)
{
    auto b = backtracker();

    float2 r;
    float rotation;
    bool isLargeArcFlag;
    bool sweepFlag;
    float2 ep;

    if (!parse_coordinate_pair(&r))
        return false;

    if (!try_take_number(&rotation))
        return false;

    if (!parse_flag(&isLargeArcFlag))
        return false;

    if (!parse_flag(&sweepFlag))
        return false;

    if (!parse_coordinate_pair(&ep))
        return false;

    if (!absoluteCoords)
    {
        ep += currentPos_;
    }

    rotation = DirectX::XMConvertToRadians(rotation);

    path_->AddArc(
        ep, 
        r.x, 
        r.y, 
        rotation, 
        sweepFlag ? CanvasSweepDirection::Clockwise : CanvasSweepDirection::CounterClockwise,
        isLargeArcFlag ? CanvasArcSize::Large : CanvasArcSize::Small);

    set_current_pos(ep);

    b.accept();
    return true;
}

bool path_parser::parse_coordinate_pair(float2* pos)
{
    auto b = backtracker();

    if (!try_take_number(&pos->x))
        return false;

    if (!try_take_number(&pos->y))
        return false;

    b.accept();
    return true;
}


bool path_parser::parse_flag(bool* flag)
{
    float flagAsNumber;

    if (!try_take_number(&flagAsNumber))
        return false;

    *flag = (flagAsNumber != 0.0f);
    return true;
}


void path_parser::set_current_pos(float2 pos)
{
    currentPos_ = pos;
    lastCommandCPType_ = cp_type::none;
}


void path_parser::set_current_pos(float2 pos, cp_type type, float2 cp)
{
    currentPos_ = pos;
    lastCommandCPType_ = type;
    lastCommandCP_ = cp;
}


float2 path_parser::last_cp(cp_type type)
{
    if (type == lastCommandCPType_)
        return lastCommandCP_;
    else
        return currentPos_;
}