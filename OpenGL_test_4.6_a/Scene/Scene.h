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
public:
    virtual const Camera& getCamera() const = 0;

public:
    virtual void setCamera(const Camera& camera) = 0;

public:
    virtual SceneMesh& addMesh(const Mesh& mesh) = 0;
    virtual SceneBezierPatch& addBezierPatch(const BezierPatch& patch) = 0;
    virtual SceneSphere& addSphere(const Vector3& center, const float radius) = 0;

public:
    virtual void render() const = 0;

public:
    static Scene* makeScene(BackendWindow& window);
};

