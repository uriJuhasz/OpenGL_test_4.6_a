#pragma once
#include "OpenGLGraphicObject.h"
#include "Backend/GraphicObjects/BackendSurface.h"

class OpenGLSurface
	: public virtual OpenGLGraphicObject
	, public virtual BackendSurface
{
public:
	void setEdgesWidth(const float)  override;
	void setEdgesColor(const ColorRGBA&) override;

	void setEdgesVisibility(const bool) override;
	void setFacesVisibility(const bool) override;

protected:
	bool m_facesVisible = true;
	bool m_edgesVisible = true;

	float m_edgesWidth = 1.0f;
	ColorRGBA m_edgesColor = ColorRGBA::White;
};

