#pragma once
#include "Backend/Scene/SceneObjects/SceneObject.h"
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

	void setModelMatrix(const Matrix4x4& newTransformation) override;
	Matrix4x4 getModelMatrix() const override;

	void setBoundingBoxVisibility(const bool) override;

public:
	void render();

	virtual void renderMain() = 0;

protected:
	OpenGLScene& m_scene;

	bool m_isVisible = false;
	Matrix4x4 m_modelMatrix = unitMatrix4x4;

	bool m_boundingBoxVisibility = true;

protected:
	class BoundingBox final
	{
	public:
		BoundingBox(const Vector3& min, const Vector3& max) : m_box{ min,max } {}
		BoundingBox(const std::array<Vector3, 2>& bb) : m_box{ bb } {}

		std::array<Vector3, 2> get() const { return m_box; }
	private:
		std::array<Vector3, 2> m_box;
	};

	virtual BoundingBox getBoundingBox() const = 0;
	BoundingBox calculateBoundingBox(const std::vector<Vector3>& vertices) const;

private:
	void renderBoundingBox();
};

