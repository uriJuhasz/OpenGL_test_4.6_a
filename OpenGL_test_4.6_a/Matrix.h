#pragma once

#include <array>

class Matrix4x4 final
{
public:
    explicit constexpr Matrix4x4(const std::array<float, 16>& a) : m_value(a) {}
    constexpr Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
        : m_value{ m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,m23,m30,m31,m32,m33 } {}
    float* data() { return m_value.data(); }
    const float* data() const { return m_value.data(); }
private:
    std::array<float, 16> m_value;
};

constexpr Matrix4x4 unitMatrix44 = Matrix4x4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);
