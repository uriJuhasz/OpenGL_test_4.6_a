#pragma once

#include "Math/Vector.h"
#include "Math/Matrix.h"

#include <vector>
#include <memory>
using std::vector;
using std::unique_ptr;

class SceneObject
{
public:
    void setVisibility(const bool newVisitiblity) { m_visibility = newVisitiblity; }
    bool isVisible() const { return m_visibility; }

    void setTransformation(const Matrix4x4& newTransformation) { m_transformation = newTransformation; }
    Matrix4x4 getTransformation() const { return m_transformation; }

    virtual void addToCache() = 0;
    virtual void removeFromCache() = 0;
    virtual void render() = 0;

private:
    bool m_visibility;
    Matrix4x4 m_transformation;
};

class ColorRGB final
{
public:
    Vector3 m_value;
};

class Camera final
{
public:
    Vector3 m_position = Vector3(0.0f, 0.0f, -4.0f);
    Vector3 m_target = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_up = Vector3(0.0f, 1.0f, 0.0f);

    float m_near = 1.0;
    float m_far = 1000.0f;
    Vector2 m_field = Vector2(1.0f, 1.0f);

    Matrix4x4 makeViewMatrix() const
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

    Matrix4x4 makeProjectionMatrix(const float aspectRatio) const
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
};

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

class Scene1 final
{
public:
    Camera m_sceneCamera;
    vector<unique_ptr<PointLight>> m_pointLights;
    vector<unique_ptr<SceneObject>> m_objects;
};

