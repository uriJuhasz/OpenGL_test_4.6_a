#include "View.h"

#include "Backend/Scene/Scene.h"
#include "Backend/Scene/Camera.h"
#include "Backend/Scene/SceneObjects/SceneMesh.h"
#include "Backend/Scene/SceneObjects/SceneBezierPatch.h"
#include "Backend/Scene/SceneObjects/SceneSphere.h"

#include "Backend/BackendWindow.h"

#include "Geometry/BezierPatch.h"

#include "Utilities/Misc.h"

#include <string>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class ViewImpl final : public View
{
public:
    explicit ViewImpl(BackendWindow& backendWindow);

    virtual ~ViewImpl();

    void setupScene() override;
    void renderScene() override
    {
        m_scene->render();
    }

    void mouseWheelCallback(const Vector2& wheelDelta) override;
    void mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos) override;

public:
    void setMesh(unique_ptr<Mesh> m) override { m_mesh = move(m); }

public:
    BackendWindow& m_backendWindow;

public: //Mesh
    unique_ptr<Scene> m_scene;
    shared_ptr<Mesh> m_mesh;
    shared_ptr<BezierPatch> m_bezierPatch;
    array<Vector3, 2> m_meshBoundingBox;

};

View* View::makeView(BackendWindow& backendWindow)
{
    return new ViewImpl(backendWindow);
}

ViewImpl::ViewImpl(BackendWindow& backendWindow)
    : m_backendWindow(backendWindow)
    , m_scene(backendWindow.makeScene())
{
    backendWindow.registerView(this);
}

ViewImpl::~ViewImpl()
{
    m_backendWindow.registerView(nullptr);
}

array<Vector3, 2> calculateBoundingBox(const vector<Vector3>& vertices)
{
    if (vertices.size() > 0)
    {
        array<Vector3, 2> boundingBox{ vertices[0],vertices[0] };
        const auto numVertices = toInt(vertices.size());
        for (int vi = 1; vi < numVertices; ++vi)
        {
            const auto& vertex = vertices[vi];
            for (int i = 0; i < 3; ++i)
            {
                boundingBox[0][i] = min(boundingBox[0][i], vertex[i]);
                boundingBox[1][i] = max(boundingBox[1][i], vertex[i]);
            }
        }
        return boundingBox;
    }
    return array<Vector3, 2>();
}
void ViewImpl::setupScene()
{
    auto& scene = *m_scene;
    /////////////////////////////////////////
    /////////////////////////////////////////
    //Setup lights
    {
        auto& pointLight0 = scene.addPointLight();
        auto& pointLight1 = scene.addPointLight();
        const Vector3 target(0.0f, 0.0f, 0.0f);
        pointLight0.setPosition(target + Vector3(0.0f, 100.0f, 0.0f));
        pointLight1.setPosition(target + Vector3(100.0f, -100.0f, 0.0f));
    }

    ////////////////////////////////////////////////////////////////
    //Sphere
    {
        const vector<Vector4> patchParameters = {
            Vector4(2.0f, 0.0f, 0.0f, 1.0f),
            Vector4(-5.0f, 0.0f, 0.0f, 2.0f),
            Vector4(7.0f, 0.0f, 0.0f, 3.0f),
            Vector4(0.0f, 10.0f, 0.0f, 5.0f),
            Vector4(0.0f, -100.0f, 0.0f, 10.0f)
        };
        for (const auto sphereDef : patchParameters)
        {
            auto& sphere = m_scene->addSphere(Vector3(sphereDef[0], sphereDef[1], sphereDef[2]), sphereDef[3]);
            sphere.setVisibility(false);
            sphere.setFaceVisibility(true);
            sphere.setEdgeVisibility(false);
        }
    }

    /////////////////////////////////////////
    /////////////////////////////////////////
    //Mesh setup
    {
        auto& mesh = *m_mesh;
        mesh.calculateTopology();
        auto& sceneMeshObject = scene.addMesh(m_mesh);
        sceneMeshObject.setVisibility(false);
        sceneMeshObject.setFaceVisibility(true);
        sceneMeshObject.setEdgeVisibility(false);

        { //Bounding box
            const auto& vertices = mesh.m_vertices;

            const auto boundingBox = calculateBoundingBox(vertices);
            m_meshBoundingBox = boundingBox;
            const auto modelCenter = (boundingBox[0] + boundingBox[1]) / 2;
            const auto modelRadius = length(boundingBox[1] - boundingBox[0]) * 0.5f;

            auto camera = scene.getCamera();
            camera.m_target = modelCenter;
            camera.m_position = modelCenter - Vector3(1.0f, 0.0f, 0.0f) * modelRadius * 1.5f;
            scene.setCamera(camera);
        }

        ////////////////////////////////////////////////////////////////
        //Bezier patch
        {
            const array<Vector3, 16> patchParameters = {
                Vector3(0,2,0), Vector3(1, 1,0), Vector3(2, 1,0), Vector3(3, 2,0),
                Vector3(0,1,1), Vector3(1,-2,1), Vector3(2, 1,1), Vector3(3, 0,1),
                Vector3(0,0,2), Vector3(1, 1,2), Vector3(2, 0,2), Vector3(3,-1,2),
                Vector3(0,0,3), Vector3(1, 1,3), Vector3(2,-1,3), Vector3(3,-1,3)
            };
            m_bezierPatch = make_shared<BezierPatch>(patchParameters);
            auto& sceneBezierPatch = scene.addBezierPatch(m_bezierPatch);
            sceneBezierPatch.setVisibility(true);
            sceneBezierPatch.setFaceVisibility(true);
            sceneBezierPatch.setEdgeVisibility(false);
            sceneBezierPatch.setModelMatrix(makeTranslationMatrix({ -1.5f,0.0f,-1.5f }));
            sceneBezierPatch.setFaceFrontColor(ColorRGBA::Red);
            sceneBezierPatch.setFaceBackColor(ColorRGBA::Blue);
            sceneBezierPatch.setEdgeColor(ColorRGBA::White);

            auto camera = scene.getCamera();
            camera.m_target = Vector3();
            camera.m_position = -Vector3(0.0f, 0.0f, 1.0f) * 10.0f;
            scene.setCamera(camera);
        }

    }
}

void ViewImpl::mouseWheelCallback(const Vector2& wheelDelta)
{
    auto sceneCamera = m_scene->getCamera();
    const auto dy = wheelDelta[1];
    constexpr float factor = 1.0f;
    const auto forward = sceneCamera.m_target - sceneCamera.m_position;
    const auto distance = length(forward);
    constexpr float minimalDistance = 2.0f;
    const auto newDistance = max(distance - dy * factor, minimalDistance);
    sceneCamera.m_position = sceneCamera.m_target - normalize(forward) * newDistance;
    m_scene->setCamera(sceneCamera);
    m_backendWindow.requestUpdate();
}
void ViewImpl::mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos)
{
    if (m_backendWindow.isMiddleMouseButtonPressed())
    {
        auto sceneCamera = m_scene->getCamera();

        constexpr float factor = 0.005f;
        auto viewMatrix = sceneCamera.makeViewMatrix();
        const auto forwardLength = length(sceneCamera.m_target - sceneCamera.m_position);
        auto viewMatrixInverse = transpose(viewMatrix);
        viewMatrix.at(2, 3) -= forwardLength; //Move target to origin
        for (int i = 0; i < 3; ++i)
        {
            viewMatrixInverse.at(i, 3) = sceneCamera.m_target[i];
            viewMatrixInverse.at(3, i) = 0.0f;
        }

        const auto rotationMatrix = makeXRotationMatrix(-factor * delta[1]) * makeYRotationMatrix(factor * delta[0]);
        const auto matrix = viewMatrixInverse * rotationMatrix * viewMatrix;
        sceneCamera.m_position = mulHomogeneous(matrix, sceneCamera.m_position);
        sceneCamera.m_up = mulHomogeneous(matrix, sceneCamera.m_up);

        m_scene->setCamera(sceneCamera);
        m_backendWindow.requestUpdate();

    }
    else if (m_backendWindow.isRightMouseButtonPressed())
    {
    }
}
