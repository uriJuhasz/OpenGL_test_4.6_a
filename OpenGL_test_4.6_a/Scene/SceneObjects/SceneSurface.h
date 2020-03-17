#pragma once
#include "SceneObject.h"
#include "Style/ColorRGBA.h"

class SceneSurface :
	public virtual SceneObject
{
public:
	virtual void setEdgeWidth(const float) = 0;
	virtual void setEdgeColor(const ColorRGBA&) = 0;

public:
	virtual float     getEdgeWidth() const = 0;
	virtual ColorRGBA getEdgeColor() const = 0;
};

