#pragma once
#include "SceneSurfaceImpl.h"
#include "Scene/SceneObjects/SceneMesh.h"

class SceneMeshImpl
	: public virtual SceneSurfaceImpl
	, public virtual SceneMesh
{
public:
	SceneMeshImpl(const Mesh& mesh)
		: m_mesh(mesh)
	{}
public:
	const Mesh& getMesh() const override { return m_mesh; }


	const std::vector<bool> getEdgeVisibility() const override { return std::vector<bool>(); }

public:
	void setEdgeVisibility(const int edgeIndex, const bool isVisible) override;

private:
	const Mesh& m_mesh;
	float m_edgeWidth = 1.0f;
	ColorRGB m_edgeColor = ColorRGB(1.0f, 1.0f, 1.0f);
};

