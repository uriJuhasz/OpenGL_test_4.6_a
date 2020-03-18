#include "OpenGLMeshInstance.h"

OpenGLMeshInstance::OpenGLMeshInstance(const OpenGLMeshPrimitive& meshPrimitive)
	: OpenGLGraphicObject(meshPrimitive.getScene())
	, OpenGLSurface(meshPrimitive.getScene())
	, m_meshPrimitive(meshPrimitive)
{}


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
		edgeShader.setParameter("edgeColor", m_edgesColor.m_value);
	}
	m_meshPrimitive.render(m_facesVisible, m_edgesVisible);
}

OpenGLMeshInstance& OpenGLMeshInstance::createInstance() const
{
	return getScene().makeInstance(*this);
}

