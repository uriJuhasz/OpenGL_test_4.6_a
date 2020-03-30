#pragma once

#include "Math/Vector.h"
#include "Math/Matrix.h"

class Camera final
{
public:
    Vector3 m_position = Vector3(0.0f, 0.0f, -4.0f);
    Vector3 m_target = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_up = Vector3(0.0f, 1.0f, 0.0f);

    float m_near = 0.1f;
    float m_far = 1000.0f;
    Vector2 m_field = Vector2(0.1f, 0.1f);

    Matrix4x4 makeViewMatrix() const;
    Matrix4x4 makeProjectionMatrix(const float aspectRatio) const;
};
