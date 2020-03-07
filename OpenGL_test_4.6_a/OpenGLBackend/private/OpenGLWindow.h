#pragma once

#include "OpenGLContext.h"

#include "Backend/BackendWindow.h"

class OpenGLWindow : public BackendWindow
{
public:
	static OpenGLWindow* make(OpenGLContext&);
};
