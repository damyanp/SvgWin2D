#pragma once

using Windows::Foundation::Numerics::float2;
using namespace Microsoft::Graphics::Canvas::Geometry;

inline std::wostream& operator<<(std::wostream& s, float2 const& value)
{
    s << value.x << L", " << value.y;
    return s;
}


inline std::wostream& operator<<(std::wostream& s, CanvasFigureLoop figureLoop)
{
    switch (figureLoop)
    {
    case CanvasFigureLoop::Open: s << L"open"; break;
    case CanvasFigureLoop::Closed: s<< L"closed"; break;
    default: s << L"<<unknown CanvasFigureLoop>>"; break;
    }

    return s;
}


ref class CanvasPathToString sealed : public ICanvasPathReceiver
{
    std::wstringstream str_;

public:
    virtual void BeginFigure(float2 startPos, CanvasFigureFill fill)
    {
        str_ << L"BeginFigure(" << startPos << L") ";
    }

    virtual void AddArc(float2 endPoint, float rx, float ry, float angle, CanvasSweepDirection sweepDirection, CanvasArcSize arcSize)
    {
    }

    virtual void AddCubicBezier(float2 controlPoint1, float2 controlPoint2, float2 endPoint)
    {
    }

    virtual void AddLine(float2 endPoint)
    {
        str_ << L"AddLine(" << endPoint << L") ";
    }

    virtual void AddQuadraticBezier(float2 controlPoint, float2 endPoint)
    {
        str_ << L"AddQuadraticBezier(" << controlPoint << L" " << endPoint << L") ";
    }

    virtual void SetFilledRegionDetermination(CanvasFilledRegionDetermination filledRegionDetermination)
    {
    }

    virtual void SetSegmentOptions(CanvasFigureSegmentOptions segmentOptions)
    {
    }

    virtual void EndFigure(CanvasFigureLoop figureLoop)
    {
        str_ << L"EndFigure(" << figureLoop << L") ";
    }

internal:
    std::wstring str() { return str_.str(); }
};