#pragma once

#include "SceneObjects/SceneObject.h"
#include "SceneObjects/SceneMesh.h"
#include "SceneObjects/SceneBezierPatch.h"
#include "SceneObjects/SceneSphere.h"

#include "Camera.h"
#include "PointLight.h"

#include <vector>
#include <memory>

class BackendWindow;

class Scene
{
protected:
    Scene() {}
public:
    virtual ~Scene() {}

public:
    virtual const Camera& getCamera() const = 0;
    virtual void setCamera(const Camera& camera) = 0;

public:
    virtual SceneMesh       & addMesh(const std::shared_ptr<const Mesh>& mesh) = 0;
    virtual SceneBezierPatch& addBezierPatch(const std::shared_ptr<const BezierPatch>& patch) = 0;
    virtual SceneSphere     & addSphere(const Vector3& center, const float radius) = 0;

public:
    virtual PointLight& addPointLight() = 0;

public:
    virtual void render() const = 0;

};

