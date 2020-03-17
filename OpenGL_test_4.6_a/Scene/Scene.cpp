#include "Scene.h"
#include "Geometry/Mesh.h"
#include "Geometry/BezierPatch.h"

#include "Backend/GraphicObjects/BackendMesh.h"
#include "Backend/BackendWindow.h"

#include "Scene/private/SceneObjectImplementation/SceneMeshImpl.h"
#include "Scene/private/SceneObjectImplementation/SceneBezierPatchImpl.h"
#include "Scene/private/SceneObjectImplementation/SceneSphereImpl.h"

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

    template<class O, class ...Args> O& addObject(const O*, Args... args)
    {
        auto ptr = make_unique<O>(args...);
        auto& ref = *ptr;
        m_sceneObjects.emplace_back(move(ptr));
        return ref;
    }
    SceneMeshImpl& addMesh(const Mesh& mesh) override
    {
        return addObject((SceneMeshImpl*)nullptr, mesh);
    }
    SceneBezierPatchImpl& addBezierPatch(const BezierPatch& bezierPatch) override
    {
        return addObject((SceneBezierPatchImpl*)nullptr, bezierPatch);
    }
    SceneSphereImpl& addSphere(const Vector3& center, const float radius) override
    {
        return addObject((SceneSphereImpl*)nullptr, center,radius);
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
    vector<unique_ptr<SceneObjectImpl>> m_sceneObjects;
};


Scene* Scene::makeScene(BackendWindow& window)
{
    return new SceneImpl(window);
}

