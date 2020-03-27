#pragma once
#include "Math/Vector.h"

class ColorRGBA final
{
public:
    constexpr explicit ColorRGBA(const float r, const float g, const float b, const float a)
        : m_value(r, g, b, a)
    {}
    Vector4 m_value;

    static const ColorRGBA White;
    static const ColorRGBA Black;
    static const ColorRGBA Red;
    static const ColorRGBA Green;
    static const ColorRGBA Blue;
    static const ColorRGBA Yellow;
};


