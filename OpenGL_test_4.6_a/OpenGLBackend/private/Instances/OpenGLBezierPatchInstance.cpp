#include "OpenGLBezierPatchInstance.h"

OpenGLBezierPatchInstance::OpenGLBezierPatchInstance(const OpenGLBezierPatchPrimitive& primitive)
	: OpenGLGraphicObject(primitive.getScene())
	, OpenGLSurface(primitive.getScene())
	, m_primitive(primitive)
{
}

OpenGLBezierPatchInstance& OpenGLBezierPatchInstance::createInstance() const
{
	return getScene().makeInstance(*this);
}

void OpenGLBezierPatchInstance::render()
{
	if (!isVisible())
		return;

	if (m_facesVisible)
	{
		auto& faceShader = m_primitive.getFaceShader();

		faceShader.setParameter("modelMatrix", m_modelMatrix);
	}
	if (m_edgesVisible)
	{
		auto& edgeShader = m_primitive.getEdgeShader();

		edgeShader.setParameter("modelMatrix", m_modelMatrix);
		edgeShader.setParameter("edgeColor", m_edgesColor.m_value);

		glLineWidth(m_edgesWidth);
	}
	m_primitive.render(m_facesVisible,m_edgesVisible);
}

const OpenGLBezierPatchPrimitive& OpenGLBezierPatchInstance::getPrimitive() const
{
	return m_primitive;
}
