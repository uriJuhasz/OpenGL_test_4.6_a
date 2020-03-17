#include "Scene.h"
#include "Geometry/Mesh.h"
#include "Geometry/BezierPatch.h"

#include "Backend/GraphicObjects/BackendMesh.h"
#include "Backend/BackendWindow.h"

#include "Scene/SceneObjectImplementation/SceneMeshObjectImpl.h"

#include "Utilities/Misc.h"

#include <vector>
#include <memory>
using namespace std;

class SceneImpl final : public Scene
{
public:
    SceneImpl(BackendWindow& window)
        : m_backendWindow(window)
    {}

private:
    const Camera& getCamera() const override
    {
        return m_sceneCamera;
    }
    void setCamera(const Camera& camera) override
    {
        m_sceneCamera = camera;
    }

private:
    BackendWindow& getBackendWindow() { return m_backendWindow; }
private:
    class SceneMesh final 
    {
    public:
        SceneMesh(SceneImpl& scene, unique_ptr<Mesh> mesh)
            : m_mesh(move(mesh))
        {
            m_backendMesh = &scene.getBackendWindow().makeMesh(*m_mesh);
        }
        unique_ptr<Mesh> m_mesh;
        BackendMesh* m_backendMesh;
    };
    MeshID addMesh(unique_ptr<Mesh> mesh) override
    {
        const MeshID meshID(toInt(m_meshes.size()));
        m_meshes.emplace_back(*this,move(mesh));
    }

    SceneMeshObjectImpl& addMeshObject(const MeshID id) override
    {
        SceneMeshObjectImpl& meshObject = *m_meshObjects.emplace_back(make_unique<SceneMeshObjectImpl>(*m_meshes[id.id].m_mesh));
        return meshObject;
    }

private:
    void render() const override
    {
        m_backendWindow.setViewMatrix(m_sceneCamera.makeViewMatrix());
        const auto viewportDimensions = m_backendWindow.getViewportDimensions();
        m_backendWindow.setProjectionMatrix(m_sceneCamera.makeProjectionMatrix(viewportDimensions[0] / viewportDimensions[1]));
        
        m_backendWindow.render();
    }
private:
    BackendWindow& m_backendWindow;
    Camera m_sceneCamera;
    vector<unique_ptr<PointLight>> m_pointLights;
    vector<unique_ptr<SceneObject>> m_objects;
    vector<unique_ptr<SceneMeshObjectImpl>> m_meshObjects;

    vector<SceneMesh> m_meshes;
};


Scene* Scene::makeScene(BackendWindow& window)
{
    return new SceneImpl(window);
}

