#pragma once
#include "OpenGLSurface.h"
#include "Backend/GraphicObjects/BackendMesh.h"
#include "../Primitives/OpenGLMeshPrimitive.h"

class OpenGLWindow;
	
class OpenGLMeshInstance final
	: public OpenGLSurface
	, public virtual BackendMesh
{
public:
	OpenGLMeshInstance(const OpenGLMeshPrimitive&);

public:
	void render() override;

public:
	void setEdgeVisibility(const int edgeIndex, const bool isVisible) override {}

public:
	virtual BackendMesh& createInstance() const override;

public:
	OpenGLWindow& getWindow() const;

	const OpenGLMeshPrimitive& getPrimitive() const { return m_meshPrimitive; }

private:
	const OpenGLMeshPrimitive& m_meshPrimitive;

};

