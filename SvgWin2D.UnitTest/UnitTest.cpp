#include "pch.h"
#include "CppUnitTest.h"

#include "parse.h"
#include "transform.h"

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
        transform_parser parser(ref new Platform::String(string));
        float3x2 transform;
        Assert::IsTrue(parser.try_get_next(&transform), (std::wstring(string) + L" -- try_get_next").c_str());
        Assert::AreEqual(expectedMatrix.m11, transform.m11, (std::wstring(string) + L" -> m11").c_str());
        Assert::AreEqual(expectedMatrix.m12, transform.m12, (std::wstring(string) + L" -> m12").c_str());
        Assert::AreEqual(expectedMatrix.m21, transform.m21, (std::wstring(string) + L" -> m21").c_str());
        Assert::AreEqual(expectedMatrix.m22, transform.m22, (std::wstring(string) + L" -> m22").c_str());
        Assert::AreEqual(expectedMatrix.m31, transform.m31, (std::wstring(string) + L" -> m31").c_str());
        Assert::AreEqual(expectedMatrix.m32, transform.m32, (std::wstring(string) + L" -> m32").c_str());
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

#pragma endregion
};
