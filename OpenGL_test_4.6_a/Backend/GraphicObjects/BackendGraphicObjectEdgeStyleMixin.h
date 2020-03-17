#pragma once
#include "BackendGraphicObject.h"
#include "Style/ColorRGBA.h"

class BackendGraphicObjectEdgeStyleMixin
	: public virtual BackendGraphicObject
{
public:
	virtual void setEdgesVisibility(const bool) = 0;
	virtual void setEdgesColor(const ColorRGBA&) = 0;
	virtual void setEdgesWidth(const float) = 0;

};

