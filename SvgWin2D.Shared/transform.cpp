#include "pch.h"

#include "transform.h"

// TODO: this whole file is horribly repetitive

// TODO: figure out why 100.coords-transformattr-01-f.svg fails


/*static*/
std::wregex transform_parser::sTransformTokenRegex(
    L"[[:space:]]*"             // ignored whitespace
    L"("                        // a token could be:
    L"[,()]|"                   // one of , ( )
    L"[+-]?[0-9]+\\.[0-9]*([eE][+-]?[0-9]+)?|" // a number
    L"[+-]?[0-9]+|"             // an integer
    L"matrix|translate|scale|rotate|skewX|skewY" // a keyword
    L")"
    L"[[:space:]]*"             // ignored whitespace
    );


/*static*/
std::regex_token_iterator<const wchar_t*> transform_parser::end;

transform_parser::transform_parser(Platform::String^ string)
    : string_(string)
    , it_(string->Begin(), string->Begin() + string->Length(), sTransformTokenRegex, 1)
{
}


bool transform_parser::try_get_next(float3x2* transform)
{
    if (it_ == end)
        return false;

    // at this point we're expecting a keyword
    auto keyword = it_->str();
    ++it_;

    if (keyword == L"matrix")
        return try_get_matrix(transform);
    if (keyword == L"translate")
        return try_get_translate(transform);
    if (keyword == L"scale")
        return try_get_scale(transform);
    if (keyword == L"rotate")
        return try_get_rotate(transform);
    if (keyword == L"skewX")
        return try_get_skew_x(transform);
    if (keyword == L"skewY")
        return try_get_skew_y(transform);

    return false;
}


bool transform_parser::peek(const wchar_t* token)
{
    if (it_ == end)
        return false;

    auto nextToken = it_->str();
    if (nextToken != token)
        return false;
    
    return true;
}


bool transform_parser::expect(const wchar_t* token)
{
    if (it_ == end)
        return false;

    bool result = peek(token);
    ++it_;

    return result;
}


bool transform_parser::next_number(float* n)
{
    if (it_ == end)
        return false;

    auto nextToken = it_->str();
    ++it_;

    *n = static_cast<float>(_wtof(nextToken.c_str()));

    return true;
}


bool transform_parser::next_number_ignore_comma(float* n)
{
    if (it_ == end)
        return false;

    auto nextToken = it_->str();
    ++it_;

    if (nextToken == L",")
    {
        nextToken = it_->str();
        ++it_;
    }

    *n = static_cast<float>(_wtof(nextToken.c_str()));

    return true;
}


bool transform_parser::try_get_matrix(float3x2* transform)
{
    if (!expect(L"("))
        return false;

    if (!next_number(&transform->m11))
        return false;

    if (!next_number_ignore_comma(&transform->m12))
        return false;

    if (!next_number_ignore_comma(&transform->m21))
        return false;

    if (!next_number_ignore_comma(&transform->m22))
        return false;

    if (!next_number_ignore_comma(&transform->m31))
        return false;

    if (!next_number_ignore_comma(&transform->m32))
        return false;

    if (!expect(L")"))
        return false;

    return true;
}


bool transform_parser::try_get_translate(float3x2* transform)
{
    if (!expect(L"("))
        return false;

    float tx;
    float ty = 0;

    if (!next_number(&tx))
        return false;

    if (peek(L")"))
    {
        ++it_;
    }
    else
    {
        if (!next_number_ignore_comma(&ty))
            return false;

        if (!expect(L")"))
            return false;
    }

    *transform = Windows::Foundation::Numerics::make_float3x2_translation(tx, ty);
    return true;
}


bool transform_parser::try_get_scale(float3x2* transform)
{
    if (!expect(L"("))
        return false;

    float sx;
    float sy;

    if (!next_number(&sx))
        return false;

    if (peek(L")"))
    {
        ++it_;
        sy = sx;
    }
    else
    {
        if (!next_number_ignore_comma(&sy))
            return false;

        if (!expect(L")"))
            return false;
    }

    *transform = Windows::Foundation::Numerics::make_float3x2_scale(sx, sy);
    return true;
}


bool transform_parser::try_get_rotate(float3x2* transform)
{
    if (!expect(L"("))
        return false;

    float angle;
    if (!next_number(&angle))
        return false;

    angle = DirectX::XMConvertToRadians(angle);

    if (peek(L")"))
    {
        ++it_;

        *transform = Windows::Foundation::Numerics::make_float3x2_rotation(angle);
    }
    else
    {
        using Windows::Foundation::Numerics::float2;

        float cx, cy;
        if (!next_number_ignore_comma(&cx))
            return false;

        if (!next_number_ignore_comma(&cy))
            return false;

        *transform = Windows::Foundation::Numerics::make_float3x2_rotation(angle, float2{ cx, cy });
    }

    return true;
}


bool transform_parser::try_get_skew_x(float3x2* transform)
{
    if (!expect(L"("))
        return false;

    float angle;
    if (!next_number(&angle))
        return false;

    angle = DirectX::XMConvertToRadians(angle);

    if (!expect(L")"))
        return false;

    *transform = Windows::Foundation::Numerics::make_float3x2_skew(angle, 0);

    return true;
}


bool transform_parser::try_get_skew_y(float3x2* transform)
{
    if (!expect(L"("))
        return false;

    float angle;
    if (!next_number(&angle))
        return false;

    angle = DirectX::XMConvertToRadians(angle);

    if (!expect(L")"))
        return false;

    *transform = Windows::Foundation::Numerics::make_float3x2_skew(0, angle);

    return true;
}

