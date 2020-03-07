#pragma once

#include "Math/Vector.h"

class ViewInterface
{
protected:
    ViewInterface() {}
    
public:
    virtual ~ViewInterface() {}

    virtual void initOpenGL() = 0;
    virtual void setupScene() = 0;
    virtual void renderScene() = 0;

    virtual void mouseWheelCallback(const Vector2& wheelDelta) = 0;
    virtual void mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos) = 0;

};

