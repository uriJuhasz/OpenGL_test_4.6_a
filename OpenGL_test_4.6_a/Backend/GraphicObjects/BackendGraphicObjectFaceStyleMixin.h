#pragma once

#include "BackendGraphicObject.h"
class BackendGraphicObjectFaceStyleMixin
	: public virtual BackendGraphicObject
{
public:
	virtual void setFacesVisibility(const bool) = 0;
	virtual void setFacesFrontColor(const ColorRGBA&) = 0;
	virtual void setFacesBackColor(const ColorRGBA&) = 0;
};
