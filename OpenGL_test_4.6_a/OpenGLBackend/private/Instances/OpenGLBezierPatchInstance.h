#pragma once
#include "OpenGLSurface.h"
#include "../Primitives/OpenGLBezierPatchPrimitive.h"
#include "Scene/SceneObjects/SceneBezierPatch.h"

class OpenGLBezierPatchInstance
	: public OpenGLSurface
	, public virtual SceneBezierPatch
{
public:
	explicit OpenGLBezierPatchInstance(const OpenGLBezierPatchPrimitive&);
	OpenGLBezierPatchInstance& createInstance() const override;

public:
	void render() override;
};

