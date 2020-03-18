#include "OpenGLGraphicObject.h"

OpenGLGraphicObject::OpenGLGraphicObject(OpenGLScene& scene)
	: m_scene(scene)
{
}

void OpenGLGraphicObject::setVisibility(const bool isVisible)
{
	m_isVisible = isVisible;
}

bool OpenGLGraphicObject::isVisible() const
{
	return m_isVisible;
}

Matrix4x4 OpenGLGraphicObject::getTransformation() const
{
	return m_modelMatrix;
}

void OpenGLGraphicObject::setTransformation(const Matrix4x4& newTransformation)
{
	m_modelMatrix = newTransformation;
}
