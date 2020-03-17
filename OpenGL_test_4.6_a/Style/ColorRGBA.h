#pragma once
#include "Math/Vector.h"

class ColorRGBA final
{
public:
    ColorRGBA(const float r, const float g, const float b, const float a)
        : m_value(r, g, b, a)
    {}
    Vector4 m_value;
};

