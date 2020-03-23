#pragma once
#include "Geometry/BezierPatch.h"
#include "OpenGLPrimitive.h"
#include <GL/glew.h>

class OpenGLWindow;

class OpenGLBezierPatchPrimitive
	: public OpenGLPrimitive
{
public:
	OpenGLBezierPatchPrimitive(OpenGLScene& scene, const std::shared_ptr<const BezierPatch>);
	~OpenGLBezierPatchPrimitive();

	void render(const bool renderFaces, const bool renderEdges = false) const;

public:
	OpenGLTessellationShaderProgram& getFaceShader() const;
	OpenGLTessellationShaderProgram& getEdgeShader() const;

public:
	const BezierPatch& getPatch() const;

private:
	const std::shared_ptr<const BezierPatch> m_patchPtr;

	GLuint m_vertexArrayObject;

	bool m_cullBackfaces = false;
};

