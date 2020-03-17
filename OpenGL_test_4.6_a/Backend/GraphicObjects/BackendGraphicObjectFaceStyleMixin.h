#pragma once

#include "BackendGraphicObject.h"
class BackendGraphicObjectFaceStyleMixin
	: public virtual BackendGraphicObject
{
public:
	virtual void setFacesVisibility(const bool) = 0;
	virtual void setFacesColor(const ColorRGBA&) = 0;
};
