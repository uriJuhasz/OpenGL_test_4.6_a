#pragma once
#pragma warning(disable: 26451)
#include <array>

#include "Vector.h"

class Matrix4x4;

inline Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);

class Matrix4x4 final
{
public:
    constexpr Matrix4x4() : m_value()
    {
        for (int i = 0; i < 16; ++i)
            m_value[i] = (i%4) ? 1.0f : 0.0f;
    }

    explicit constexpr Matrix4x4(const std::array<float, 16>& a) : m_value(a) {}
    constexpr Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
        : m_value{ m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,m23,m30,m31,m32,m33 } {}


    float  at(const int r, const int c) const { return m_value[r * 4 + c]; }
    float& at(const int r, const int c)       { return m_value[r * 4 + c]; }

public:
    Matrix4x4 operator+=(const Matrix4x4& other)
    {
        for (int i = 0; i < 16; ++i)
            m_value[i] += other.m_value[i];
        return *this;
    }
    Matrix4x4 operator-=(const Matrix4x4& other)
    {
        for (int i = 0; i < 16; ++i)
            m_value[i] -= other.m_value[i];
        return *this;
    }
    Matrix4x4 operator*=(const Matrix4x4& other)
    {
        *this = *this * other;
        return *this;
    }
public:
    float* data() { return m_value.data(); }
    const float* data() const { return m_value.data(); }
private:
    std::array<float, 16> m_value;
};

inline Matrix4x4 transpose(const Matrix4x4& m)
{
    return {
        m.at(0,0), m.at(1,0), m.at(2,0), m.at(3,0), 
        m.at(0,1), m.at(1,1), m.at(2,1), m.at(3,1),
        m.at(0,2), m.at(1,2), m.at(2,2), m.at(3,2),
        m.at(0,3), m.at(1,3), m.at(2,3), m.at(3,3)
    };
}

static const Matrix4x4 unitMatrix4x4 = Matrix4x4(
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
}
inline Matrix4x4 makeXRotationMatrix(const float angle)
{
    const auto cost = cosf(angle);
    const auto sint = sinf(angle);
    return Matrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cost, sint, 0.0f,
        0.0f, -sint, cost, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}
inline Matrix4x4 makeYRotationMatrix(const float angle)
{
    const auto cost = cosf(angle);
    const auto sint = sinf(angle);
    return Matrix4x4(
        cost, 0.0f, sint, 0.0f,
        0.0f, 1.0f,  0.0f, 0.0f,
       -sint, 0.0f, cost, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}
inline Matrix4x4 makeZRotationMatrix(const float angle)
{
    const auto cost = cosf(angle);
    const auto sint = sinf(angle);
    return Matrix4x4(
        cost, sint, 0.0f, 0.0f,
       -sint, cost, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
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

inline Vector4 operator*(const Matrix4x4& m, const Vector4 v)
{
    Vector4 r;
    for (int i = 0; i < 4; ++i)
    {
        float rv = 0.0f;
        for (int j = 0; j < 4; ++j)
            rv += m.at(i, j) * v[j];
        r[i] = rv;
    }
    return r;
}

inline Vector3 mulHomogeneous(const Matrix4x4& m, const Vector3& v)
{
    return makeNonHomogeneous(m * makeHomogeneous(v));
}

