#include "OpenGLSurface.h"

OpenGLSurface::OpenGLSurface(OpenGLScene& scene)
	: OpenGLGraphicObject(scene)
{
}

void OpenGLSurface::setEdgeWidth(const float newWidth)
{
	m_edgesWidth = newWidth;
}

void OpenGLSurface::setEdgeColor(const ColorRGBA& newColor)
{
	m_edgesColor = newColor;
}

void OpenGLSurface::setEdgeVisibility(const bool edgesVisible)
{
	m_edgesVisible = edgesVisible;
}
void OpenGLSurface::setFaceVisibility(const bool facesVisible)
{
	m_facesVisible = facesVisible;
}

void OpenGLSurface::setFaceFrontColor(const ColorRGBA& newColor)
{
	m_facesFrontColor = newColor;
}
void OpenGLSurface::setFaceBackColor(const ColorRGBA& newColor)
{
	m_facesBackColor = newColor;
}

float     OpenGLSurface::getEdgeWidth() const { return m_edgesWidth; }
ColorRGBA OpenGLSurface::getEdgeColor() const { return m_edgesColor; }
