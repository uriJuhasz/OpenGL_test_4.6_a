#pragma once
#include "OpenGLSurface.h"
#include "Backend/GraphicObjects/BackendSphere.h"

#include "OpenGLBackend/private/OpenGLWindow.h"

class OpenGLSphere
	: public virtual OpenGLSurface
	, public virtual BackendSphere
{
public:
	explicit OpenGLSphere(OpenGLWindow& window, const float radius);

public:
	void render() override;

private:
	OpenGLWindow& m_window;
	float m_radius = 1.0f;
};

