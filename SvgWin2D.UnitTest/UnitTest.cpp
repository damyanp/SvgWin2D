#include "pch.h"
#include "CppUnitTest.h"

#include "parse.h"

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
};
