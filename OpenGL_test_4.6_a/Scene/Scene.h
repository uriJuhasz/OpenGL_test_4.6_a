#pragma once

#include "SceneObject.h"
#include "Camera.h"
#include "PointLight.h"

class Scene
{
public:
    static Scene* makeScene();

    virtual const Camera& getCamera() const = 0;
    virtual void setCamera(const Camera& camera) = 0;
};

