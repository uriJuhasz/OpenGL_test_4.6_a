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
	m_primitive.render(m_facesVisible,m_edgesVisible);
}

const OpenGLBezierPatchPrimitive& OpenGLBezierPatchInstance::getPrimitive() const
{
	return m_primitive;
}
