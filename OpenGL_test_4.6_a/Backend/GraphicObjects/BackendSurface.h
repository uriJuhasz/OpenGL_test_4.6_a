#pragma once
#include "BackendGraphicObject.h"
#include "BackendGraphicObjectEdgeStyleMixin.h"
#include "BackendGraphicObjectFaceStyleMixin.h"

class BackendSurface
	: public virtual BackendGraphicObject
	, public virtual BackendGraphicObjectEdgeStyleMixin
	, public virtual BackendGraphicObjectFaceStyleMixin
{
};

