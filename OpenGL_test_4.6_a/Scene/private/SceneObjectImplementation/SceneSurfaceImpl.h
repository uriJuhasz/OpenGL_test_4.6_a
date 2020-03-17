#pragma once
#include "SceneObjectImpl.h"
#include "Scene/SceneObjects/SceneSurface.h"

class SceneSurfaceImpl
	: public virtual SceneObjectImpl
	, public virtual SceneSurface
{
public:
	void setEdgeWidth(const float edgeWidth) override { m_edgeWidth = edgeWidth; }
	void setEdgeColor(const ColorRGBA& edgeColor) override { m_edgeColor = edgeColor; }

public:
	float    getEdgeWidth() const override { return m_edgeWidth; }
	ColorRGBA getEdgeColor() const override { return m_edgeColor; }
private:
	float m_edgeWidth = 1.0f;
	ColorRGBA m_edgeColor = ColorRGBA::White;
};

