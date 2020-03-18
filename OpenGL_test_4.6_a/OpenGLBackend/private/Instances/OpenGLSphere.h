#pragma once
#include "OpenGLSurface.h"
#include "Scene/SceneObjects/SceneSphere.h"

#include "OpenGLBackend/private/OpenGLWindow.h"

class OpenGLSphere
	: public virtual OpenGLSurface
	, public virtual SceneSphere
{
public:
	explicit OpenGLSphere(OpenGLScene& scene, const float radius);

public:
	SceneObject& createInstance() const override;

public:
	void render() override;

public:
	void setRadius(const float) override;
	float getRadius() const override;

	Vector3 getCenter() const override;

private:
	float m_radius = 1.0f;
};

