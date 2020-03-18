#pragma once
#include "OpenGLBackend/private/OpenGLScene.h"

class OpenGLPrimitive
{
protected:
	explicit OpenGLPrimitive(OpenGLScene& scene) : m_scene(scene) {}

public:
	OpenGLScene& getScene() const { return m_scene; }

protected:
	OpenGLScene& m_scene;
};

