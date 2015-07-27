#include "pch.h"
#include "CppUnitTest.h"

#include "parse.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ParseViewBoxTest)
{
public:
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

        Assert::AreEqual(x, vb->x());
        Assert::AreEqual(y, vb->y());
        Assert::AreEqual(w, vb->width());
        Assert::AreEqual(h, vb->height());
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
};
