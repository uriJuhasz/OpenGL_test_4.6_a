#pragma once
#include "Backend/BackendContext.h"

class OpenGLContext :  public BackendContext
{
public:
	static OpenGLContext* make();
};

