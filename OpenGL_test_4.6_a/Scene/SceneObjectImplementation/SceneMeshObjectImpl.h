#pragma once
#include "SceneObjectImpl.h"
#include "Scene/SceneObjects/SceneMeshObject.h"

#include <memory>

class SceneMeshObjectImpl
	: public SceneObjectImpl
	, public virtual SceneMeshObject
{
public:
	SceneMeshObjectImpl(const Mesh& mesh)
		: m_mesh(mesh)
	{}
public:
	const Mesh& getMesh() const override { return m_mesh; }

	float    getEdgeWidth() const override { return m_edgeWidth;}
	ColorRGB getEdgeColor() const override { return m_edgeColor; }

	const std::vector<bool> getEdgeVisibility() const override { return std::vector<bool>(); }

public:
	void setEdgeWidth(const float edgeWidth) override;
	void setEdgeColor(const ColorRGB& edgeColor) override;
	void setEdgeVisibility(const int edgeIndex, const bool isVisible) override;

private:
	const Mesh& m_mesh;
	float m_edgeWidth = 1.0f;
	ColorRGB m_edgeColor = ColorRGB(1.0f, 1.0f, 1.0f);
};

