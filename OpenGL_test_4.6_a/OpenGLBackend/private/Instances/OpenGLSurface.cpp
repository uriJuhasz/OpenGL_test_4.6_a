#include "OpenGLSurface.h"

void OpenGLSurface::setEdgesWidth(const float newWidth)
{
	m_edgesWidth = newWidth;
}

void OpenGLSurface::setEdgesColor(const ColorRGBA& newColor)
{
	m_edgesColor = newColor;
}

void OpenGLSurface::setEdgesVisibility(const bool edgesVisible)
{
	m_edgesVisible = edgesVisible;
}
void OpenGLSurface::setFacesVisibility(const bool facesVisible)
{
	m_facesVisible = facesVisible;
}
