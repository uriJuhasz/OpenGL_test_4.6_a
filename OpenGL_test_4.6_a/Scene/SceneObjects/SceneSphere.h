#pragma once
#include "SceneSurface.h"

#include "Math/Vector.h"

class SceneSphere
	: public virtual SceneSurface
{
public:
	virtual void setRadius(const float) = 0;
	virtual float getRadius() const = 0;

	virtual Vector3 getCenter() const = 0;
};

