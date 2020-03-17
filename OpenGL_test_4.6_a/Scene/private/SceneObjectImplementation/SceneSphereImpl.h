#pragma once

#include "SceneSurfaceImpl.h"
#include "Scene/SceneObjects/SceneSphere.h"

class SceneSphereImpl
	: public virtual SceneSurfaceImpl
	, public virtual SceneSphere
{
public:
	SceneSphereImpl(const Vector3&, const float);

	void setRadius(const float) override;
};
