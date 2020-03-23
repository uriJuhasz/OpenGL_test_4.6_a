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
	const OpenGLBezierPatchPrimitive& getPrimitive() const;

public:
	void renderMain() override;

public:
	BoundingBox getBoundingBox() const override;

private:
	const OpenGLBezierPatchPrimitive& m_patchPrimitive;
};

