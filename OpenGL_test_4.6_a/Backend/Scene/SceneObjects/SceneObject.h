#pragma once
#include "Math/Matrix.h"

class SceneObject
{
private:
    SceneObject(const SceneObject&) = delete;
    SceneObject(SceneObject&&) = delete;
    SceneObject& operator=(const SceneObject&) = delete;
    SceneObject& operator=(SceneObject&&) = delete;

protected:
    SceneObject() {}

public:
    virtual ~SceneObject() {}

public:
    virtual SceneObject& createInstance() const = 0;

public:
    virtual void setVisibility(const bool newVisitiblity) = 0;
    virtual bool isVisible() const = 0;

    virtual void setModelMatrix(const Matrix4x4& newTransformation) = 0;
    virtual Matrix4x4 getModelMatrix() const = 0;

public:
    virtual void setBoundingBoxVisibility(const bool) = 0;

};

