#pragma once
#include "OpenGLBackend/private/OpenGLWindow.h"

class OpenGLPrimitive
{
protected:
	explicit OpenGLPrimitive(OpenGLWindow& window) : m_window(window) {}

public:
	OpenGLWindow& getWindow() const { return m_window; }

protected:
	OpenGLWindow& m_window;
};

