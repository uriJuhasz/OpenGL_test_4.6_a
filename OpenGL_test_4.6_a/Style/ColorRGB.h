#pragma once

#include "Math/Vector.h"

class ColorRGB final
{
public:
    constexpr ColorRGB(const float r, const float g, const float b)
        : m_value(r, g, b)
    {}
    Vector3 m_value;

    static const ColorRGB White;
};

