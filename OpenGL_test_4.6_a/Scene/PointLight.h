#pragma once;

#include "Math/Vector.h"

#include "Style/ColorRGB.h"

class PointLight final
{
public:
    void setPosition(const Vector3 newPosition) { m_position = newPosition; }
    Vector3 getPosition() const { return m_position; }
    bool m_visibility = true;
    ColorRGB m_color = ColorRGB::White;
    float m_diffuseCoefficient = 1.0f;
    float m_specularExponent = 100.0f;
    float m_specularCoefficient = 1.0f;
private:
    Vector3 m_position = Vector3(0.0f, 0.0f, 0.0f);
};
