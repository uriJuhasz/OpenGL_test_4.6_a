#pragma once
#include "Geometry/BezierPatch.h"
#include "OpenGLPrimitive.h"
#include <GL/glew.h>

class OpenGLWindow;

class OpenGLBezierPatchPrimitive
	: public OpenGLPrimitive
{
public:
	OpenGLBezierPatchPrimitive(OpenGLWindow& window, const BezierPatch&);
	~OpenGLBezierPatchPrimitive();

	void render(const bool renderFaces, const bool renderEdges = false);

private:
	GLuint m_vertexArrayObject;
	GLuint m_vertexBuffer;

	int m_numVertices = 0;
	int m_numVerticesPerPatch = 0;
};

