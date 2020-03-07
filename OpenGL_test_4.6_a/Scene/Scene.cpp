#include "Scene.h"
#include <vector>
#include <memory>
using namespace std;

class SceneImpl final : public Scene
{
public:
    const Camera& getCamera() const override
    {
        return m_sceneCamera;
    }
    void setCamera(const Camera& camera) override
    {
        m_sceneCamera = camera;
    }

public:
    Camera m_sceneCamera;
    vector<unique_ptr<PointLight>> m_pointLights;
    vector<unique_ptr<SceneObject>> m_objects;
};

Scene* Scene::makeScene()
{
    return new SceneImpl();
}

