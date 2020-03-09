#pragma once
#include "BackendTesselationShaderProgram.h"

class BackendPatch
{
public:
	virtual void setFaceShader(const BackendTesselationShaderProgram* faceShader) = 0;
	virtual void setEdgeShader(const BackendTesselationShaderProgram* edgeShader) = 0;

	virtual void render(const bool renderFaces, const bool renderEdges = false) = 0;

	virtual void setCullBackfaces(const bool) = 0;
};

