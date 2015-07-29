#pragma once

using Windows::Foundation::Numerics::float3x2;

class transform_parser
{
    std::regex_token_iterator<const wchar_t*> it_;
    static std::regex_token_iterator<const wchar_t*> end;
    
public:
    static float3x2 parse(Platform::String^ string);

    static std::wregex sTransformTokenRegex;

private:
    transform_parser(Platform::String^ string);

    bool try_get_next(float3x2* transform);

    bool expect(const wchar_t* token);
    bool peek(const wchar_t* token);
    bool next_number(float* n);
    bool next_number_ignore_comma(float* n);

    bool try_get_matrix(float3x2* transform);
    bool try_get_translate(float3x2* transform);
    bool try_get_scale(float3x2* transform);
    bool try_get_rotate(float3x2* transform);
    bool try_get_skew_x(float3x2* transform);
    bool try_get_skew_y(float3x2* transform);
};