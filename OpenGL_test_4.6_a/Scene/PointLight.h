#pragma once;

#include "ColorRGB.h"
#include "Math/Vector.h"

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
