#pragma once
#include "Backend/BackendStandardShaderProgram.h"

class BackendMesh
{
public:
	virtual void setFaceShader(const BackendStandardShaderProgram* faceShader) = 0;
	virtual void setEdgeShader(const BackendStandardShaderProgram* edgeShader) = 0;

	virtual void render(const bool renderFaces, const bool renderEdges = false) = 0;
};

