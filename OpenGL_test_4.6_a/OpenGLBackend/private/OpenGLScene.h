#pragma once
#include "Scene/Scene.h"
#include "OpenGLShaderProgram.h"
#include <memory>

class OpenGLWindow;
class OpenGLMeshInstance;

class OpenGLScene :
	public virtual Scene
{
public:
	static std::unique_ptr<OpenGLScene> makeScene(OpenGLWindow&);

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

