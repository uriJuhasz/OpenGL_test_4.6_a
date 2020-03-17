#include "View.h"

#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Scene/SceneObjects/SceneMesh.h"
#include "Scene/SceneObjects/SceneBezierPatch.h"
#include "Scene/SceneObjects/SceneSphere.h"

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
    void renderScene() override;

    void mouseWheelCallback(const Vector2& wheelDelta) override;
    void mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos) override;

public:
    void setMesh(unique_ptr<Mesh> m) override { m_mesh = move(m); }

public: //Mesh
    unique_ptr<Scene> m_scene;
    unique_ptr<Mesh> m_mesh;
    array<Vector3, 2> m_meshBoundingBox;

public: //Bezier patch
    SceneBezierPatch* m_bezierPatch;

public: //Sphere

public: //Scene parameters
    Vector3 m_light0Position, m_light1Position;

    Camera m_sceneCamera;
public:
    BackendWindow& m_backendWindow;
};

View* View::makeView(BackendWindow& backendWindow)
{
    return new ViewImpl(backendWindow);
}

ViewImpl::ViewImpl(BackendWindow& backendWindow)
    : m_backendWindow(backendWindow)
    , m_scene(Scene::makeScene(backendWindow))
{
    backendWindow.registerView(this);
    backendWindow.init();
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
    /////////////////////////////////////////
    /////////////////////////////////////////
    //Setup lights
    {
        const Vector3 target(0.0f, 0.0f, 0.0f);
        m_light0Position = target + Vector3(0.0f, 100.0f, 0.0f);
        m_light1Position = target + Vector3(0.0f, -1000.0f, 0.0f);
    }

    /////////////////////////////////////////
    /////////////////////////////////////////
    //Mesh setup
    {
        auto& mesh = *m_mesh;
        mesh.calculateTopology();
        auto& scene = *m_scene;
        const auto sceneMeshObject = scene.addMesh(mesh);
//        m_backendMesh.reset(m_backendWindow.makeBackendMesh(mesh));
//        const auto& meshObject = scene.addSceneObject();

        { //Bounding box
            const auto& vertices = mesh.m_vertices;

            const auto boundingBox = calculateBoundingBox(vertices);
            m_meshBoundingBox = boundingBox;
            const auto modelCenter = (boundingBox[0] + boundingBox[1]) / 2;
            const auto modelRadius = length(boundingBox[1] - boundingBox[0]) * 0.5f;

            auto& camera = m_sceneCamera;
            camera.m_target = modelCenter;
            camera.m_position = modelCenter - Vector3(1.0f, 0.0f, 0.0f) * modelRadius * 1.5f;
        }

        const auto modelMatrix = unitMatrix4x4;
        {
            auto& shaderProgram = *m_meshFaceShaderProgram;

            shaderProgram.setParameter("modelMatrix", modelMatrix);

            shaderProgram.setParameter("light0Position", m_light0Position);
            shaderProgram.setParameter("light0Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light0SpecularExponent", 10.0f);
            shaderProgram.setParameter("light1Position", m_light1Position);
            shaderProgram.setParameter("light1Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light1SpecularExponent", 100.0f);
        }
        {
            m_backendMesh->setEdgeShader(m_meshEdgeShaderProgram.get());
            auto& shaderProgram = *m_meshEdgeShaderProgram;
        }

        {

            GLuint vertexArrayObjectID = 0;
            {
                glCreateVertexArrays(1, &vertexArrayObjectID);
                glBindVertexArray(vertexArrayObjectID);
            }
            {
                const auto values = vector<Vector3>{ m_meshBoundingBox[0],m_meshBoundingBox[1] };
                const int numValues = toInt(values.size());
                const auto valueSize = sizeof(values[0]);
                const auto attributeIndex = 0;
                const auto D = 3;
                GLuint bufferID = 0;
                glCreateBuffers(1, &bufferID);
                glNamedBufferStorage(bufferID, numValues * valueSize, values.data(), 0);
                glVertexArrayVertexBuffer(vertexArrayObjectID, attributeIndex, bufferID, 0, valueSize);
                glEnableVertexArrayAttrib(vertexArrayObjectID, attributeIndex);
                glVertexArrayAttribFormat(vertexArrayObjectID, attributeIndex, D, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(vertexArrayObjectID, attributeIndex, attributeIndex);
            }

            {
                const array<int, 2> indexArray = { 0,1 };
                const auto indexSize = sizeof(indexArray[0]);
                const auto numIndices = indexArray.size();
                GLuint indexBufferID;
                glCreateBuffers(1, &indexBufferID);
                glNamedBufferStorage(indexBufferID, numIndices * indexSize, indexArray.data(), GL_DYNAMIC_STORAGE_BIT);
                glVertexArrayElementBuffer(vertexArrayObjectID, indexBufferID);
            }
            m_meshBoundingBoxVertexArrayObjectID = vertexArrayObjectID;

            m_meshBoundingboxShaderProgram = backendContext.makeStandardShaderProgram("BoundingBoxShaderProgram.glsl", "boundingBoxLine");
            auto& shaderProgram = *m_meshBoundingboxShaderProgram;

            shaderProgram.setParameter("modelMatrix", modelMatrix);
        }
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
        auto& scene = *m_scene;
        m_bezierPatch = &scene.addBezierPatch(new BezierPatch(patchParameters));
        m_backendBezierPatch.reset(m_backendWindow.makeBackendPatch(*m_bezierPatch));

        m_backendBezierPatch->setFaceShader(m_bezierShaderProgram.get());
        m_backendBezierPatch->setEdgeShader(m_bezierEdgeShaderProgram.get());

        const Matrix4x4 modelMatrix = makeTranslationMatrix({ -1.5f,0.0f,-1.5f });
        {
            auto& shaderProgram = *m_bezierShaderProgram;

            shaderProgram.setParameter("modelMatrix", modelMatrix);

            const Vector3 target(0.0f, 0.0f, 0.0f);

            shaderProgram.setParameter("light0Position", m_light0Position);
            shaderProgram.setParameter("light0Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light0SpecularExponent", 10.0f);
            shaderProgram.setParameter("light1Position", m_light1Position);
            shaderProgram.setParameter("light1Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light1SpecularExponent", 100.0f);

            const Vector3 frontColor(1.0f, 0.0f, 1.0f);
            shaderProgram.setParameter("frontColor", frontColor);
            const Vector3 backColor(0.1f, 0.3f, 0.3f);
            shaderProgram.setParameter("backColor", backColor);

            shaderProgram.setParameter("maxTessellationLevel", backendContext.getMaxTessellationLevel());
            shaderProgram.setParameter("desiredPixelsPerTriangle", 20.0f);
        }
        {
            auto& shaderProgram = *m_bezierEdgeShaderProgram;

            shaderProgram.setParameter("modelMatrix", modelMatrix);

            shaderProgram.setParameter("edgeColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));

            shaderProgram.setParameter("maxTessellationLevel", backendContext.getMaxTessellationLevel());
            shaderProgram.setParameter("desiredPixelsPerTriangle", 20.0f);
        }
    }

    ////////////////////////////////////////////////////////////////
    //Sphere
    {
        const vector<Vector4> patchParameters = {
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
            Vector4(-20.0f, 0.0f, 0.0f, 2.0f),
            Vector4(20.0f, 0.0f, 0.0f, 5.0f),
            Vector4(0.0f, 20.0f, 0.0f, 10.0f),
            Vector4(0.0f, -30.0f, -.0f, 20.0f)
        };
        for (const auto sphereDef : patchParameters)
            m_scene->addSphere(Vector3(sphereDef[0], sphereDef[1], sphereDef[2]), sphereDef[3]);
    }
}
using std::clamp;

void ViewImpl::renderScene()
{
    const auto viewportDimensions = m_backendWindow.getViewportDimensions();

    const auto sceneCamera = m_sceneCamera;
    const auto viewMatrix = sceneCamera.makeViewMatrix();
    const auto projectionMatrix = sceneCamera.makeProjectionMatrix(viewportDimensions[0] / viewportDimensions[1]);

    //////////////////////
    constexpr bool renderSphere = false;
    constexpr bool showBezierPatch = false;
    constexpr bool renderMesh = true;
    constexpr bool renderMeshFaces = renderMesh && true;
    constexpr bool renderWireframe = renderMesh && true;
    constexpr bool showBoundingBox = renderMesh && true;

    //////////////////////
    //Patch sphere
    if (renderSphere)
    {
        auto& shaderProgram = *m_sphereShaderProgram;

        {
            const auto pixelWidth = m_backendWindow.getFramebufferSize()[0];
            shaderProgram.setParameter("pixelWidth", pixelWidth);
        }

        m_backendSpherePatch->render(false, true);
    }

    //////////////////////
    //Bezier patch
    if (showBezierPatch)
    {
        {
            auto& shaderProgram = *m_bezierShaderProgram;
            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);

            shaderProgram.setParameter("viewerPosition", sceneCamera.m_position);

            {
                const auto pixelWidth = m_backendWindow.getFramebufferSize()[0];
                shaderProgram.setParameter("pixelWidth", pixelWidth);
            }

            m_backendBezierPatch->render(true, false);
        }
        {
            auto& shaderProgram = *m_bezierEdgeShaderProgram;
            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);

            {
                const auto pixelWidth = m_backendWindow.getFramebufferSize()[0];
                shaderProgram.setParameter("pixelWidth", pixelWidth);
            }

            m_backendBezierPatch->render(false, true);
        }
    }
    
    //////////////////////
    //Render mesh
    if (renderMesh)
    {
        for (int i = 0; i < 2; ++i)
        {
            const auto modelMatrix = (i == 0)
                ? makeTranslationMatrix(Vector3(-20, 0, 0))
                : makeTranslationMatrix(Vector3( 20, 0, 0));

            if (renderMeshFaces)
            {
                auto& shaderProgram = *m_meshFaceShaderProgram;

                shaderProgram.setParameter("modelMatrix", modelMatrix);
                shaderProgram.setParameter("viewMatrix", viewMatrix);
                shaderProgram.setParameter("projectionMatrix", projectionMatrix);
                shaderProgram.setParameter("viewerPosition", sceneCamera.m_position);

                m_backendMesh->render(true, false);
            }

            if (renderWireframe)
            {
                auto& shaderProgram = *m_meshEdgeShaderProgram;

                shaderProgram.setParameter("modelMatrix", modelMatrix);
                shaderProgram.setParameter("viewMatrix", viewMatrix);
                shaderProgram.setParameter("projectionMatrix", projectionMatrix);

                shaderProgram.setParameter("edgeColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));

                m_backendMesh->render(false, true);
            }
        }
        ///////////////////////
        //Bounding box
        if (showBoundingBox)
        {
            glBindVertexArray(m_meshBoundingBoxVertexArrayObjectID);
            auto& shaderProgram = *m_meshBoundingboxShaderProgram;
            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);

            shaderProgram.setParameter("edgeColor", Vector4(1.0f, 0.0f, 0.0f, 1.0f));

            glDrawArrays(GL_LINES, 0, 2);
        }
    }
}

void ViewImpl::mouseWheelCallback(const Vector2& wheelDelta)
{
    auto& sceneCamera = m_sceneCamera;
    const auto dy = wheelDelta[1];
    constexpr float factor = 1.0f;
    const auto forward = sceneCamera.m_target - sceneCamera.m_position;
    const auto distance = length(forward);
    constexpr float minimalDistance = 2.0f;
    const auto newDistance = max(distance - dy * factor, minimalDistance);
    sceneCamera.m_position = sceneCamera.m_target - normalize(forward) * newDistance;
    m_backendWindow.requestUpdate();
}
void ViewImpl::mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos)
{
    if (m_backendWindow.isMiddleMouseButtonPressed())
    {
        auto& sceneCamera = m_sceneCamera;

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
        m_backendWindow.requestUpdate();

    }
    else if (m_backendWindow.isRightMouseButtonPressed())
    {
    }
}
