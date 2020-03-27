#include "Camera.h"

Matrix4x4 Camera::makeViewMatrix() const
{
    const auto target = m_target;
    const auto position = m_position;
    const auto forward = normalize(target - position);
    const auto right = normalize(cross(forward, m_up));
    const auto up = cross(right, forward);

    return
    {
            right[0]  , right[1]  , right[2]  , -dot(right  ,position),
            up[0]     , up[1]     , up[2]     , -dot(up     ,position),
            forward[0], forward[1], forward[2], -dot(forward,position),
            0.0f      , 0.0f      , 0.0f      , 1.0f
    };
}

Matrix4x4 Camera::makeProjectionMatrix(const float aspectRatio) const
{
    // vpw / vph;
    const float ar = aspectRatio;
    const auto horizontal = ar >= 1.0f;
    const float w = m_field[0] * (horizontal ? ar : 1);
    const float h = m_field[1] * (horizontal ? 1 : 1 / ar);

    const float n = m_near;
    const float f = m_far;
    const float d = f - n;
    return
    {
        2 * n / w, 0.0f, 0.0f, 0.0f,
        0.0f, 2 * n / h, 0.0f, 0.0f,
        0.0f, 0.0f, (f + n) / d, -2 * f * n / d,
        0.0f, 0.0f, 1.0f, 0.0f
    };
}
