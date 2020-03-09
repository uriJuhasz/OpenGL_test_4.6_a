#pragma once

#include "Math/Vector.h"

class BackendViewInterface
{
protected:
    BackendViewInterface() {}
    
public:
    virtual ~BackendViewInterface() {}

    virtual void setupScene() = 0;
    virtual void renderScene() = 0;

    virtual void mouseWheelCallback(const Vector2& wheelDelta) = 0;
    virtual void mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos) = 0;

};

