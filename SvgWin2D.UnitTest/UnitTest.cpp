#include "pch.h"
#include "CppUnitTest.h"

#include "parse.h"
#include "path_parser.h"
#include "transform_parser.h"

#include "CanvasPathToString.h"

using Windows::Foundation::Numerics::make_float3x2_translation;
using Windows::Foundation::Numerics::make_float3x2_scale;
using Windows::Foundation::Numerics::make_float3x2_rotation;
using Windows::Foundation::Numerics::make_float3x2_skew;
using Windows::Foundation::Numerics::float2;
using DirectX::XMConvertToRadians;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template <> 
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<length>(const length& l)
{ 
    return L"<length>";
}

TEST_CLASS(ParseTest)
{
public:
#pragma region(ViewBox)
    TEST_METHOD(ViewBox_EmptyString_ReturnsNull)
    {
        Assert::IsNull(parse_viewBox(L"").get());
    }

    TEST_METHOD(ViewBox_Invalid_ReturnsNull)
    {
        Assert::IsNull(parse_viewBox(L"a").get());
        Assert::IsNull(parse_viewBox(L"1,2,v").get());
        Assert::IsNull(parse_viewBox(L"1 2 3").get());
        Assert::IsNull(parse_viewBox(L"1 2 4 c").get());
    }

    void TestParseViewBox(const wchar_t* str, float x, float y, float w, float h)
    {
        auto vb = parse_viewBox(ref new Platform::String(str));
        Assert::IsNotNull(vb.get());

        Assert::AreEqual(x, vb->X);
        Assert::AreEqual(y, vb->Y);
        Assert::AreEqual(w, vb->Width);
        Assert::AreEqual(h, vb->Height);
    }

    TEST_METHOD(ViewBox_ValidStrings)
    {
        TestParseViewBox(L"0 0 0 0", 0, 0, 0, 0);
        TestParseViewBox(L"1 2 3 4", 1, 2, 3, 4);
        TestParseViewBox(L"1.2 3.4 5.6 7.8", 1.2f, 3.4f, 5.6f, 7.8f);
           
        TestParseViewBox(L"0,0,0,0", 0, 0, 0, 0);
        TestParseViewBox(L"1,2,3,4", 1, 2, 3, 4);
        TestParseViewBox(L"1.2,3.4,5.6,7.8", 1.2f, 3.4f, 5.6f, 7.8f);

        TestParseViewBox(L"   1.2 , 3.4 , 5.6 , 7.8    ", 1.2f, 3.4f, 5.6f, 7.8f);

        TestParseViewBox(L"-4, -3, -2, -1", -4, -3, -2, -1);
    }
#pragma endregion
#pragma region(Length)
    TEST_METHOD(Length_EmptyString_ReturnsDefault)
    {
        Assert::AreEqual(length{ 1, unit::in }, parse_length(L"", length{ 1, unit::in }));
        Assert::AreEqual(length{ 2, unit::em }, parse_length(L"", length{ 2, unit::em }));
    }

    TEST_METHOD(Length_Invalid_ReturnsDefault)
    {
        auto anyLength = length{ 77, unit::ex };

        Assert::AreEqual(anyLength, parse_length(L"1 2ex", anyLength));
        Assert::AreEqual(anyLength, parse_length(L"ex 1", anyLength));
    }

    TEST_METHOD(Length_ValidStrings)
    {
        auto anyLength = length{ 77, unit::ex };

        Assert::AreEqual(length{ 1, unit::em }, parse_length(L"1em", anyLength));
        Assert::AreEqual(length{ 2, unit::ex }, parse_length(L"2ex", anyLength));
        Assert::AreEqual(length{ 3, unit::px }, parse_length(L"3px", anyLength));
        Assert::AreEqual(length{ 4, unit::in }, parse_length(L"4in", anyLength));
        Assert::AreEqual(length{ 5, unit::cm }, parse_length(L"5cm", anyLength));
        Assert::AreEqual(length{ 6, unit::mm }, parse_length(L"6mm", anyLength));
        Assert::AreEqual(length{ 7, unit::pt }, parse_length(L"7pt", anyLength));
        Assert::AreEqual(length{ 8, unit::pc }, parse_length(L"8pc", anyLength));
        Assert::AreEqual(length{ 9, unit::percent }, parse_length(L"9%", anyLength));
        Assert::AreEqual(length{ 10, unit::unspecified }, parse_length(L"10", anyLength));

        Assert::AreEqual(length{ 10.1f, unit::unspecified }, parse_length(L"10.1", anyLength));
        Assert::AreEqual(length{ -10.2f, unit::unspecified }, parse_length(L"-10.2", anyLength));
    }
#pragma endregion
#pragma region(Transform)
    TEST_METHOD(Transform_TokenizeRegex)
    {
        Platform::String^ s(L"1 2.3 -4 -5.6 -7. -8.9E+10 -11.12E13 matrix scale translate(14) rotate(15,16,17,)");

        std::regex_token_iterator<const wchar_t*> it(s->Begin(), s->Begin() + s->Length(), transform_parser::sTransformTokenRegex, 1);

#define E(T) Assert::AreEqual(T, it->str().c_str()); ++it;
        
        E(L"1");
        E(L"2.3");
        E(L"-4");
        E(L"-5.6");
        E(L"-7.");
        E(L"-8.9E+10");
        E(L"-11.12E13");
        E(L"matrix");
        E(L"scale");
        E(L"translate");
        E(L"(");
        E(L"14");
        E(L")");
        E(L"rotate");
        E(L"(");
        E(L"15");
        E(L",");
        E(L"16");
        E(L",");
        E(L"17");
        E(L",");
        E(L")");

#undef E
    }

    void CheckMatrix(float3x2 expectedMatrix, const wchar_t* string)
    {
        auto transform = transform_parser::parse(ref new Platform::String(string));
        auto tolerance = 0.001f;
        Assert::AreEqual(expectedMatrix.m11, transform.m11, tolerance, (std::wstring(string) + L" -> m11").c_str());
        Assert::AreEqual(expectedMatrix.m12, transform.m12, tolerance, (std::wstring(string) + L" -> m12").c_str());
        Assert::AreEqual(expectedMatrix.m21, transform.m21, tolerance, (std::wstring(string) + L" -> m21").c_str());
        Assert::AreEqual(expectedMatrix.m22, transform.m22, tolerance, (std::wstring(string) + L" -> m22").c_str());
        Assert::AreEqual(expectedMatrix.m31, transform.m31, tolerance, (std::wstring(string) + L" -> m31").c_str());
        Assert::AreEqual(expectedMatrix.m32, transform.m32, tolerance, (std::wstring(string) + L" -> m32").c_str());
    }

    TEST_METHOD(Transform_Matrix)
    {
        CheckMatrix(float3x2{1,2,3,4,5,6}, L"matrix(1 2 3 4 5 6)");
        CheckMatrix(float3x2{1,2,3,4,5,6}, L"matrix(1,2,3,4,5,6)");
        CheckMatrix(float3x2{1,2,3,4,5,6}, L"matrix(1, 2 ,3 ,4 ,5, 6)");
        CheckMatrix(float3x2{1,2,3,4,5,6}, L"matrix(1,2 3,4 5,6)");
        CheckMatrix(float3x2{-1,2,-3,4,-5,6}, L"matrix(-1,2 -3,4 -5,6)");
    }

    TEST_METHOD(Transform_Translate)
    {
        CheckMatrix(make_float3x2_translation(1, 0), L"translate(1)");
        CheckMatrix(make_float3x2_translation(-1, 0), L"translate(-1)");
        CheckMatrix(make_float3x2_translation(0.25, 0), L"translate(0.25)");

        CheckMatrix(make_float3x2_translation(0, 0), L"translate(0, 0)");
        CheckMatrix(make_float3x2_translation(0, -1), L"translate(0, -1)");
        CheckMatrix(make_float3x2_translation(0, 1), L"translate(0, 1)");
        CheckMatrix(make_float3x2_translation(0, 1), L"translate(0 1)");
        CheckMatrix(make_float3x2_translation(0, 0.25), L"translate(0 0.25)");

        CheckMatrix(make_float3x2_translation(210, 120), L"translate(210, 120)");
    }

    TEST_METHOD(Transform_Scale)
    {
        CheckMatrix(make_float3x2_scale(1, 1), L"scale(1)");
        CheckMatrix(make_float3x2_scale(-1, -1), L"scale(-1)");
        CheckMatrix(make_float3x2_scale(0.25, 0.25), L"scale(0.25)");

        CheckMatrix(make_float3x2_scale(0, 0), L"scale(0, 0)");
        CheckMatrix(make_float3x2_scale(0, -1), L"scale(0, -1)");
        CheckMatrix(make_float3x2_scale(0, 1), L"scale(0, 1)");
        CheckMatrix(make_float3x2_scale(0, 1), L"scale(0 1)");
        CheckMatrix(make_float3x2_scale(0, 0.25), L"scale(0 0.25)");
    }

    TEST_METHOD(Transform_Rotate)
    {
        CheckMatrix(make_float3x2_rotation(XMConvertToRadians(10)), L"rotate(10)");
        CheckMatrix(make_float3x2_rotation(XMConvertToRadians(-10)), L"rotate(-10)");
        CheckMatrix(make_float3x2_rotation(XMConvertToRadians(45)), L"rotate(45)");
        CheckMatrix(make_float3x2_rotation(XMConvertToRadians(45), float2{1, 2}), L"rotate(45 1 2)");
    }

    TEST_METHOD(Transform_SkewX)
    {
        CheckMatrix(make_float3x2_skew(0, 0), L"skewX(0)");
        CheckMatrix(make_float3x2_skew(XMConvertToRadians(45), 0), L"skewX(45)");
        CheckMatrix(make_float3x2_skew(XMConvertToRadians(-45), 0), L"skewX(-45)");
    }


    TEST_METHOD(Transform_SkewY)
    {
        CheckMatrix(make_float3x2_skew(0, 0), L"skewY(0)");
        CheckMatrix(make_float3x2_skew(0, XMConvertToRadians(45)), L"skewY(45)");
        CheckMatrix(make_float3x2_skew(0, XMConvertToRadians(-45)), L"skewY(-45)");
    }


    TEST_METHOD(Transform_Compound)
    {
        auto matrix =
            make_float3x2_scale(0.8f)
            * make_float3x2_skew(XMConvertToRadians(15), 0)
            * make_float3x2_rotation(XMConvertToRadians(45))
            * make_float3x2_translation(50, 50);
            
        CheckMatrix(matrix, L"translate(50, 50) rotate(45) skewX(15) scale(0.8)");
        CheckMatrix(matrix, L"translate(50, 50)rotate(45)skewX(15)scale(0.8)");
        CheckMatrix(matrix, L"translate(50, 50),rotate(45),skewX(15),scale(0.8)");
    }

#pragma endregion
#pragma region(Path)
    TEST_METHOD(Path_TokenizeRegex)
    {
        Platform::String^ s(L"M 100 101 L 202 203 M104 105L206 -207 M 0.6.5");

        std::regex_token_iterator<const wchar_t*> it(s->Begin(), s->Begin() + s->Length(), path_parser::sPathTokenRegex, 1);

#define E(T) Assert::AreEqual(T, it->str().c_str()); ++it;

        E(L"M");
        E(L"100");
        E(L"101");
        E(L"L");
        E(L"202");
        E(L"203");
        E(L"M");
        E(L"104");
        E(L"105");
        E(L"L");
        E(L"206");
        E(L"-207");
        E(L"M");
        E(L"0.6");
        E(L".5");

#undef E
    }

    void CheckPath(wchar_t const* path, wchar_t const* expected)
    {
        auto device = CanvasDevice::GetSharedDevice(true);
        auto geom = path_parser::parse(device, ref new Platform::String(path));
        auto receiver = ref new CanvasPathToString();
        geom->SendPathTo(receiver);

        Assert::AreEqual(expected, receiver->str().c_str());
    }

    TEST_METHOD(Path_MoveTo)
    {
        CheckPath(L"M1,2 3,4 5,6", L"BeginFigure(1, 2) AddLine(3, 4) AddLine(5, 6) EndFigure(open) ");
    }

    TEST_METHOD(Path_StraightLines_Absolute)
    {
        // from http://www.w3.org/Graphics/SVG/Test/20110816/svg/paths-data-04-t.svg
        CheckPath(L" M   62.00000   56.00000    L  113.96152  146.00000   L   10.03848  146.00000    L   62.00000   56.00000   Z    M   62.00000  71.00000   L  100.97114  138.50000   L   23.02886  138.50000   L   62.00000  71.00000   Z  ",
            L"BeginFigure(62, 56) "
            L"AddLine(113.962, 146) "
            L"AddLine(10.0385, 146) "
            L"AddLine(62, 56) "
            L"EndFigure(closed) "
            L"BeginFigure(62, 71) "
            L"AddLine(100.971, 138.5) "
            L"AddLine(23.0289, 138.5) "
            L"AddLine(62, 71) "
            L"EndFigure(closed) "
            );
    }

    TEST_METHOD(Path_StraightLines_Relative)
    {
        // from http://www.w3.org/Graphics/SVG/Test/20110816/svg/paths-data-05-t.svg
        CheckPath(L"m   62.00000  190.00000    l   51.96152   90.00000   l -103.92304    0.00000    l   51.96152  -90.00000   z    m    0.00000   15.00000   l   38.97114   67.50000   l  -77.91228    0.00000   l   38.97114  -67.50000   z  ",
            L"BeginFigure(62, 190) "
            L"AddLine(113.962, 280) "
            L"AddLine(10.0385, 280) "
            L"AddLine(62, 190) "
            L"EndFigure(closed) "
            L"BeginFigure(62, 205) "
            L"AddLine(100.971, 272.5) "
            L"AddLine(23.0589, 272.5) "
            L"AddLine(62.03, 205) "
            L"EndFigure(closed) "
            );
    }

    TEST_METHOD(Path_StraightLines_Relative_Implied_L)
    {
        // from http://www.w3.org/Graphics/SVG/Test/20110816/svg/paths-data-09-t.svg

        CheckPath(L"m 62.00000 190.00000    51.96152   90.00000   -103.92304    0.00000    51.96152  -90.00000   z    m 0.00000   15.00000   38.97114   67.50000   -77.91228    0.00000   38.97114  -67.50000   z",
            L"BeginFigure(62, 190) "
            L"AddLine(113.962, 280) "
            L"AddLine(10.0385, 280) "
            L"AddLine(62, 190) "
            L"EndFigure(closed) "
            L"BeginFigure(62, 205) "
            L"AddLine(100.971, 272.5) "
            L"AddLine(23.0589, 272.5) "
            L"AddLine(62.03, 205) "
            L"EndFigure(closed) ");
    }

    TEST_METHOD(Path_StraightLines_Relative_m_l)
    {
        // from http://www.w3.org/Graphics/SVG/Test/20110816/svg/paths-data-10-t.svg
        CheckPath(L"m 100 0 l 0 80 -100 -40 100 -40",
            L"BeginFigure(100, 0) "
            L"AddLine(100, 80) "
            L"AddLine(0, 40) "
            L"AddLine(100, 0) "
            L"EndFigure(open) ");
    }

    TEST_METHOD(Path_ParserEdgeCases)
    {
        // from http://www.w3.org/Graphics/SVG/Test/20110816/svg/paths-data-18-f.svg
        CheckPath(L"M 20 100 H 40#90",
            L"BeginFigure(20, 100) "
            L"AddLine(40, 100) "
            L"EndFigure(open) ");

        CheckPath(L"M 20 120 H 40.5 0.6",
            L"BeginFigure(20, 120) "
            L"AddLine(40.5, 120) "
            L"AddLine(0.6, 120) "
            L"EndFigure(open) ");

        CheckPath(L"M 20 120 H 40.5.6",
            L"BeginFigure(20, 120) "
            L"AddLine(40.5, 120) "
            L"AddLine(0.6, 120) "
            L"EndFigure(open) ");

        CheckPath(L"M 20 140 h 10-20",
            L"BeginFigure(20, 140) "
            L"AddLine(30, 140) "
            L"AddLine(10, 140) "
            L"EndFigure(open) ");
    }

    TEST_METHOD(Path_CloseResetsCurrentPointToBeginningOfPath)
    {
        // from http://www.w3.org/Graphics/SVG/Test/20110816/svg/paths-data-02-t.svg
        CheckPath(L"M372 130Q272 50 422 10zm70 0q50-150-80-90z",
            L"BeginFigure(372, 130) "
            L"AddQuadraticBezier(272, 50 422, 10) "
            L"EndFigure(closed) "
            L"BeginFigure(442, 130) "
            L"AddQuadraticBezier(492, -20 362, 40) "
            L"EndFigure(closed) "
            );
    }
#pragma endregion
#pragma region(ParseCommaSeparatedList)

    void CheckCommaSeparatedList(wchar_t const* src, std::initializer_list<wchar_t const*> expectedList)
    {
        std::vector<std::wstring> expected(expectedList.begin(), expectedList.end());

        auto actual = parse_comma_separated_list(ref new Platform::String(src));

        for (size_t i = 0; i < expected.size(); ++i)
        {
            if (actual.size() <= i)
            {
                Assert::AreEqual(expected.size(), actual.size(), (std::wstring(src) + L" (list sizes should match)").c_str());
            }

            Assert::AreEqual(expected[i], actual[i]);
        }
    }

    TEST_METHOD(ParseCommaSeparatedList)
    {
        CheckCommaSeparatedList(L"a,b,c,d", {L"a", L"b", L"c", L"d"});
        CheckCommaSeparatedList(L"   a   ,   b    ,   c   ,    d   ",  {L"a", L"b", L"c", L"d"});
        CheckCommaSeparatedList(L"\"a\",\"b\",c,\"d\"   ", { L"a", L"b", L"c", L"d" });
        CheckCommaSeparatedList(L"\"a b\",c", { L"a b", L"c" });
    }

#pragma endregion
#pragma region(preserveAspectRatio)

    TEST_METHOD(ParsePreserveAspectRatio)
    {
#define CHECK(STR, DEFER, ALIGN, SLICE) CheckPreserveAspectRatio(STR, DEFER, preserveAspectRatio::align::ALIGN, SLICE)
        CHECK(nullptr, false, xMidYMid, false);
        CHECK(L"", false, xMidYMid, false);
        CHECK(L"none", false, none, false);
        CHECK(L"xMinYMin", false, xMinYMin, false);
        CHECK(L"xMidYMin meet", false, xMidYMin, false);
        CHECK(L"xMaxYMin slice", false, xMaxYMin, true);
        CHECK(L"defer xMinYMid", true, xMinYMid, false);
        CHECK(L"defer xMaxYMid slice", true, xMaxYMid, true);
#undef CHECK
    }

    void CheckPreserveAspectRatio(wchar_t const* str, bool expectedDefer, preserveAspectRatio::align expectedAlign, bool expectedSlice)
    {
        preserveAspectRatio value(ref new Platform::String(str));

        Assert::AreEqual(expectedDefer, value.Defer, str);
        Assert::AreEqual((int)expectedAlign, (int)value.Align, str);
        Assert::AreEqual(expectedSlice, value.Slice, str);
    }

#pragma endregion
};
