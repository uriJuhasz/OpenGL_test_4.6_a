#include "OpenGLBezierPatchInstance.h"

#include "Utilities/Misc.h"

OpenGLBezierPatchInstance::OpenGLBezierPatchInstance(const OpenGLBezierPatchPrimitive& primitive)
	: OpenGLGraphicObject(primitive.getScene())
	, OpenGLSurface(primitive.getScene())
	, m_patchPrimitive(primitive)
{
}

OpenGLBezierPatchInstance& OpenGLBezierPatchInstance::createInstance() const
{
	return getScene().makeInstance(*this);
}

void OpenGLBezierPatchInstance::renderMain()
{
	if (m_facesVisible)
	{
		auto& faceShader = m_patchPrimitive.getFaceShader();

		faceShader.setParameter("modelMatrix", m_modelMatrix);
	}
	if (m_edgesVisible)
	{
		auto& edgeShader = m_patchPrimitive.getEdgeShader();

		edgeShader.setParameter("modelMatrix", m_modelMatrix);
		edgeShader.setParameter("edgeColor", m_edgesColor.m_value);

		glLineWidth(m_edgesWidth);
	}
	m_patchPrimitive.render(m_facesVisible,m_edgesVisible);
}

OpenGLGraphicObject::BoundingBox OpenGLBezierPatchInstance::getBoundingBox() const
{
	return calculateBoundingBox(toVector(m_patchPrimitive.getPatch().getVertices()));
}

const OpenGLBezierPatchPrimitive& OpenGLBezierPatchInstance::getPrimitive() const
{
	return m_patchPrimitive;
}
