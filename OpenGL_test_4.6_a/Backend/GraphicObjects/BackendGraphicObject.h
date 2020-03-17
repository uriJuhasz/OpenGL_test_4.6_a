#pragma once
#include "Math/Matrix.h"
class BackendGraphicObject
{
protected:
	BackendGraphicObject() {}

public:
	virtual ~BackendGraphicObject() {}

public:
    virtual void setVisibility(const bool isVisible) = 0;

    virtual void setModelMatrix(const Matrix4x4& newTransformation) = 0;

public:
	virtual BackendGraphicObject& createInstance() const = 0;
};

