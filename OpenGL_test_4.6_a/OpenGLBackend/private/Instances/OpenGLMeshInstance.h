#pragma once
#include "OpenGLSurface.h"
#include "Backend/Scene/SceneObjects/SceneMesh.h"
#include "../Primitives/OpenGLMeshPrimitive.h"

class OpenGLWindow;
	
class OpenGLMeshInstance final
	: public OpenGLSurface
	, public virtual SceneMesh
{
public:
	OpenGLMeshInstance(const OpenGLMeshPrimitive&);
	~OpenGLMeshInstance();

public:
	const Mesh& getMesh() const override;

public:
	void renderMain() override;

public:
	void setSingleEdgeVisibility(const int edgeIndex, const bool isVisible) override {}
	const std::vector<bool> getEdgeVisibility() const override { return std::vector<bool>(); }


public:
	OpenGLMeshInstance& createInstance() const override;

public:
	const OpenGLMeshPrimitive& getPrimitive() const { return m_meshPrimitive; }

protected:
	BoundingBox getBoundingBox() const override;

private:
	const OpenGLMeshPrimitive& m_meshPrimitive;
};

