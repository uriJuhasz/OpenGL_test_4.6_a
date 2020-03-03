#pragma once

#include <array>

class Matrix4x4 final
{
public:
    constexpr Matrix4x4() : m_value()
    {
        for (int i = 0; i < 16; ++i)
            m_value[i] = (i%4) ? 1.0f : 0.0f;
    }

    float  at(const int r, const int c) const { return m_value[r * 4 + c]; }
    float& at(const int r, const int c)       { return m_value[r * 4 + c]; }
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

constexpr Matrix4x4 unitMatrix4x4 = Matrix4x4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

inline Matrix4x4 makeTranslationMatrix(const Vector3& v)
{
    return Matrix4x4(
        1.0f, 0.0f, 0.0f, v[0],
        0.0f, 1.0f, 0.0f, v[1],
        0.0f, 0.0f, 1.0f, v[2],
        0.0f, 0.0f, 0.0f, 1.0f
    );
    /*
    return Matrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        v[0], v[1], v[2], 1.0f
    );*/
}
inline Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 r;
    for (int i=0; i<4; ++i)
        for (int j = 0; j < 4; ++j)
        {
            float v = 0.0f;
            for (int k = 0; k < 4; ++k)
                v += a.at(i,k) * b.at(k,j);
            r.at(i,j) = v;
        }
    return r;
}