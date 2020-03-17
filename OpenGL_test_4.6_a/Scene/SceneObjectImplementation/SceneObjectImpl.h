#pragma once
#include "Scene/SceneObjects/SceneObject.h"

class SceneObjectImpl : public virtual SceneObject
{
public:
    void setVisibility(const bool newVisitiblity) override { m_visibility = newVisitiblity; }
    bool isVisible() const override { return m_visibility; }

    void setTransformation(const Matrix4x4& newTransformation) override { m_transformation = newTransformation; }
    Matrix4x4 getTransformation() const override { return m_transformation; }

private:
    bool m_visibility = true;
    Matrix4x4 m_transformation = unitMatrix4x4;
};

