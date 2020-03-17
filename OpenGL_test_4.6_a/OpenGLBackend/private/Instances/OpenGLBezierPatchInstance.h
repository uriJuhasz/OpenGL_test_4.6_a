#pragma once
#include "OpenGLSurface.h"
#include "../Primitives/OpenGLBezierPatchPrimitive.h"
#include "Backend/GraphicObjects/BackendBezierPatch.h"

class OpenGLBezierPatchInstance
	: public OpenGLSurface
	, public virtual BackendBezierPatch
{
public:
	OpenGLBezierPatchInstance(const OpenGLBezierPatchPrimitive&);
	OpenGLBezierPatchInstance& createInstance() const override;

public:
	void render() override;
};

