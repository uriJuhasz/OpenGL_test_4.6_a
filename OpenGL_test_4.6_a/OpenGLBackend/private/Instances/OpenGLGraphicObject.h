#pragma once
#include "Backend/GraphicObjects/BackendGraphicObject.h"

class OpenGLGraphicObject :
	public virtual BackendGraphicObject
{
public:
	void setVisibility(const bool isVisible) override;

	void setModelMatrix(const Matrix4x4& newTransformation) override;

public:
	virtual void render() = 0;

protected:
	bool m_isVisible = false;
	Matrix4x4 m_modelMatrix = unitMatrix4x4;
};

