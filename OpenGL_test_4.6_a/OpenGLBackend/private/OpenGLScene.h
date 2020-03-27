#pragma once
#include "Backend/Scene/Scene.h"
#include "OpenGLShaderProgram.h"
#include <memory>

class OpenGLWindow;
class OpenGLMeshInstance;
class OpenGLBezierPatchInstance;

class OpenGLScene :
	public virtual Scene
{
public:
	static std::unique_ptr<OpenGLScene> makeScene(OpenGLWindow&);

public:
	virtual OpenGLStandardShaderProgram& getPointsShader() const = 0;

public:
	virtual OpenGLStandardShaderProgram& getBoundingBoxShader() const = 0;

public:
	virtual OpenGLStandardShaderProgram& getMeshFaceShader() const = 0;
	virtual OpenGLStandardShaderProgram& getMeshEdgeShader() const = 0;

public:
	virtual OpenGLTessellationShaderProgram& getBezierPatchFaceShader() const = 0;
	virtual OpenGLTessellationShaderProgram& getBezierPatchEdgeShader() const = 0;

public:
	virtual OpenGLTessellationShaderProgram& getSphereFaceShader() const = 0;
	virtual OpenGLTessellationShaderProgram& getSphereEdgeShader() const = 0;

public:
	virtual OpenGLMeshInstance& makeInstance(const OpenGLMeshInstance&) = 0;
	virtual OpenGLBezierPatchInstance& makeInstance(const OpenGLBezierPatchInstance&) = 0;
};

