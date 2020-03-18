#pragma once;

#include "Math/Vector.h"

#include "Style/ColorRGB.h"

class PointLight final
{
public:
    bool m_visibility = true;
    Vector3 m_position = Vector3(0.0f, 0.0f, 0.0f);
    ColorRGB m_color = ColorRGB::White;
    float m_diffuseCoefficient = 1.0f;
    float m_specularExponent = 10.0f;
    float m_specularCoefficient = 1.0f;
};
