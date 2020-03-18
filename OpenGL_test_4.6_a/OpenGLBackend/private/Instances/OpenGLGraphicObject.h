#pragma once
#include "Scene/SceneObjects/SceneObject.h"
#include "OpenGLBackend/private/OpenGLScene.h"

class OpenGLGraphicObject :
	public virtual SceneObject
{
protected:
	explicit OpenGLGraphicObject(OpenGLScene&);

	OpenGLScene& getScene() const { return m_scene; }

public:
	void setVisibility(const bool newVisitiblity) override;
	bool isVisible() const override;

	void setTransformation(const Matrix4x4& newTransformation) override;
	Matrix4x4 getTransformation() const override;

public:
	virtual void render() = 0;

protected:
	OpenGLScene& m_scene;

	bool m_isVisible = false;
	Matrix4x4 m_modelMatrix = unitMatrix4x4;
};

