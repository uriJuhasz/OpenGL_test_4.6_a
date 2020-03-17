#include "OpenGLGraphicObject.h"

void OpenGLGraphicObject::setVisibility(const bool isVisible)
{
	m_isVisible = isVisible;
}

void OpenGLGraphicObject::setModelMatrix(const Matrix4x4& newTransformation)
{
	m_modelMatrix = newTransformation;
}
