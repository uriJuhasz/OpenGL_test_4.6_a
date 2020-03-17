#pragma once

#include "BackendGraphicObject.h"
#include "Style/ColorRGBA.h"

class BackendMesh :
	public virtual BackendGraphicObject
{
public:
	virtual void setEdgeWidth(const float) = 0;
	virtual void setEdgeColor(const ColorRGBA&) = 0;
	virtual void setEdgeVisibility(const int edgeIndex, const bool isVisible) = 0;

public:
	virtual BackendMesh& createInstance() const override = 0;
};

