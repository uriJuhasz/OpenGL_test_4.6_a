#pragma once
#include "Geometry/BezierPatch.h"
#include "OpenGLPrimitive.h"
#include <GL/glew.h>

class OpenGLWindow;

class OpenGLBezierPatchPrimitive
	: public OpenGLPrimitive
{
public:
	OpenGLBezierPatchPrimitive(OpenGLScene& scene, const BezierPatch&);
	~OpenGLBezierPatchPrimitive();

	void render(const bool renderFaces, const bool renderEdges = false) const;

private:
	GLuint m_vertexArrayObject;
	GLuint m_vertexBuffer;

	int m_numVertices = 0;
};

