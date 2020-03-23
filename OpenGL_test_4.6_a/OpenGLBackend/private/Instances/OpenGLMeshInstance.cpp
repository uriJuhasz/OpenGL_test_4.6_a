#include "OpenGLMeshInstance.h"

#include "Utilities/Misc.h"

using std::vector;
using std::array;
using std::min;
using std::max;

OpenGLMeshInstance::OpenGLMeshInstance(const OpenGLMeshPrimitive& meshPrimitive)
	: OpenGLGraphicObject(meshPrimitive.getScene())
	, OpenGLSurface(meshPrimitive.getScene())
	, m_meshPrimitive(meshPrimitive)
{
}

OpenGLMeshInstance::~OpenGLMeshInstance()
{
}

const Mesh& OpenGLMeshInstance::getMesh() const
{
	return m_meshPrimitive.getMesh();
}

void OpenGLMeshInstance::renderMain()
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

OpenGLGraphicObject::BoundingBox OpenGLMeshInstance::getBoundingBox() const
{
	return calculateBoundingBox(m_meshPrimitive.getMesh().m_vertices);
}

