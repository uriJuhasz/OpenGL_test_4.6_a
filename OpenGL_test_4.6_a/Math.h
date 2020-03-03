#ifndef Math_H_
#define Math_H_

#include <array>
using std::array;

//////////////////////////////////////////////////////////////////////
template<unsigned int D> class Vector final
{
public:
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

template<unsigned int D> float dot(const Vector<D>& a, const Vector<D>& b)
{
    float r = 0.0f;
    for (int i = 0; i < D; ++i)
        r += a[i] * b[i];
    return r;
}
template<> float dot(const Vector3& a, const Vector3& b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector3 operator+(const Vector3 a, const Vector3 b)
{
    return Vector3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}
Vector3 operator-(const Vector3 a, const Vector3 b)
{
    return Vector3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}
Vector3 operator*(const Vector3 v, const float s)
{
    return Vector3(v[0] * s, v[1] * s, v[2] * s);
}
Vector3 operator*(const float s, const Vector3 v)
{
    return Vector3(v[0] * s, v[1] * s, v[2] * s);
}
Vector3 operator/(const Vector3 v, const float s)
{
    return Vector3(v[0] / s, v[1] / s, v[2] / s);
}
Vector3 cross(const Vector3 a, const Vector3 b)
{
    return Vector3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}
float length(const Vector3& v)
{
    return sqrtf(dot(v, v));
}

Vector3 normalize(const Vector3& v)
{
    return v / length(v);
}

class Matrix44 final
{
public:
    explicit constexpr Matrix44(const array<float, 16>& a) : m_value(a) {}
    constexpr Matrix44(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
        : m_value{ m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,m23,m30,m31,m32,m33 } {}
    float* data() { return m_value.data(); }
    const float* data() const { return m_value.data(); }
private:
    array<float, 16> m_value;
};


constexpr Matrix44 unitMatrix44 = Matrix44(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);
const array<Vector3, 4> vertices = {
   Vector3(-1.0f,  -1.0f,  0.0f),
   Vector3(1.0f,  -1.0f,  0.0f),
   Vector3(1.0f,   1.0f,  0.0f),
   Vector3(-1.0f,   1.0f,  0.0f)
};

const array<Vector3, 4> normals = {
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f)
};

const array<Vector2, 4> textureUV = {
   Vector2(0.0f,  0.0f),
   Vector2(1.0f,  0.0f),
   Vector2(1.0f,  1.0f),
   Vector2(0.0f,  1.0f)
};

#endif //Math_H_
