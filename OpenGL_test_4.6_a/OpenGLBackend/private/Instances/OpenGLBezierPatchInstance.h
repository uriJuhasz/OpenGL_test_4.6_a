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
	void render() override;

private:
	const OpenGLBezierPatchPrimitive& m_primitive;
};

