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
};
const ColorRGBA ColorRGBA::Black = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
const ColorRGBA ColorRGBA::White = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
const ColorRGBA ColorRGBA::Red   = ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
const ColorRGBA ColorRGBA::Green = ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
const ColorRGBA ColorRGBA::Blue  = ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);

