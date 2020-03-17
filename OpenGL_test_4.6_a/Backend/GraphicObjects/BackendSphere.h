#pragma once
#include "BackendSurface.h"

class BackendSphere
	: public virtual BackendSurface
{
public:
	virtual void setRadius(const float) const = 0;
};

