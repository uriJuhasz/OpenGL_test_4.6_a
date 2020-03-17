#pragma once

#include "OpenGLContext.h"
#include "OpenGLShaderProgram.h"

#include "Backend/BackendWindow.h"

class OpenGLMeshInstance;

class OpenGLWindow : public BackendWindow
{
public:
	static OpenGLWindow* make(OpenGLContext&);

public:
	virtual OpenGLStandardShaderProgram& getMeshFaceShader() const = 0;
	virtual OpenGLStandardShaderProgram& getMeshEdgeShader() const = 0;

public:
	virtual OpenGLTessellationShaderProgram& getBezierPatchFaceShader() const = 0;
	virtual OpenGLTessellationShaderProgram& getBezierPatchEdgeShader() const = 0;

public:
	virtual OpenGLTessellationShaderProgram& getSphereEdgeShader() const = 0;

public:
	virtual OpenGLMeshInstance& makeInstance(const OpenGLMeshInstance&) = 0;
};
