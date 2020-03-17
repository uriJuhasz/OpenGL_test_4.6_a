#pragma once;

#include "Math/Vector.h"

#include "Style/ColorRGB.h"

class PointLight final
{
public:
    bool m_visibility;
    Vector3 m_position;
    ColorRGB m_color;
    float m_diffuseCoefficient;
    float m_specularExponent;
    float m_specularCoefficient;
};
