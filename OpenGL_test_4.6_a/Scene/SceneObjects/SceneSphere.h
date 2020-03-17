#pragma once
#include "SceneObject.h"

class SceneSphere
	: public virtual SceneObject
{
public:
	virtual void setRadius(const float) = 0;
};

