#pragma once
#include "Math/Matrix.h"

class SceneObject
{
public:
    void setVisibility(const bool newVisitiblity) { m_visibility = newVisitiblity; }
    bool isVisible() const { return m_visibility; }

    void setTransformation(const Matrix4x4& newTransformation) { m_transformation = newTransformation; }
    Matrix4x4 getTransformation() const { return m_transformation; }

    //    virtual void addToCache() = 0;
    //    virtual void removeFromCache() = 0;
    //    virtual void render() = 0;

private:
    bool m_visibility;
    Matrix4x4 m_transformation;
};

