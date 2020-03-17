#pragma once
#include "OpenGLGraphicObject.h"
#include "Backend/GraphicObjects/BackendMesh.h"
#include "../Primitives/OpenGLMeshPrimitive.h"

class OpenGLWindow;
	
class OpenGLMeshInstance final
	: public OpenGLGraphicObject
	, public virtual BackendMesh
{
public:
	OpenGLMeshInstance(const OpenGLMeshPrimitive&);

public:
	void setEdgeWidth(const float)  override;
	void setEdgeColor(const ColorRGBA&) override;
	void setEdgeVisibility(const int edgeIndex, const bool isVisible) override {}

	void setEdgesVisibility(const bool);
	void setFacesVisibility(const bool);

public:
	void render() override;

public:
	virtual BackendMesh& createInstance() const override;

public:
	OpenGLWindow& getWindow() const;

	const OpenGLMeshPrimitive& getPrimitive() const { return m_meshPrimitive; }

private:
	const OpenGLMeshPrimitive& m_meshPrimitive;
	
	bool m_facesVisible = true;
	
	bool m_edgesVisible = false;
	float m_edgeWidth = 1.0f;
	ColorRGBA m_edgeColor = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
};

