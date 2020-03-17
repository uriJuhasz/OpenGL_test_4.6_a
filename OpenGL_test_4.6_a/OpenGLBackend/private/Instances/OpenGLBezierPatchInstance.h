#pragma once
#include "OpenGLGraphicObject.h"
#include "../Primitives/OpenGLBezierPatchPrimitive.h"
#include "Backend/GraphicObjects/BackendBezierPatch.h"

class OpenGLBezierPatchInstance
	: public OpenGLGraphicObject
	, public virtual BackendBezierPatch
{
public:
	OpenGLBezierPatchInstance(const OpenGLBezierPatchPrimitive&);
	OpenGLBezierPatchInstance& createInstance() const override;

public:
	void render() override;
};

