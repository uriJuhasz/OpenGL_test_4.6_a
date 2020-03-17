#pragma once
#include "SceneSurfaceImpl.h"
#include "Scene/SceneObjects/SceneBezierPatch.h"

class SceneBezierPatchImpl
	: public virtual SceneSurfaceImpl
	, public virtual SceneBezierPatch
{
public:
	SceneBezierPatchImpl(const BezierPatch&);
};

