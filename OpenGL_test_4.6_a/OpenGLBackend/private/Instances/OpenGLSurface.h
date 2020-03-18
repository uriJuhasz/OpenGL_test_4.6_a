#pragma once
#include "OpenGLGraphicObject.h"
#include "Scene/SceneObjects/SceneSurface.h"

class OpenGLSurface
	: public virtual OpenGLGraphicObject
	, public virtual SceneSurface
{
public:
	explicit OpenGLSurface(OpenGLScene& scene);

public:
	void setEdgeVisibility(const bool) override;
	void setEdgeColor(const ColorRGBA&) override;
	void setEdgeWidth(const float)  override;

	void setFaceVisibility(const bool) override;
	void setFaceFrontColor(const ColorRGBA&) override;
	void setFaceBackColor(const ColorRGBA&) override;

public:
	virtual float     getEdgeWidth() const override;
	virtual ColorRGBA getEdgeColor() const override;

protected:
	bool m_facesVisible = true;
	ColorRGBA m_facesFrontColor = ColorRGBA::Red;
	ColorRGBA m_facesBackColor = ColorRGBA::Blue;

	bool m_edgesVisible = true;
	float m_edgesWidth = 1.0f;
	ColorRGBA m_edgesColor = ColorRGBA::White;

};

