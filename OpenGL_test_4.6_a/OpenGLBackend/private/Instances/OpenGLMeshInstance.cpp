#include "OpenGLMeshInstance.h"

OpenGLMeshInstance::OpenGLMeshInstance(const OpenGLMeshPrimitive& meshPrimitive)
	: m_meshPrimitive(meshPrimitive)
{}

void OpenGLMeshInstance::setEdgeWidth(const float newWidth)
{
	m_edgeWidth = newWidth;
}

void OpenGLMeshInstance::setEdgeColor(const ColorRGBA& newColor)
{
	m_edgeColor = newColor;
}

void OpenGLMeshInstance::setEdgesVisibility(const bool edgesVisible)
{
	m_edgesVisible = edgesVisible;
}
void OpenGLMeshInstance::setFacesVisibility(const bool facesVisible)
{
	m_facesVisible = facesVisible;
}

void OpenGLMeshInstance::render()
{
	if (m_facesVisible)
	{
		auto& faceShader = m_meshPrimitive.getFaceShader();

		faceShader.setParameter("modelMatrix", m_modelMatrix);
	}
	if (m_edgesVisible)
	{
		auto& edgeShader = m_meshPrimitive.getEdgeShader();

		edgeShader.setParameter("modelMatrix", m_modelMatrix);
		edgeShader.setParameter("edgeColor", m_edgeColor.m_value);
	}
	m_meshPrimitive.render(m_facesVisible, m_edgesVisible);
}

OpenGLWindow& OpenGLMeshInstance::getWindow() const { return m_meshPrimitive.getWindow(); }

BackendMesh& OpenGLMeshInstance::createInstance() const
{
	return getWindow().makeInstance(*this);
}

