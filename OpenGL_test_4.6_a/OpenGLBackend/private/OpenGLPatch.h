#pragma once

#include "Backend/BackendPatch.h"

#include "OpenGLShaderProgram.h"

#include <Geometry/Patch.h>

class OpenGLPatch final : public virtual BackendPatch
{
public:
	OpenGLPatch(const Patch& patch);
	~OpenGLPatch();

	void setFaceShader(const BackendTesselationShaderProgram* faceShader) override;
	void setEdgeShader(const BackendTesselationShaderProgram* edgeShader) override;


	void render(const bool renderFaces, const bool renderEdges = false) override;

	void setCullBackfaces(const bool) override;

private:
	GLuint m_vertexArrayObject;
	GLuint m_vertexBuffer;

	bool m_cullBackfaces = false;

	int m_numVertices = 0;
	int m_numVerticesPerPatch = 0;

	const OpenGLTessellationShaderProgram* m_faceShader = nullptr;
	const OpenGLTessellationShaderProgram* m_edgeShader = nullptr;
};

