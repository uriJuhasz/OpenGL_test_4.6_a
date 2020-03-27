#pragma once
#include "SceneObject.h"
#include "Style/ColorRGBA.h"

class SceneSurface :
	public virtual SceneObject
{
public:
	virtual void setFaceVisibility(const bool) = 0;
	virtual void setFaceFrontColor(const ColorRGBA& newColor) = 0;
	virtual void setFaceBackColor(const ColorRGBA& newColor) = 0;

public:
	virtual void setEdgeVisibility(const bool) = 0;
	virtual void setEdgeWidth(const float) = 0;
	virtual void setEdgeColor(const ColorRGBA&) = 0;

public:
	virtual float     getEdgeWidth() const = 0;
	virtual ColorRGBA getEdgeColor() const = 0;
};

