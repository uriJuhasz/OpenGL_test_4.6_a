#pragma once
#include "OpenGLSurface.h"
#include "Backend/Scene/SceneObjects/SceneSphere.h"

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
	void renderMain() override;

public:
	void setRadius(const float) override;
	float getRadius() const override;

	Vector3 getCenter() const override;

public:
	BoundingBox getBoundingBox() const override;

private:
	float m_radius = 1.0f;
};

