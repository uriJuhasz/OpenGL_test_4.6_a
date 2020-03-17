#include "OpenGLMeshInstance.h"

OpenGLMeshInstance::OpenGLMeshInstance(const OpenGLMeshPrimitive& meshPrimitive)
	: m_meshPrimitive(meshPrimitive)
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

OpenGLWindow& OpenGLMeshInstance::getWindow() const { return m_meshPrimitive.getWindow(); }

BackendMesh& OpenGLMeshInstance::createInstance() const
{
	return getWindow().makeInstance(*this);
}

