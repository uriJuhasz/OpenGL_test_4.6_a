#pragma once
#include "OpenGLSurface.h"
#include "Scene/SceneObjects/SceneMesh.h"
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
	void render() override;

public:
	void setEdgeVisibility(const int edgeIndex, const bool isVisible) override {}
	const std::vector<bool> getEdgeVisibility() const override { return std::vector<bool>(); }

public:
	OpenGLMeshInstance& createInstance() const override;

public:
	const OpenGLMeshPrimitive& getPrimitive() const { return m_meshPrimitive; }

private:
	const OpenGLMeshPrimitive& m_meshPrimitive;
};

