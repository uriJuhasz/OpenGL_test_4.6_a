#pragma once

#include <array>
using std::array;

//////////////////////////////////////////////////////////////////////
template<unsigned int D> class Vector final
{
public:
    Vector() 
    {
        for (int i = 0; i < D; ++i)
            m_value[i] = 0.0f;
    }
    Vector(const float x, const float y) : m_value{ x,y }
    {
        static_assert(D == 2);
    }
    Vector(const float x, const float y, const float z) : m_value{ x,y,z }
    {
        static_assert(D == 3);
    }
    Vector(const float x, const float y, const float z, const float w) : m_value{ x,y,z,w }
    {
        static_assert(D == 4);
    }
    const float& operator[](const int i) const { return m_value[i]; }
    float& operator[](const int i) { return m_value[i]; }

    const float* data() const { return m_value.data(); }
    float* data() { return m_value.data(); }
private:
    array<float, D> m_value;
};
typedef Vector<2> Vector2;
typedef Vector<3> Vector3;
typedef Vector<4> Vector4;

template<unsigned int D> inline float dot(const Vector<D>& a, const Vector<D>& b)
{
    float r = 0.0f;
    for (int i = 0; i < D; ++i)
        r += a[i] * b[i];
    return r;
}
template<> inline float dot(const Vector3& a, const Vector3& b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline Vector3 operator+(const Vector3 a, const Vector3 b)
{
    return Vector3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}
inline Vector3 operator-(const Vector3 a, const Vector3 b)
{
    return Vector3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}
inline Vector3 operator*(const Vector3 v, const float s)
{
    return Vector3(v[0] * s, v[1] * s, v[2] * s);
}
inline Vector3 operator*(const float s, const Vector3 v)
{
    return Vector3(v[0] * s, v[1] * s, v[2] * s);
}
inline Vector3 operator/(const Vector3 v, const float s)
{
    return Vector3(v[0] / s, v[1] / s, v[2] / s);
}
inline Vector3 cross(const Vector3 a, const Vector3 b)
{
    return Vector3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}
inline float length(const Vector3& v)
{
    return sqrtf(dot(v, v));
}

inline Vector3 normalize(const Vector3& v)
{
    return v / length(v);
}

