#pragma once

#include "SceneObjects/SceneObject.h"
#include "SceneObjects/SceneMeshObject.h"

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
    class MeshID final { private: friend class SceneImpl; MeshID(const int id) : id(id) {} const int id; };
    virtual MeshID addMesh(std::unique_ptr<Mesh> mesh) = 0;
    virtual SceneMeshObject& addMeshObject(const MeshID id) = 0;

public:
    virtual void render() const = 0;

public:
    static Scene* makeScene(BackendWindow& window);
};

